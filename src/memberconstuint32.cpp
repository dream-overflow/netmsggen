/**
 * @file memberconstuint32.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "memberconstuint32.h"
#include "registermember.h"

using namespace o3d;
using namespace o3d::nmg;

RegisterMember<MemberConstUInt32>::R memberConstUInt32;

MemberConstUInt32::MemberConstUInt32(Member *parent) :
    MemberHelper(parent)
{
}

String MemberConstUInt32::getTypeName() const
{
    return "const uint32";
}

String MemberConstUInt32::getOutTypeName() const
{
    return "o3d::UInt32";
}

String MemberConstUInt32::getReadMethod() const
{
    return "";
}

String MemberConstUInt32::getWriteMethod() const
{
    return "";
}

void MemberConstUInt32::writeDecl(OutStream *os)
{
    String line = "    ";
    line += String("static const ") + getOutTypeName() + " " + getName() + " = " + m_value + ";";

    os->writeLine(line);
}

void MemberConstUInt32::writeRead(OutStream *os)
{
    // nothing
}

void MemberConstUInt32::writeWrite(OutStream *os)
{
    // nothing
}

void MemberConstUInt32::setValue(const String &value)
{
    m_value = value;
}

void MemberConstUInt32::writeSetterDecl(OutStream *os)
{
    // nothing
}

void MemberConstUInt32::writeSetterImpl(OutStream *os)
{
    // nothing
}
