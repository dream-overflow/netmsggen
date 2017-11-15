/**
 * @file memberstring.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "memberstring.h"
#include "registermember.h"
#include <o3d/core/file.h>

using namespace o3d;
using namespace o3d::nmg;

RegisterMember<MemberString>::R memberString;

MemberString::MemberString(Member *parent) :
    MemberHelper(parent)
{
}

String MemberString::getTypeName() const
{
    return "string";
}

String MemberString::getOutTypeName() const
{
    return "o3d::String";
}

String MemberString::getReadMethod() const
{
    return "readUTF8";
}

String MemberString::getWriteMethod() const
{
    return "writeUTF8";
}

String MemberString::getHeader() const
{
    return "<o3d/core/String.h>";
}

void MemberString::writeSetterDecl(OutStream *os)
{
    String line("    ");

    String name = getName();
    if (name.startsWith("_"))
        name.remove(0, 1);
    if (name.startsWith("m_"))
        name.remove(0, 2);
    String mname = name;
    if (name.length() >= 1 && name[0] >= 'a' && name[0] <= 'z')
        name[0] = name[0] - ('a' - 'A');

    line += String("void set") + name + "(const " + getOutTypeName() + " &" + mname + ")";
    os->writeLine(line);

    os->writeLine("    {");
    line = "        " + getName() + " = " + mname + ";";
    os->writeLine(line);
    os->writeLine("    }");
    os->writeLine("");
}

void MemberString::writeRead(OutStream *os)
{
    Int32 ident = 1;
    Member *parent = getParent();
    while (parent != nullptr)
    {
        ident += parent->getIdent();
        parent = parent->getParent();
    }

    String line;
    for (Int32 i = 0; i < ident; ++i)
    {
        line += "    ";
    }

    line += String("buffer->") + getReadMethod() + "(" + (isParent() ? getParent()->getPrefix() : "") + getName() + ");";

    os->writeLine(line);
}

void MemberString::writeWrite(OutStream *os)
{
    Int32 ident = 1;
    Member *parent = getParent();
    while (parent != nullptr)
    {
        ident += parent->getIdent();
        parent = parent->getParent();
    }

    String line;
    for (Int32 i = 0; i < ident; ++i)
    {
        line += "    ";
    }

    String prefixedName = (isParent() ? getParent()->getPrefix() : "") + getName();

    line += String("buffer->") + getWriteMethod() + "(" + prefixedName + ");";

    os->writeLine(line);
}

void MemberString::writeSetterImpl(OutStream *os)
{

}

Bool MemberString::isRef() const
{
    return True;
}

UInt32 MemberString::getMinSize() const
{
    return 2;
}

String MemberString::getSizeOf() const
{
    return getName() + ".length() + 2";
}
