/**
 * @file member.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "member.h"

using namespace o3d;
using namespace o3d::nmg;

Member::Member(Member *parent) :
    m_parent(parent),
    m_public(False)
{
}

Member::~Member()
{

}

String Member::getHeader() const
{
    return "";
}

String Member::getIfTest(const Member *param) const
{
    if (param)
        return String("[") + param->getName() + "] != 0";
    else
        return " != 0";
}

UInt32 Member::getNewUIntId()
{
    return 0;
}

void Member::addMember(Member *member)
{
    // nothing
}

Member *Member::findMember(const String &name) const
{
    if (m_parent)
        return m_parent->findMember(name);
    else
        return nullptr;
}

void Member::setCond(Member *var, Member *varParam)
{
    // nothing
}

void Member::setPublic()
{
    m_public = True;
}

void Member::setPrivate()
{
    m_public = False;
}

Bool Member::isPrivate() const
{
    return !m_public;
}

Bool Member::isPublic() const
{
    return m_public;
}

void Member::setValue(const String &value)
{
    // nothing
}

void Member::writeDecl(OutStream *os)
{
    Int32 ident = 1;
    Member *parent = m_parent;
    while (parent != nullptr)
    {
        ident += parent->getIdent();
        parent = parent->m_parent;
    }

    String line;
    for (Int32 i = 0; i < ident; ++i)
    {
        line += "    ";
    }
    line += getOutTypeName() + " " + m_name + ";";

    os->writeLine(line);
}

void Member::writeRead(OutStream *os)
{
    Int32 ident = 1;
    Member *parent = m_parent;
    while (parent != nullptr)
    {
        ident += parent->getIdent();
        parent = parent->m_parent;
    }

    String line;
    for (Int32 i = 0; i < ident; ++i)
    {
        line += "    ";
    }

    line += String((m_parent ? m_parent->getPrefix() : String("")) + m_name + " = buffer->") + getReadMethod() + "();";

    os->writeLine(line);
}

void Member::writeWrite(OutStream *os)
{
    Int32 ident = 1;
    Member *parent = m_parent;
    while (parent != nullptr)
    {
        ident += parent->getIdent();
        parent = parent->m_parent;
    }

    String line;
    for (Int32 i = 0; i < ident; ++i)
    {
        line += "    ";
    }

    line += String("buffer->") + getWriteMethod() + "(" + (m_parent ? m_parent->getPrefix() : "") + m_name + ");";

    os->writeLine(line);
}

void Member::writeSetterDecl(OutStream *os)
{
    String line("    ");

    String name = getName();
    if (name.startsWith("m_"))
        name.remove(0, 2);
    String mname = name;
    if (name.length() >= 1 && name[0] >= 'a' && name[0] <= 'z')
        name[0] = name[0] - ('a' - 'A');
    mname.insert('_', 0);

    if (isRef())
        line += String("void set") + name + "(const " + getOutTypeName() + " &" + mname + ")";
    else
        line += String("void set") + name + "(" + getOutTypeName() + " " + mname + ")";
    os->writeLine(line);

    os->writeLine("    {");
    line = "        " + getName() + " = " + mname + ";";
    os->writeLine(line);
    os->writeLine("    }");
    os->writeLine("");
}

void Member::writeSetterImpl(OutStream *os)
{

}

UInt32 Member::getIdent() const
{
    return 0;
}

String Member::getPrefix() const
{
    return "";
}

UInt32 Member::getMinSize() const
{
    return 0;
}

Bool Member::isRef() const
{
    return False;
}

String Member::getSetTo(const Member *param, SetValue value) const
{
    if (param)
    {
        if (value == SET_TRUE)
            return String("[") + param->getName() + "] = 1";
        else
            return String("[") + param->getName() + "] = 0";
    }
    else
    {
        if (value == SET_TRUE)
            return " = 1";
        else
            return " = 0";
    }
}

String Member::getSizeOf() const
{
    return "0";
}
