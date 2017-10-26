/**
 * @file memberstaticarrayuint8.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "memberstaticarrayuint8.h"
#include "registermember.h"
#include <o3d/core/integer.h>

using namespace o3d;
using namespace o3d::nmg;

RegisterMember<MemberStaticArrayUInt8>::R memberStaticArrayUInt8;

MemberStaticArrayUInt8::MemberStaticArrayUInt8(Member *parent) :
    MemberHelper(parent)
{
}

String MemberStaticArrayUInt8::getTypeName() const
{
    return "uint8[]";
}

String MemberStaticArrayUInt8::getOutTypeName() const
{
    return "o3d::UInt8";
}

String MemberStaticArrayUInt8::getReadMethod() const
{
    return "read";
}

String MemberStaticArrayUInt8::getWriteMethod() const
{
    return "write";
}

String MemberStaticArrayUInt8::getHeader() const
{
    return "<stdio.h>";
}

String MemberStaticArrayUInt8::getIfTest(const Member *param) const
{
    O3D_ASSERT(param != nullptr);
    return "[" + param->getName() + "]";
}

UInt32 MemberStaticArrayUInt8::getNewUIntId()
{
    return m_uintId.getID();
}

void MemberStaticArrayUInt8::writeDecl(OutStream *os)
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
    line += getOutTypeName() + " " + getName() + "[" + m_value + "];";

    os->writeLine(line);
}

void MemberStaticArrayUInt8::writeSetterDecl(OutStream *os)
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

    line += String("void set") + name + "(const " + getOutTypeName() + " *_" + mname + ")";
    os->writeLine(line);

    os->writeLine("    {");
    line = "        memcpy(" + getName() + ", _" + mname + ", " + m_value + ");";
    os->writeLine(line);

    os->writeLine("    }");
    os->writeLine("");
}

void MemberStaticArrayUInt8::writeSetterImpl(OutStream *os)
{

}

void MemberStaticArrayUInt8::writeRead(OutStream *os)
{
    Int32 ident = 1;
    Member *parent = getParent();
    while (parent != nullptr)
    {
        ident += parent->getIdent();
        parent = parent->getParent();
    }

    String identStr;
    for (Int32 i = 0; i < ident; ++i)
    {
        identStr += "    ";
    }

    // content
    String line = identStr + "buffer->" + getReadMethod() +
            "(" +
            getName() + ", " +
            m_value +
            ");";

    os->writeLine(line);
}

void MemberStaticArrayUInt8::writeWrite(OutStream *os)
{
    Int32 ident = 1;
    Member *parent = getParent();
    while (parent != nullptr)
    {
        ident += parent->getIdent();
        parent = parent->getParent();
    }

    String identStr;
    for (Int32 i = 0; i < ident; ++i)
    {
        identStr += "    ";
    }

    // content
    String line = identStr + String("buffer->") + getWriteMethod() +
            "(" +
            getName() + ", " +
            m_value +
            ");";

    os->writeLine(line);
}

void MemberStaticArrayUInt8::setValue(const String &value)
{
    m_value = value;
}

Bool MemberStaticArrayUInt8::isRef() const
{
    return True;
}

UInt32 MemberStaticArrayUInt8::getMinSize() const
{
    return UInteger32::parseInteger(m_value);
}

String MemberStaticArrayUInt8::getSizeOf() const
{
    return m_value;
}
