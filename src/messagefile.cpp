/**
 * @file messagefile.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "messagefile.h"
#include "main.h"
#include <o3d/core/fileinfo.h>
#include <o3d/core/filemanager.h>
#include <o3d/core/integer.h>
#include <o3d/core/stringtokenizer.h>
#include <o3d/gui/integervalidator.h>
#include "memberfactory.h"

using namespace o3d;
using namespace o3d::nmg;

MessageFile::MessageFile(const String &filename) :
    m_filename(filename)
{
    Int32 s = m_filename.reverseFind('/');
    m_prefix = m_filename.sub(s+1, m_filename.length() - 4);
}

MessageFile::~MessageFile()
{
    for (std::pair<String,Message*> entry : m_messages)
    {
        deletePtr(entry.second);
    }

    for (std::pair<String,Message*> entry : m_imports)
    {
        deletePtr(entry.second);
    }
}

const String &MessageFile::getName() const
{
    return m_prefix;
}

void MessageFile::parse()
{
    System::print(m_filename, "Parse message file");

    InStream *is = FileManager::instance()->openInStream(m_filename);

    try {
        parse(is, m_messages);
    } catch (E_BaseException &e)
    {
        System::print(e.getMsg(), e.getDescr() + " in " + m_filename, System::MSG_ERROR);

        deletePtr(is);
        O3D_ERROR(E_InvalidFormat(String("Error parsing ") + m_filename));
    }

    deletePtr(is);

    // compute the min size of each message
    for (std::pair<String, Message*> message : m_messages)
    {
        for (Member *member : message.second->members)
        {
            message.second->minSize += member->getMinSize();
        }
    }
}

void MessageFile::process()
{
    System::print(m_filename, "Process message file");

    // attribute id to messages
    for (std::pair<String, Message*> entry : m_messages)
    {
        // auto id
        if (entry.second->id == 0)
            entry.second->id = Main::instance()->getNextMessageId();
    }

    // write for profiles
    for (Int32 profile = 0; profile < 2; ++profile)
    {
        // headers
        //m_msgInClassIncludes.clear();
        //m_msgInClassIncludes.push_back("<o3d/net/NetMessageAdapter.h>");

        m_msgInImplIncludes.clear();
        m_msgInImplIncludes.push_back(Main::instance()->getMessageFactoryHeader((Main::Profile)profile));
        m_msgInImplIncludes.push_back("<o3d/net/NetBuffer.h>");

        m_msgOutImplIncludes.clear();
        m_msgOutImplIncludes.push_back("<o3d/net/NetBuffer.h>");

        writeMessageInClass(Main::instance()->getOutPath((Main::Profile)profile), Main::instance()->getHppExt(), (Profile)profile);
        writeMessageInImpl(Main::instance()->getOutPath((Main::Profile)profile), Main::instance()->getCppExt(), (Profile)profile);
        writeMessageInRun(Main::instance()->getOutPath((Main::Profile)profile), Main::instance()->getCppExt(), (Profile)profile);

        writeMessageOutClass(Main::instance()->getOutPath((Main::Profile)profile), Main::instance()->getHppExt(), (Profile)profile);
        writeMessageOutImpl(Main::instance()->getOutPath((Main::Profile)profile), Main::instance()->getCppExt(), (Profile)profile);
    }
}

void MessageFile::rename(const String &old, const String &name)
{
    for (StringMap<Message*>::IT it = m_messages.begin(); it != m_messages.end(); ++it)
    {
        if (it->first == old)
        {
            Message *message = it->second;

            m_messages.erase(it);

            it->second->name = name;

            m_messages.insert(std::make_pair(name, message));

            break;
        }
    }
}

void MessageFile::addMember(const String &msgName, Member *member)
{
    for (std::pair<String, Message*> msg : m_messages)
    {
        if (msg.first == msgName)
        {
            msg.second->members.push_back(member);
            break;
        }
    }
}

void MessageFile::parse(InStream *is, StringMap<Message*>& messages)
{
    String line;

    while (is->readLine(line) != EOF)
    {
        // trim white spaces
        line.trimLeftChars(" \t");
        line.trimRightChars(" \t");

        // ignore comment lines
        if (line.startsWith("#"))
            continue;

        if (line.isEmpty())
            continue;

        // import statement
        else if (line.startsWith("import"))
        {
            importMessage(line);
        }
        // a message
        else if (line.startsWith("msg"))
        {
            parseMessage(is, line, messages);
        }
        // an abstract message
        else if (line.startsWith("abstract"))
        {
            parseMessage(is, line, messages);
        }
    }
}

void MessageFile::importMessage(const String &_line)
{
    StringTokenizer tk(_line, " ");
    String import;
    String name;

    if (tk.hasMoreTokens())
        import = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing import declaration in " + m_filename));

    if (tk.hasMoreTokens())
        name = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing import target name in " + m_filename));

    String filename = Main::instance()->getInPath() + "/" + name + ".msg";
    System::print(filename, "Import message file");

    InStream *is = FileManager::instance()->openInStream(filename);

    try {
        parse(is, m_imports);
    } catch (E_BaseException &e)
    {
        System::print(e.getMsg(), e.getDescr() + " in " + filename, System::MSG_ERROR);

        deletePtr(is);
        O3D_ERROR(E_InvalidFormat(String("Error parsing ") + filename));
    }

    deletePtr(is);
}

void MessageFile::writeMessageInClass(const String &outPath, const String &hppExt, Profile profile)
{
    // only if concrete message to export
    Bool something = False;
    for (std::pair<String, Message*> entry : m_messages)
    {
        if ((entry.second->type[profile] == IN || entry.second->type[profile] == BOTH) &&
             !entry.second->abstract)
        {
            something = True;
            break;
        }
    }

    if (!something)
        return;

    FileOutStream *os = FileManager::instance()->openOutStream(
                outPath + "/" + m_prefix + "In." + hppExt,
                FileOutStream::CREATE);

    try {
        for (const String &hppLine : Main::instance()->getHppTpl())
        {
            String outLine = hppLine;
            Int32 p1;
            Int32 p2;

            if (outLine.isEmpty())
                os->writeLine("");

            // block
            else if ((p1 = outLine.sub("@{", 0)) != -1)
            {
                p1 = 0;
                p2 = outLine.find('}', p1+2);

                if (p2 == -1)
                {
                    deletePtr(os);
                    O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in hpp.template"));
                }

                String blockName = outLine.sub(p1+2, p2);
                if (blockName == "license")
                {
                    // license
                    for (const String &line : Main::instance()->getLicenseTpl())
                    {
                        os->writeLine(line);
                    }
                }
                else if (blockName == "content")
                {
                    for (std::pair<String,Message*> entry : m_messages)
                    {
                        if ((entry.second->type[profile] == IN || entry.second->type[profile] == BOTH) &&
                                !entry.second->abstract)
                        {
                            writeMessageInClassContent(os, entry.second, profile);
                        }
                    }
                }
                else if (blockName == "includes")
                {
                    // include
                    for (const String &header : m_msgInClassIncludes)
                    {
                        os->writeLine(String("#include ") + header);
                    }
                }
            }
            else
            {
                // line with 0 or many variables
                parseVariable(outLine, nullptr, m_prefix + "In", profile);
                os->writeLine(outLine);
            }
        }
    } catch (E_BaseException &e)
    {
        deletePtr(os);
        throw;
    }

    deletePtr(os);
}

void MessageFile::writeMessageInImpl(const String &outPath, const String &cppExt, Profile profile)
{
    // only if concrete message to export
    Bool something = False;
    for (std::pair<String, Message*> entry : m_messages)
    {
        if ((entry.second->type[profile] == IN || entry.second->type[profile] == BOTH) &&
             !entry.second->abstract)
        {
            something = True;
            break;
        }
    }

    if (!something)
        return;

    FileOutStream *os = FileManager::instance()->openOutStream(
                outPath + "/" + m_prefix + "In." + cppExt,
                FileOutStream::CREATE);

    try {
        for (const String &cppLine : Main::instance()->getCppTpl())
        {
            String outLine = cppLine;
            Int32 p1;
            Int32 p2;

            if (outLine.isEmpty())
                os->writeLine("");

            // block
            else if ((p1 = outLine.sub("@{", 0)) != -1)
            {
                p1 = 0;
                p2 = outLine.find('}', p1+2);

                if (p2 == -1)
                {
                    deletePtr(os);
                    O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in cpp.template"));
                }

                String blockName = outLine.sub(p1+2, p2);
                if (blockName == "license")
                {
                    // license
                    for (const String &line : Main::instance()->getLicenseTpl())
                    {
                        os->writeLine(line);
                    }
                }
                else if (blockName == "content")
                {
                    for (std::pair<String,Message*> entry : m_messages)
                    {
                        if ((entry.second->type[profile] == IN || entry.second->type[profile] == BOTH) &&
                                !entry.second->abstract)
                        {
                            writeMessageInImplContent(os, entry.second, profile);
                        }
                    }
                }
                else if (blockName == "includes")
                {
                    // include
                    for (const String &header : m_msgInImplIncludes)
                    {
                        os->writeLine(String("#include ") + header);
                    }
                }
            }
            else
            {
                // line with 0 or many variables
                parseVariable(outLine, nullptr, m_prefix + "In", profile);
                os->writeLine(outLine);
            }
        }
    } catch (E_BaseException &e)
    {
        deletePtr(os);
        throw;
    }

    deletePtr(os);
}

void MessageFile::writeMessageInRun(const String &outPath, const String &cppExt, Profile profile)
{
    // only if concrete message to export
    Bool something = False;
    for (std::pair<String, Message*> entry : m_messages)
    {
        if ((entry.second->type[profile] == IN || entry.second->type[profile] == BOTH) &&
             !entry.second->abstract)
        {
            something = True;
            break;
        }
    }

    if (!something)
        return;

    FileInfo fileInfo(outPath + "/" + m_prefix + "In.run." + cppExt);
    if (fileInfo.exists())
        return;

    FileOutStream *os = FileManager::instance()->openOutStream(
                outPath + "/" + m_prefix + "In.run." + cppExt,
                FileOutStream::CREATE);

    try {
        for (const String &cppLine : Main::instance()->getCppTpl())
        {
            String outLine = cppLine;
            Int32 p1;
            Int32 p2;

            if (outLine.isEmpty())
                os->writeLine("");

            // block
            else if ((p1 = outLine.sub("@{", 0)) != -1)
            {
                p1 = 0;
                p2 = outLine.find('}', p1+2);

                if (p2 == -1)
                {
                    deletePtr(os);
                    O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in cpp.template"));
                }

                String blockName = outLine.sub(p1+2, p2);
                if (blockName == "license")
                {
                    // license
                    for (const String &line : Main::instance()->getLicenseTpl())
                    {
                        os->writeLine(line);
                    }
                }
                else if (blockName == "content")
                {
                    for (std::pair<String,Message*> entry : m_messages)
                    {
                        if ((entry.second->type[profile] == IN || entry.second->type[profile] == BOTH) &&
                                !entry.second->abstract)
                        {
                            writeMessageInRunContent(os, entry.second, profile);
                        }
                    }
                }
                else if (blockName == "includes")
                {
                    // include
                    for (const String &header : m_msgInRunIncludes)
                    {
                        os->writeLine(String("#include ") + header);
                    }
                }
            }
            else
            {
                // line with 0 or many variables
                parseVariable(outLine, nullptr, m_prefix + "In", profile);
                os->writeLine(outLine);
            }
        }
    } catch (E_BaseException &e)
    {
        deletePtr(os);
        throw;
    }

    deletePtr(os);
}

void MessageFile::writeMessageInClassContent(OutStream *os, MessageFile::Message *msg, Profile profile)
{
    for (const String &inClassLine : Main::instance()->getMessageInClassTpl())
    {
        String outLine = inClassLine;
        Int32 p1;
        Int32 p2;

        if (outLine.isEmpty())
            os->writeLine("");

        // block
        else if ((p1 = outLine.sub("@{", 0)) != -1)
        {
            p2 = outLine.find('}', p1+2);

            if (p2 == -1)
            {
                deletePtr(os);
                O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in msg.in.class.template"));
            }

            String blockName = outLine.sub(p1+2, p2);
            if (blockName == "private_members")
            {
                for (Member *member : msg->members)
                {
                    if (member->isPrivate())
                        member->writeDecl(os);
                }
            }
            else if (blockName == "public_members")
            {
                for (Member *member : msg->members)
                {
                    if (member->isPublic())
                        member->writeDecl(os);
                }
            }
        }
        else
        {
            // line with 0 or many variables
            parseVariable(outLine, msg, m_prefix + "In", profile);
            os->writeLine(outLine);
        }
    }

    os->writeLine("");
}

void MessageFile::writeMessageInImplContent(OutStream *os, MessageFile::Message *msg, Profile profile)
{
    for (const String &inImplLine : Main::instance()->getMessageInImplTpl())
    {
        String outLine = inImplLine;
        Int32 p1;
        Int32 p2;

        if (outLine.isEmpty())
            os->writeLine("");

        // block
        else if ((p1 = outLine.sub("@{", 0)) != -1)
        {
            p2 = outLine.find('}', p1+2);

            if (p2 == -1)
            {
                deletePtr(os);
                O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in msg.in.class.template"));
            }

            String blockName = outLine.sub(p1+2, p2);
            if (blockName == "readFromBuffer")
            {
                for (Member *member : msg->members)
                {
                    member->writeRead(os);
                }
            }
        }
        else
        {
            // line with 0 or many variables
            parseVariable(outLine, msg, m_prefix + "In", profile);
            os->writeLine(outLine);
        }
    }

    os->writeLine("");
}

void MessageFile::writeMessageInRunContent(OutStream *os, MessageFile::Message *msg, Profile profile)
{
    for (const String &inImplLine : Main::instance()->getMessageInImplRunlTpl())
    {
        String outLine = inImplLine;
        Int32 p1;
        Int32 p2;

        if (outLine.isEmpty())
            os->writeLine("");

        // block
        else if ((p1 = outLine.sub("@{", 0)) != -1)
        {
            p2 = outLine.find('}', p1+2);

            if (p2 == -1)
            {
                deletePtr(os);
                O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in msg.in.class.template"));
            }

            //String blockName = outLine.sub(p1+2, p2);
        }
        else
        {
            // line with 0 or many variables
            parseVariable(outLine, msg, m_prefix + "In", profile);
            os->writeLine(outLine);
        }
    }

    os->writeLine("");
}

void MessageFile::writeMessageOutClass(const String &outPath, const String &hppExt, MessageFile::Profile profile)
{
    // only if concrete message to export
    Bool something = False;
    for (std::pair<String, Message*> entry : m_messages)
    {
        if ((entry.second->type[profile] == OUT || entry.second->type[profile] == BOTH) &&
                !entry.second->abstract)
        {
            something = True;
            break;
        }
    }

    if (!something)
        return;

    FileOutStream *os = FileManager::instance()->openOutStream(
                outPath + "/" + m_prefix + "Out." + hppExt,
                FileOutStream::CREATE);

    // add a default constructor
    for (std::pair<String,Message*> entry : m_messages)
    {
        Member *ctor = MemberFactory::instance()->buildFromTypeName("ctor", nullptr);
        ctor->setName(entry.second->name + "Out");
        ctor->setValue(UInteger32::toString(entry.second->minSize));

        entry.second->members.push_front(ctor);
    }

    try {
        for (const String &hppLine : Main::instance()->getHppTpl())
        {
            String outLine = hppLine;
            Int32 p1;
            Int32 p2;

            if (outLine.isEmpty())
                os->writeLine("");

            // block
            else if ((p1 = outLine.sub("@{", 0)) != -1)
            {
                p1 = 0;
                p2 = outLine.find('}', p1+2);

                if (p2 == -1)
                {
                    deletePtr(os);
                    O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in hpp.template"));
                }

                String blockName = outLine.sub(p1+2, p2);
                if (blockName == "license")
                {
                    // license
                    for (const String &line : Main::instance()->getLicenseTpl())
                    {
                        os->writeLine(line);
                    }
                }
                else if (blockName == "content")
                {
                    for (std::pair<String,Message*> entry : m_messages)
                    {
                        if ((entry.second->type[profile] == OUT || entry.second->type[profile] == BOTH) &&
                                !entry.second->abstract)
                        {
                            writeMessageOutClassContent(os, entry.second, profile);
                        }
                    }
                }
                else if (blockName == "includes")
                {
                    // include
                    for (const String &header : m_msgOutClassIncludes)
                    {
                        os->writeLine(String("#include ") + header);
                    }
                }
            }
            else
            {
                // line with 0 or many variables
                parseVariable(outLine, nullptr, m_prefix + "Out", profile);
                os->writeLine(outLine);
            }
        }
    } catch (E_BaseException &e)
    {
        deletePtr(os);
        throw;
    }

    // remove the default constructor
    for (std::pair<String,Message*> entry : m_messages)
    {
        deletePtr(entry.second->members.front());
        entry.second->members.pop_front();
    }

    deletePtr(os);
}

void MessageFile::writeMessageOutImpl(const String &outPath, const String &cppExt, MessageFile::Profile profile)
{
    // only if concrete message to export
    Bool something = False;
    for (std::pair<String, Message*> entry : m_messages)
    {
        if ((entry.second->type[profile] == OUT || entry.second->type[profile] == BOTH) &&
                !entry.second->abstract)
        {
            something = True;
            break;
        }
    }

    if (!something)
        return;

    FileOutStream *os = FileManager::instance()->openOutStream(
                outPath + "/" + m_prefix + "Out." + cppExt,
                FileOutStream::CREATE);

    try {
        for (const String &cppLine : Main::instance()->getCppTpl())
        {
            String outLine = cppLine;
            Int32 p1;
            Int32 p2;

            if (outLine.isEmpty())
                os->writeLine("");

            // block
            else if ((p1 = outLine.sub("@{", 0)) != -1)
            {
                p1 = 0;
                p2 = outLine.find('}', p1+2);

                if (p2 == -1)
                {
                    deletePtr(os);
                    O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in cpp.template"));
                }

                String blockName = outLine.sub(p1+2, p2);
                if (blockName == "license")
                {
                    // license
                    for (const String &line : Main::instance()->getLicenseTpl())
                    {
                        os->writeLine(line);
                    }
                }
                else if (blockName == "content")
                {
                    for (std::pair<String,Message*> entry : m_messages)
                    {
                        if ((entry.second->type[profile] == OUT || entry.second->type[profile] == BOTH) &&
                                !entry.second->abstract)
                        {
                            writeMessageOutImplContent(os, entry.second, profile);
                        }
                    }
                }
                else if (blockName == "includes")
                {
                    // include
                    for (const String &header : m_msgOutImplIncludes)
                    {
                        os->writeLine(String("#include ") + header);
                    }
                }
            }
            else
            {
                // line with 0 or many variables
                parseVariable(outLine, nullptr, m_prefix + "Out", profile);
                os->writeLine(outLine);
            }
        }
    } catch (E_BaseException &e)
    {
        deletePtr(os);
        throw;
    }

    deletePtr(os);
}

void MessageFile::writeMessageOutClassContent(OutStream *os, MessageFile::Message *msg, MessageFile::Profile profile)
{
    for (const String &outClassLine : Main::instance()->getMessageOutClassTpl())
    {
        String outLine = outClassLine;
        Int32 p1;
        Int32 p2;

        if (outLine.isEmpty())
            os->writeLine("");

        // block
        else if ((p1 = outLine.sub("@{", 0)) != -1)
        {
            p2 = outLine.find('}', p1+2);

            if (p2 == -1)
            {
                deletePtr(os);
                O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in msg.in.class.template"));
            }

            String blockName = outLine.sub(p1+2, p2);
            if (blockName == "private_members")
            {
                for (Member *member : msg->members)
                {
                    if (member->isPrivate())
                        member->writeDecl(os);
                }
            }
            else if (blockName == "public_members")
            {
                for (Member *member : msg->members)
                {
                    if (member->isPublic())
                        member->writeDecl(os);
                }
            }
            else if (blockName == "setters")
            {
                for (Member *member : msg->members)
                {
                    member->writeSetterDecl(os);
                }
            }
        }
        else
        {
            // line with 0 or many variables
            parseVariable(outLine, msg, m_prefix + "Out", profile);
            os->writeLine(outLine);
        }
    }

    os->writeLine("");
}

void MessageFile::writeMessageOutImplContent(OutStream *os, MessageFile::Message *msg, MessageFile::Profile profile)
{
    for (const String &outImplLine : Main::instance()->getMessageOutImplTpl())
    {
        String outLine = outImplLine;
        Int32 p1;
        Int32 p2;

        if (outLine.isEmpty())
            os->writeLine("");

        // block
        else if ((p1 = outLine.sub("@{", 0)) != -1)
        {
            p2 = outLine.find('}', p1+2);

            if (p2 == -1)
            {
                deletePtr(os);
                O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in msg.out.impl.template"));
            }

            String blockName = outLine.sub(p1+2, p2);
            if (blockName == "writeToBuffer")
            {
                for (Member *member : msg->members)
                {
                    member->writeWrite(os);
                }
            }
        }
        else
        {
            // line with 0 or many variables
            parseVariable(outLine, msg, m_prefix + "Out", profile);
            os->writeLine(outLine);
        }
    }

    os->writeLine("");
}

void MessageFile::parseVariable(String &outLine, Message *msg, const String &header, Profile profile)
{
    Int32 p1, p2;

    // line with 0 or many variables
    while ((p1 = outLine.sub("${", 0)) != -1)
    {
        p2 = outLine.find('}', p1+2);

        if (p2 == -1)
            O3D_ERROR(E_InvalidFormat("Missing ending bracket } after @{ in template"));

        String varName = outLine.sub(p1+2, p2);
        outLine.remove(p1, p2-p1+1);

        if (varName == "message")
        {
            outLine.insert(msg->name, p1);
        }
        else if (varName == "messageId")
        {
            outLine.insert(String::print("%i", msg->id), p1);
        }
        else if (varName == "author")
        {
            outLine.insert(Main::instance()->getAuthor(), p1);
        }
        else if (varName == "yyyy")
        {
            outLine.insert(Main::instance()->getYear(), p1);
        }
        else if (varName == "mm")
        {
            outLine.insert(Main::instance()->getMonth(), p1);
        }
        else if (varName == "dd")
        {
            outLine.insert(Main::instance()->getDay(), p1);
        }
        else if (varName == "messageFactoryInstance")
        {
            outLine.insert(Main::instance()->getMessageFactoryInstance((Main::Profile)profile), p1);
        }
        else if (varName == "ns")
        {
            outLine.insert(Main::instance()->getNamespace((Main::Profile)profile), p1);
        }
        else if (varName == "NS")
        {
            String NS = Main::instance()->getNamespace((Main::Profile)profile);
            NS.upper();

            outLine.insert(NS, p1);
        }
        else if (varName == "header")
        {
            outLine.insert(header, p1);
        }
        else if (varName == "hpp")
        {
            outLine.insert(Main::instance()->getHppExt(), p1);
        }
        else if (varName == "FILENAME")
        {
            String FILENAME = header;
            FILENAME.upper();

            outLine.insert(FILENAME, p1);
        }
        else if (varName == "HPP")
        {
            String HPP = Main::instance()->getHppExt();
            HPP.upper();

            outLine.insert(HPP, p1);
        }
    }
}

void MessageFile::parseMessage(InStream *is, const String &line, StringMap<Message *> &messages)
{
    StringTokenizer tk(line, " :{");
    String type;
    String msg;
    String parent;

    UInt32 id = 0;

    if (tk.hasMoreTokens())
    {
        type = tk.nextToken();

        if ((type != "msg") && (type != "abstract"))
            O3D_ERROR(E_InvalidFormat("Must have msg or abstract keyword"));
    }

    if (tk.hasMoreTokens())
        msg = tk.nextToken();

    if (tk.hasMoreTokens())
    {
        Int32 semiColumn = line.find(':');
        if (semiColumn == -1)
            O3D_ERROR(E_InvalidFormat("opening bracket { is only permited after a message declaration"));

        parent = tk.nextToken();

        if (line.sub(parent, semiColumn+1) == -1)
            O3D_ERROR(E_InvalidFormat("opening bracket { is only permited after a message declaration"));
    }

    if (tk.hasMoreTokens())
        O3D_ERROR(E_InvalidFormat("opening bracket { is only permited after a message declaration"));

    Int32 inf = msg.find('<');
    if (inf != -1)
    {
        Int32 sup = msg.find('>', inf+1);
        if (sup == -1)
            O3D_ERROR(E_InvalidFormat("< without >"));

        String v = msg.sub(inf+1, sup);
        IntegerValidator validator(0, 255);

        if (validator.validate(v))
        {
            id = v.toUInt32();
        }
        else
            O3D_ERROR(E_InvalidFormat("Must be a numeric between < >"));

        msg.truncate(inf);
    }

    if (messages.find(msg) != messages.end())
        O3D_ERROR(E_InvalidFormat("Message name " + msg + " exits"));

    // auto id
    if (id == 0)
        id = 0;// later Main::instance()->getNextMessageId();
    else
        Main::instance()->registerMessageId(id);

    Message *pmsg = new Message;
    pmsg->name = msg;
    pmsg->id = id;

    messages[msg] = pmsg;

    if (type == "abstract")
        pmsg->abstract = True;

    // inheritance
    if (parent.isValid())
    {
        auto it = m_imports.find(parent);
        if (it == m_imports.end())
            O3D_ERROR(E_InvalidFormat("Undefined message " + parent));

        // inject members of inherited message
        pmsg->members = it->second->members;
    }

    // now parse until the message closing bracket for find each entry and function
    parseMessageInt(is, line, pmsg);

    System::print(msg, "Add message");
}

void MessageFile::parseMessageInt(InStream *is, const String &_line, MessageFile::Message *msg)
{
    Bool begin = False;

    if (_line.endsWith("{"))
        begin = True;

    if (_line.count('{') > 1)
        O3D_ERROR(E_InvalidFormat("opening bracket { must not succed another {"));

    String line;
    Bool ispublic;

    while (is->readLine(line) != EOF)
    {
        // trim white spaces
        line.trimLeftChars(" \t");
        line.trimRightChars(" \t");
        line.replace('\t', ' ');

        // ignore comment lines
        if (line.startsWith("#"))
            continue;

        if (line.isEmpty())
            continue;

        if (line.startsWith("{"))
        {
            // begin
            if (begin)
                O3D_ERROR(E_InvalidFormat("opening bracket { must not succed another {"));

            begin = True;
        }
        else
        {
            ispublic = False;

            if (!begin)
                O3D_ERROR(E_InvalidFormat("missing prior opening bracket {"));

            if (line.startsWith("}"))
            {
                // end
                if (line.length() > 1)
                    O3D_ERROR(E_InvalidFormat("ending bracket } line must only contain ending bracket"));

                break;
            }

            // public const declaration
            if (line.startsWith("public "))
            {
                ispublic = True;
                line.remove(0, 6);
                line.trimLeftChars(" ");
            }

            // a loop
            if (line.startsWith("loop "))
            {
                parseMessageLoop(is, line, msg, nullptr);
            }
            // a condition
            else if (line.startsWith("if "))
            {
                parseMessageIf(is, line, msg, nullptr);
            }
            else if (line.startsWith("}"))
            {
                // end
                break;
            }
            else if (line.startsWith("@"))
            {
                // annotation
                parseAnnotation(is, line, msg);
            }            // a const
            else if (line.startsWith("const "))
            {
                // should be a const member
                parseMessageConst(is, line, msg, nullptr, ispublic);
            }
            // a bit const
            else if (line.startsWith("bit "))
            {
                // should be a const member
                parseMessageBit(is, line, msg, nullptr);
            }
            // a static sized array
            else if (line.find('[') != -1)
            {
                // should be a const member
                parseMessageArray(is, line, msg, nullptr);
            }
            else
            {
                // should be a membre
                parseMessageMember(is, line, msg, nullptr);
            }
        }
    }
}

void MessageFile::parseMessageLoop(
        InStream *is,
        const String &_line,
        MessageFile::Message *msg,
        Member *parent)
{
    Bool begin = False;

    if (_line.endsWith("{"))
        begin = True;

    if (_line.count('{') > 1)
        O3D_ERROR(E_InvalidFormat("opening bracket { must not succed another {"));

    // parse the counter
    StringTokenizer tk(_line, " :{}");
    if (tk.hasMoreTokens())
        if (tk.nextToken() != "loop")
            O3D_ERROR(E_InvalidFormat("invalid loop keyword"));

    String loopName;

    if (tk.hasMoreTokens())
        loopName = tk.nextToken();

    String counterVar;

    if (tk.hasMoreTokens())
        counterVar = tk.nextToken();

    String counterVarName;
    String counterVarParam;

    StringTokenizer tk2(counterVar, "[]");
    if (tk2.hasMoreTokens())
        counterVarName = tk2.nextToken();

    if (tk2.hasMoreTokens())
        counterVarParam = tk2.nextToken();

    Member *varMember = nullptr;
    for (Member *m : msg->members)
    {
        if (m->getName() == counterVarName)
        {
            varMember = m;
            break;
        }
    }

    if (varMember == nullptr && parent)
        varMember = parent->findMember(counterVarName);

    if (varMember == nullptr)
        O3D_ERROR(E_InvalidParameter("unable to find the counter variable " + counterVarName));

    Member *constMember = nullptr;

    // create the const member if counterVarParam is defined and it is not an integer
    if (counterVarParam.isValid())
    {
        if (UInteger32::isInteger(counterVarParam))
        {
            constMember = MemberFactory::instance()->buildFromTypeName("immediate", nullptr);
            constMember->setName(UInteger32::toString(varMember->getNewUIntId()));
            msg->members.push_back(constMember);
        }
        else
        {
            constMember = MemberFactory::instance()->buildFromTypeName("const uint32", nullptr);
            constMember->setName(counterVarParam);
            constMember->setValue(UInteger32::toString(varMember->getNewUIntId()));
            msg->members.push_back(constMember);
        }
    }

    // create the loop member
    Member *member = MemberFactory::instance()->buildFromTypeName("loop", parent);
    member->setName(loopName);
    member->setCond(varMember, constMember);
    if (parent)
        parent->addMember(member);
    else
        msg->members.push_back(member);

    String line;
    Bool ispublic;

    while (is->readLine(line) != EOF)
    {
        // trim white spaces
        line.trimLeftChars(" \t");
        line.trimRightChars(" \t");
        line.replace('\t', ' ');

        // ignore comment lines
        if (line.startsWith("#"))
            continue;

        if (line.isEmpty())
            continue;

        if (line.startsWith("{"))
        {
            // begin
            if (begin)
                O3D_ERROR(E_InvalidFormat("opening bracket { must not succed another {"));

            begin = True;
        }
        else
        {
            ispublic = False;

            if (!begin)
                O3D_ERROR(E_InvalidFormat("missing prior opening bracket {"));

            if (line.startsWith("}"))
            {
                // end
                if (line.length() > 1)
                    O3D_ERROR(E_InvalidFormat("ending bracket } line must only contain ending bracket"));

                break;
            }

            // public declaration
            if (line.startsWith("public "))
            {
                ispublic = True;
                line.remove(0, 6);
                line.trimLeftChars(" ");
            }

            // a loop
            if (line.startsWith("loop "))
            {
                O3D_ERROR(E_InvalidFormat("loop in loop is forbidden"));
                //parseMessageLoop(is, line, msg, member);
            }
            // a condition
            else if (line.startsWith("if "))
            {
                O3D_ERROR(E_InvalidFormat("if in loop is forbidden"));
                //parseMessageIf(is, line, msg, member);
            }
            // a const
            else if (line.startsWith("const "))
            {
                // should be a const member
                parseMessageConst(is, line, msg, member, ispublic);
            }
            // a bit const
            else if (line.startsWith("bit "))
            {
                // should be a const member
                parseMessageBit(is, line, msg, member);
            }
            // a static sized array
            else if (line.find('[') != -1)
            {
                // should be a const member
                parseMessageArray(is, line, msg, member);
            }
            else
            {
                // should be a membre
                parseMessageMember(is, line, msg, member);
            }
        }
    }
}

void MessageFile::parseMessageIf(
        InStream *is,
        const String &_line,
        MessageFile::Message *msg,
        Member *parent)
{
    Bool begin = False;

    if (_line.endsWith("{"))
        begin = True;

    if (_line.count('{') > 1)
        O3D_ERROR(E_InvalidFormat("opening bracket { must not succed another {"));

    // parse the condition
    StringTokenizer tk(_line, " {}");
    if (tk.hasMoreTokens())
        if (tk.nextToken() != "if")
            O3D_ERROR(E_InvalidFormat("invalid if keyword"));

    String condVar;

    if (tk.hasMoreTokens())
        condVar = tk.nextToken();

    String condVarName;
    String condVarParam;

    StringTokenizer tk2(condVar, "[]");
    if (tk2.hasMoreTokens())
        condVarName = tk2.nextToken();

    if (tk2.hasMoreTokens())
        condVarParam = tk2.nextToken();

    Member *varMember = nullptr;
    for (Member *m : msg->members)
    {
        if (m->getName() == condVarName)
        {
            varMember = m;
            break;
        }
    }

    if (varMember == nullptr && parent)
        varMember = parent->findMember(condVarName);

    if (varMember == nullptr)
        O3D_ERROR(E_InvalidParameter("unable to find the condition variable " + condVarName));

    Member *constMember = nullptr;

    // create the const member if condVarParam is defined and it is not an integer
    if (condVarParam.isValid())
    {
        if (UInteger32::isInteger(condVarParam))
        {
            constMember = MemberFactory::instance()->buildFromTypeName("immediate", nullptr);
            constMember->setName(UInteger32::toString(varMember->getNewUIntId()));
            msg->members.push_back(constMember);
        }
        else
        {
            constMember = MemberFactory::instance()->buildFromTypeName("const uint32", nullptr);
            constMember->setName(condVarParam);
            constMember->setValue(UInteger32::toString(varMember->getNewUIntId()));
            msg->members.push_back(constMember);
        }
    }

    // create the if member
    Member *member = MemberFactory::instance()->buildFromTypeName("if", parent);
    member->setName("if");
    member->setCond(varMember, constMember);
    if (parent)
        parent->addMember(member);
    else
        msg->members.push_back(member);

    String line;
    Bool ispublic;

    while (is->readLine(line) != EOF)
    {
        // trim white spaces
        line.trimLeftChars(" \t");
        line.trimRightChars(" \t");
        line.replace('\t', ' ');

        // ignore comment lines
        if (line.startsWith("#"))
            continue;

        if (line.isEmpty())
            continue;

        if (line.startsWith("{"))
        {
            // begin
            if (begin)
                O3D_ERROR(E_InvalidFormat("opening bracket { must not succed another {"));

            begin = True;
        }
        else
        {
            ispublic = False;

            if (!begin)
                O3D_ERROR(E_InvalidFormat("missing prior opening bracket {"));

            if (line.startsWith("}"))
            {
                // end
                if (line.length() > 1)
                    O3D_ERROR(E_InvalidFormat("ending bracket } line must only contain ending bracket"));

                break;
            }

            // public const declaration
            if (line.startsWith("public "))
            {
                ispublic = True;
                line.remove(0, 6);
                line.trimLeftChars(" ");
            }

            // a loop
            if (line.startsWith("loop "))
            {
                parseMessageLoop(is, line, msg, member);
            }
            // a condition
            else if (line.startsWith("if "))
            {
                O3D_ERROR(E_InvalidFormat("if in if is forbidden"));
                parseMessageIf(is, line, msg, member);
            }
            // a const
            else if (line.startsWith("const "))
            {
                // should be a const member
                parseMessageConst(is, line, msg, member, ispublic);
            }
            // a bit const
            else if (line.startsWith("bit "))
            {
                // should be a const member
                parseMessageBit(is, line, msg, member);
            }
            // a static sized array
            else if (line.find('[') != -1)
            {
                // should be a const member
                parseMessageArray(is, line, msg, member);
            }
            else
            {
                // should be a membre
                parseMessageMember(is, line, msg, member);
            }
        }
    }
}

void MessageFile::parseMessageMember(
        InStream *is,
        const String &_line,
        MessageFile::Message *msg,
        Member *parent)
{
    StringTokenizer tk(_line, " ");
    String type;
    String name;

    if (tk.hasMoreTokens())
        type = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing member type"));

    if (tk.hasMoreTokens())
        name = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing member name for " + type));

    Member *member = MemberFactory::instance()->buildFromTypeName(type, parent);
    member->setName(name);

    // add the header if necessary
    Bool hd = True;
    for (const String &header : m_msgInClassIncludes)
    {
        if (header == member->getHeader())
        {
            hd = False;
            break;
        }
    }
    if (hd && member->getHeader().isValid())
    {
        m_msgInClassIncludes.push_back(member->getHeader());
        m_msgOutClassIncludes.push_back(member->getHeader());
    }

    if (parent == nullptr)
        msg->members.push_back(member);
    else
        parent->addMember(member);

    //System::print(member->getTypeName(), name);
}

void MessageFile::parseMessageArray(
        InStream *is,
        const String &_line,
        MessageFile::Message *msg,
        Member *parent)
{
    StringTokenizer tk(_line, " ");
    String type;
    String name;
    String size;

    if (tk.hasMoreTokens())
        type = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing member type"));

    if (tk.hasMoreTokens())
        name = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing member name"));

    // type is a static sized array
    if (type.endsWith("]"))
    {
        Int32 p1 = type.find('[');
        Int32 p2 = type.length() - 1;

        if (p1 == -1)
            O3D_ERROR(E_InvalidFormat("missing opening [ for a static sized array"));

        size = type.sub(p1+1, p2);
        if (!UInteger32::isInteger(size))
            O3D_ERROR(E_InvalidFormat("only unsigned integer are allowed between []"));

        type = type.remove(size);
    }
    else
        O3D_ERROR(E_InvalidFormat("missing closing [ for a static sized array"));

    Member *member = MemberFactory::instance()->buildFromTypeName(type, parent);
    member->setName(name);
    member->setValue(size);

    // add the header if necessary
    Bool hd = True;
    for (const String &header : m_msgInClassIncludes)
    {
        if (header == member->getHeader())
        {
            hd = False;
            break;
        }
    }
    if (hd && member->getHeader().isValid())
    {
        m_msgInClassIncludes.push_back(member->getHeader());
        m_msgOutClassIncludes.push_back(member->getHeader());
    }

    if (parent == nullptr)
        msg->members.push_back(member);
    else
        parent->addMember(member);

    //System::print(member->getTypeName(), name);
}

void MessageFile::parseMessageConst(
        InStream *is,
        const String &_line,
        MessageFile::Message *msg,
        Member *parent,
        Bool ispublic)
{
    StringTokenizer tk(_line, " ");
    String type;
    String name;
    String value;

    if (tk.hasMoreTokens())
        type = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing const keyword"));

    if (tk.hasMoreTokens())
        type += " " + tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing const type"));

    // for example "const int8 CONST = 1
    if (tk.hasMoreTokens())
        name = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing const name"));

    if (!tk.hasMoreTokens() || (tk.nextToken() != "="))
        O3D_ERROR(E_InvalidFormat("missing = between const name and value"));

    if (tk.hasMoreTokens())
        value = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing const value"));

    Member *member = MemberFactory::instance()->buildFromTypeName(type, parent);
    member->setName(name);
    member->setValue(value);

    // public const
    if (ispublic)
        member->setPublic();

    // add the header if necessary
    Bool hd = True;
    for (const String &header : m_msgInClassIncludes)
    {
        if (header == member->getHeader())
        {
            hd = False;
            break;
        }
    }
    if (hd && member->getHeader().isValid())
        m_msgInClassIncludes.push_back(member->getHeader());

    // out class
    hd = True;
    for (const String &header : m_msgOutClassIncludes)
    {
        if (header == member->getHeader())
        {
            hd = False;
            break;
        }
    }
    if (hd && member->getHeader().isValid())
        m_msgOutClassIncludes.push_back(member->getHeader());

    if (parent == nullptr)
        msg->members.push_back(member);
    else
        parent->addMember(member);

    //System::print(member->getTypeName(), name);
}

void MessageFile::parseMessageBit(InStream *is, const String &_line, MessageFile::Message *msg, Member *parent)
{
    StringTokenizer tk(_line, " ");
    String type;
    String bitSetVarName;
    String constName;

    if (tk.hasMoreTokens())
        type = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing bit keyword"));

    // for example "bit myBitset[CONST]"
    if (tk.hasMoreTokens())
        bitSetVarName = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing bitset member"));

    // name contain a bit flag name
    if (bitSetVarName.endsWith("]"))
    {
        Int32 p1 = bitSetVarName.find('[');
        Int32 p2 = bitSetVarName.length() - 1;

        if (p1 == -1)
            O3D_ERROR(E_InvalidFormat("missing opening [ for a bit"));

        constName = bitSetVarName.sub(p1+1, p2);
        bitSetVarName = bitSetVarName.remove(p1, p2-p1+1);
    }
    else
        O3D_ERROR(E_InvalidFormat("missing closing ] for a bit"));

    Member *varMember = nullptr;
    for (Member *m : msg->members)
    {
        if (m->getName() == bitSetVarName)
        {
            varMember = m;
            break;
        }
    }

    if (varMember == nullptr && parent)
        varMember = parent->findMember(bitSetVarName);

    if (varMember == nullptr)
        O3D_ERROR(E_InvalidParameter("unable to find the bitset variable " + bitSetVarName));

    Member *bitMember = nullptr;

    // create the const member if constName is defined and it is not an integer
    if (constName.isValid())
    {
        if (UInteger32::isInteger(constName))
            O3D_ERROR(E_InvalidParameter("Const value must be a litteral"));
        else
        {
            bitMember = MemberFactory::instance()->buildFromTypeName("bit", nullptr);
            bitMember->setName(constName);
            bitMember->setCond(varMember, bitMember);
            bitMember->setValue(UInteger32::toString(varMember->getNewUIntId()));
            msg->members.push_back(bitMember);

            //System::print(bitMember->getTypeName(), bitSetVarName);
        }
    }
}

void MessageFile::parseAnnotation(InStream *is, const String &_line, MessageFile::Message *msg)
{
    StringTokenizer tk(_line, " ");
    String type;
    String value;

    if (tk.hasMoreTokens())
        type = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing annotation typename"));

    if (tk.hasMoreTokens())
        value = tk.nextToken();
    else
        O3D_ERROR(E_InvalidFormat("missing annotation value"));

    if (type == "@client")
    {
        if (value == "none")
            msg->type[CLIENT] = NONE;
        else if (value == "in")
            msg->type[CLIENT] = IN;
        else if (value == "out")
            msg->type[CLIENT] = OUT;
        else if (value == "both")
            msg->type[CLIENT] = BOTH;
    }
    else if (type == "@server")
    {
        if (value == "none")
            msg->type[SERVER] = NONE;
        else if (value == "in")
            msg->type[SERVER] = IN;
        else if (value == "out")
            msg->type[SERVER] = OUT;
        else if (value == "both")
            msg->type[SERVER] = BOTH;
    }
    else
        O3D_ERROR(E_InvalidFormat("unsupported annotation typename"));
}
