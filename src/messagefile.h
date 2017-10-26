/**
 * @file messagefile.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MESSAGEFILE_H
#define _O3D_NMG_MESSAGEFILE_H

#include <o3d/core/stringlist.h>
#include <o3d/core/stringmap.h>
#include "member.h"

namespace o3d {
namespace nmg {

class MessageFile
{
public:

    MessageFile(const String &filename);

    ~MessageFile();

    const String& getName() const;

    void parse();

    void process();

    void setPrefix(const String &prefix) { m_prefix = prefix; }

    void rename(const String &old, const String &name);

    void addMember(const String &message, Member *member);

private:

    typedef std::list<Member*> T_MemberList;
    typedef T_MemberList::iterator IT_MemberList;
    typedef T_MemberList::const_iterator CIT_MemberList;

    enum ProfileType
    {
        NONE,
        IN,
        OUT,
        BOTH
    };

    enum Profile
    {
        CLIENT = 0,
        SERVER = 1
    };

    struct Message
    {
        Message() :
            abstract(False),
            minSize(0)
        {
            type[0] = type[1] = NONE;
        }

        Bool abstract;

        String name;
        UInt32 id;

        T_MemberList members;

        ProfileType type[2];

        UInt32 minSize;
    };

    String m_filename;
    String m_prefix;

    T_StringList m_msgInClassIncludes;
    T_StringList m_msgOutClassIncludes;
    T_StringList m_msgInImplIncludes;
    T_StringList m_msgInRunIncludes;
    T_StringList m_msgOutImplIncludes;

    void parse(InStream *is, StringMap<Message*>& messages);

    void importMessage(const String &line);

    void parseMessage(InStream *is, const String &line, StringMap<Message*>& messages);
    void parseMessageInt(InStream *is, const String &line, Message *msg);
    void parseMessageLoop(InStream *is, const String &line, Message *msg, Member *parent);
    void parseMessageIf(InStream *is, const String &line, Message *msg, Member *parent);
    void parseMessageMember(InStream *is, const String &line, Message *msg, Member *parent);
    void parseMessageArray(InStream *is, const String &line, Message *msg, Member *parent);
    void parseMessageConst(InStream *is, const String &line, Message *msg, Member *parent, Bool ispublic);
    void parseMessageBit(InStream *is, const String &line, Message *msg, Member *parent);

    void parseAnnotation(InStream *is, const String &line, Message *msg);

    void writeMessageInClass(const String &outPath, const String &hppExt, Profile profile);
    void writeMessageInImpl(const String &outPath, const String &cppExt, Profile profile);
    void writeMessageInRun(const String &outPath, const String &cppExt, Profile profile);

    void writeMessageInClassContent(OutStream *os, Message *msg, Profile profile);
    void writeMessageInImplContent(OutStream *os, Message *msg, Profile profile);
    void writeMessageInRunContent(OutStream *os, Message *msg, Profile profile);

    void writeMessageOutClass(const String &outPath, const String &hppExt, Profile profile);
    void writeMessageOutImpl(const String &outPath, const String &cppExt, Profile profile);

    void writeMessageOutClassContent(OutStream *os, Message *msg, Profile profile);
    void writeMessageOutImplContent(OutStream *os, Message *msg, Profile profile);

    void parseVariable(String &outLine, Message *msg, const String &header, Profile profile);

    StringMap<Message*> m_messages;
    StringMap<Message*> m_imports;
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MESSAGEFILE_H
