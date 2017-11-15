/**
 * @file memberctor.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "memberctor.h"
#include "registermember.h"
#include <o3d/core/char.h>

using namespace o3d;
using namespace o3d::nmg;

RegisterMember<MemberCtor>::R memberCtor;

MemberCtor::MemberCtor(Member *parent) :
    MemberHelper(parent),
    m_value("0")
{
}

MemberCtor::~MemberCtor()
{
}

String MemberCtor::getTypeName() const
{
    return "ctor";
}

String MemberCtor::getOutTypeName() const
{
    return "ctor";
}

String MemberCtor::getReadMethod() const
{
    return "";
}

String MemberCtor::getWriteMethod() const
{
    return "";
}

void MemberCtor::writeDecl(OutStream *os)
{
}

void MemberCtor::writeRead(OutStream *os)
{
}

void MemberCtor::writeWrite(OutStream *os)
{
}

void MemberCtor::writeSetterDecl(OutStream *os)
{
    // CTOR
    String line("    ");

    line += getName() + "()";
    os->writeLine(line);

    os->writeLine("    {");

    line = "        m_messageDataSize = " + m_value + ";";
    os->writeLine(line);

    os->writeLine("    }");
    os->writeLine("");
}

void MemberCtor::writeSetterImpl(OutStream *os)
{

}

void MemberCtor::setValue(const String &value)
{
    m_value = value;
}
