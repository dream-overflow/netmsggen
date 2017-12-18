/**
 * @file main.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include <o3d/core/architecture.h>
#include <o3d/core/main.h>
#include <o3d/core/dir.h>
#include <o3d/core/filemanager.h>
#include <o3d/core/stringtokenizer.h>
#include <o3d/core/smartpointer.h>

#include "main.h"
#include "messagefile.h"
#include "memberfactory.h"

using namespace o3d;
using namespace o3d::nmg;

Main *Main::ms_instance;

Main *Main::instance()
{
    return ms_instance;
}

Main::Main() :
    m_msgExt("msg"),
    m_hppExt("h"),
    m_cppExt("cpp"),
    m_version(1),
    m_messageId(0),
    m_versionName("Version")
{
    ms_instance = this;
}

Main::~Main()
{
    ms_instance = nullptr;
}

void Main::init()
{
    CommandLine *cmd = Application::getCommandLine();

    String configFilename = cmd->getArgs().back() + "/netmessagegen";
    if (configFilename.isEmpty())
        O3D_ERROR(E_InvalidParameter("Invalid config file"));

    readConfig(configFilename);

    Date date(True);
    m_year = date.buildString("%y");
    m_month = date.buildString("%M");
    m_day = date.buildString("%D");

    Dir inPath(m_inPath);
    if (!inPath.exists())
        O3D_ERROR(E_InvalidParameter("Invalid input path"));

    Dir serverOutPath(m_outPath[SERVER]);
    if (!serverOutPath.exists())
         O3D_ERROR(E_InvalidParameter("Invalid output path"));

    Dir clientOutPath(m_outPath[CLIENT]);
    if (!clientOutPath.exists())
         O3D_ERROR(E_InvalidParameter("Invalid output path"));

    Dir tlpPath(m_tplPath);
    if (!tlpPath.exists())
         O3D_ERROR(E_InvalidParameter("Invalid template path"));

    System::print(String::print("%i", m_version), "Generate version");

    readTemplate(m_tplPath + "/license.template", m_licenseTpl);
    readTemplate(m_tplPath + "/hpp.template", m_hppTpl);
    readTemplate(m_tplPath + "/cpp.template", m_cppTpl);
    readTemplate(m_tplPath + "/msg.in.class.template", m_messageInClassTpl);
    readTemplate(m_tplPath + "/msg.out.class.template", m_messageOutClassTpl);
    readTemplate(m_tplPath + "/msg.in.impl.template", m_messageInImplTpl);
    readTemplate(m_tplPath + "/msg.in.run.template", m_messageInImplRunTpl);
    readTemplate(m_tplPath + "/msg.out.impl.template", m_messageOutImplTpl);
}

void Main::run()
{
    // Version message
    MessageFile version(m_tplPath + "/Version.msg");
    version.setPrefix(m_versionName);
    version.parse();
    Member *v = MemberFactory::instance()->buildFromTypeName("const int16", nullptr);
    v->setName("VERSION");
    v->setValue(String::print("%u", m_version));
    if (m_versionName != "Version")
        version.rename("Version", m_versionName);

    version.addMember(m_versionName, v);
    version.process();

    // Any found messages
    FileListing files;
    files.setPath(m_inPath);
    files.setExt("*." + m_msgExt);
    files.searchFirstFile();

    typedef std::list<MessageFile*> T_MessageList;
    //typedef T_MessageList::iterator IT_MessageList;
    //typedef T_MessageList::const_iterator CIT_MessageList;

    T_MessageList messagesFiles;

    FLItem *fl;
    while ((fl = files.searchNextFile()) != nullptr)
    {
        if (fl->FileType == FILE_FILE)
        {
            // process the msg file
            MessageFile *msg = new MessageFile(files.getFileFullName());
            messagesFiles.push_back(msg);
            msg->parse();
        }
    }

    // second step process
    for (MessageFile *msg : messagesFiles)
    {
        msg->process();
        deletePtr(msg);
    }
}

Int32 Main::command()
{
    CommandLine *cmd = Application::getCommandLine();

    if (cmd->getArgs().size() >= 2)
    {
        String op = cmd->getArgs()[0];
        String message = cmd->getArgs()[1];

        // mv, rename a message from source and targets
        if (op == "mv" && message.isValid() && cmd->getArgs().size() >= 3)
        {
            String messageTo = cmd->getArgs()[2];

            Dir source(m_inPath);
            if (source.check(message + ".msg") == Dir::SUCCESS)
            {
                renameMessage(
                            source.getFullPathName() + "/" + message + ".msg",
                            source.getFullPathName() + "/" + messageTo + ".msg",
                            message,
                            messageTo);
                source.removeFile(message + ".msg");
            }

            for (Int32 i = 0; i < 2; ++i)
            {
                Dir out(m_outPath[i]);
                if (out.check(message + "In." + m_hppExt) == Dir::SUCCESS)
                {
                    renameMessageHeader(
                                out.getFullPathName() + "/" + message + "In." + m_hppExt,
                                out.getFullPathName() + "/" + messageTo + "In." + m_hppExt,
                                message,
                                messageTo);
                    out.removeFile(message + "In." + m_hppExt);
                }
                if (out.check(message + "In." + m_cppExt) == Dir::SUCCESS)
                {
                    renameMessageImpl(
                                out.getFullPathName() + "/" + message + "In." + m_cppExt,
                                out.getFullPathName() + "/" + messageTo + "In." + m_cppExt,
                                message,
                                messageTo);
                    out.removeFile(message + "In." + m_cppExt);
                }
                if (out.check(message + "In.run." + m_cppExt) == Dir::SUCCESS)
                {
                    renameMessageImpl(
                                out.getFullPathName() + "/" + message + "In.run." + m_cppExt,
                                out.getFullPathName() + "/" + messageTo + "In.run." + m_cppExt,
                                message,
                                messageTo);
                    out.removeFile(message + "In.run." + m_cppExt);
                }

                if (out.check(message + "Out." + m_hppExt) == Dir::SUCCESS)
                {
                    renameMessageHeader(
                                out.getFullPathName() + "/" + message + "Out." + m_hppExt,
                                out.getFullPathName() + "/" + messageTo + "Out." + m_hppExt,
                                message,
                                messageTo);
                    out.removeFile(message + "Out." + m_hppExt);
                }
                if (out.check(message + "Out." + m_cppExt) == Dir::SUCCESS)
                {
                    renameMessageImpl(
                                out.getFullPathName() + "/" + message + "Out." + m_cppExt,
                                out.getFullPathName() + "/" + messageTo + "Out." + m_cppExt,
                                message,
                                messageTo);
                    out.removeFile(message + "Out." + m_cppExt);
                }
            }

            return 0;
        }

        // rm, remove a message from source and targets
        if (op == "rm" && message.isValid())
        {
            Dir source(m_inPath);
            if (source.check(message + ".msg") == Dir::SUCCESS)
                source.removeFile(message + ".msg");

            for (Int32 i = 0; i < 2; ++i)
            {
                Dir out(m_outPath[i]);
                if (out.check(message + "In." + m_hppExt) == Dir::SUCCESS)
                    out.removeFile(message + "In." + m_hppExt);
                if (out.check(message + "In." + m_cppExt) == Dir::SUCCESS)
                    out.removeFile(message + "In." + m_cppExt);
                if (out.check(message + "In.run." + m_cppExt) == Dir::SUCCESS)
                    out.removeFile(message + "In.run." + m_cppExt);

                if (out.check(message + "Out." + m_hppExt) == Dir::SUCCESS)
                    out.removeFile(message + "Out." + m_hppExt);
                if (out.check(message + "Out." + m_cppExt) == Dir::SUCCESS)
                    out.removeFile(message + "Out." + m_cppExt);
            }

            return 0;
        }

        System::print("Invalid command", "NetMessageGen", System::MSG_ERROR);
        return -1;
    }

    return 1;
}

void Main::renameMessage(const String &filenameFrom, const String &filenameTo, const String &from, const String &to)
{
    AutoPtr<InStream> fileFrom(FileManager::instance()->openInStream(filenameFrom));
    AutoPtr<FileOutStream> fileTo(FileManager::instance()->openOutStream(filenameTo, FileOutStream::CREATE));

    String line, l;

    while (fileFrom->readLine(line) != -1)
    {
        l = line;
        l.trimLeftChars(" \t");
        if (l.startsWith("msg"))
        {
            // TODO better message name test
            line.replace(from, to);
        }

        fileTo->writeLine(line);
    }
}

void Main::renameMessageHeader(
        const String &filenameFrom,
        const String &filenameTo,
        const String &from,
        const String &to)
{
    AutoPtr<InStream> fileFrom(FileManager::instance()->openInStream(filenameFrom));
    AutoPtr<FileOutStream> fileTo(FileManager::instance()->openOutStream(filenameTo, FileOutStream::CREATE));

    String line;

    String FROM(from);
    FROM.upper();
    String TO(to);
    TO.upper();

    while (fileFrom->readLine(line) != -1)
    {
        // TODO more sensitive test
        line.replace(from, to);
        line.replace(FROM, TO);
        fileTo->writeLine(line);
    }
}

void Main::renameMessageImpl(
        const String &filenameFrom,
        const String &filenameTo,
        const String &from,
        const String &to)
{
    AutoPtr<InStream> fileFrom(FileManager::instance()->openInStream(filenameFrom));
    AutoPtr<FileOutStream> fileTo(FileManager::instance()->openOutStream(filenameTo, FileOutStream::CREATE));

    String line;

    String FROM(from);
    FROM.upper();
    String TO(to);
    TO.upper();

    while (fileFrom->readLine(line) != -1)
    {
        // TODO more sensitive test
        line.replace(from, to);
        line.replace(FROM, TO);
        fileTo->writeLine(line);
    }
}

UInt32 Main::getNextMessageId()
{
    return m_messageId.getID();
}

void Main::registerMessageId(UInt32 messageId)
{
    m_messageId.forceID(messageId);
}

Int32 Main::main()
{
    Debug::instance()->setDefaultLog("netmsggen.log");
    Debug::instance()->getDefaultLog().clearLog();

    CommandLine *cmd = Application::getCommandLine();
    if (cmd->getArgs().size() == 0)
    {
        System::print("Missing path to netmessagegen file", "", System::MSG_ERROR);
        return -1;
    }

    Main *apps = new Main();
    apps->init();

    Int32 res = apps->command();
    if (res != 1)
        return res;

    // process messages files
    try {
        apps->run();
    } catch (E_BaseException &e) {
    }

    // Destroy any content
    deletePtr(apps);

    return 0;
}

void Main::readTemplate(const String &filename, T_StringList &lines)
{
    // read templates
    InStream *is = FileManager::instance()->openInStream(filename);

    String line;
    while (is->readLine(line) != EOF)
    {
        lines.push_back(line);
    }

    deletePtr(is);
}

void Main::readConfig(const String &filename)
{
    // read templates
    InStream *is = FileManager::instance()->openInStream(filename);

    String line;
    Int32 equalPos;
    String key, value;

    while (is->readLine(line) != -1)
    {
        line.trimLeftChars(" \t");
        line.trimRightChars(" \t");

        // comment or empty line
        if (line.startsWith("#") || line.isEmpty())
            continue;

        // find the first = and split at
        equalPos = line.find('=');
        if (equalPos > 0)
        {
            key = line.sub(0, equalPos);
            key.trimRight(' ');

            value = line.sub(equalPos+1, -1);
            value.trimLeft(' ');

            if (key == "author")
                m_author = value;
            else if (key == "client.namespace")
                m_namespace[CLIENT] = value;
            else if (key == "server.namespace")
                m_namespace[SERVER] = value;
            else if (key == "input")
                m_inPath = FileManager::instance()->getFullFileName(value);
            else if (key == "server.output")
                m_outPath[SERVER] = FileManager::instance()->getFullFileName(value);
            else if (key == "client.output")
                m_outPath[CLIENT] = FileManager::instance()->getFullFileName(value);
            else if (key == "export")
            {
                m_build[CLIENT] = m_build[SERVER] = False;

                if (value == "client")
                    m_build[CLIENT] = True;
                else if (value == "server")
                    m_build[SERVER] = True;
                if (value == "both")
                    m_build[CLIENT] = m_build[SERVER] = True;
            }
            else if (key == "client.messageFactory.instance")
                m_messageFactoryInstance[CLIENT] = value;
            else if (key == "server.messageFactory.instance")
                m_messageFactoryInstance[SERVER] = value;
            else if (key == "client.messageFactory.header")
                m_messageFactoryHeader[CLIENT] = value;
            else if (key == "server.messageFactory.header")
                m_messageFactoryHeader[SERVER] = value;
            else if (key == "templates")
                m_tplPath = FileManager::instance()->getFullFileName(value);
            else if (key == "version")
                m_version = value.toUInt32();
            else if (key == "version.name")
                m_versionName = value;
            else if (key == "hppext")
                m_hppExt = value;
            else if (key == "cppext")
                m_cppExt = value;
        }
    }

    deletePtr(is);
}

O3D_CONSOLE_MAIN(Main, O3D_DEFAULT_CLASS_SETTINGS)
