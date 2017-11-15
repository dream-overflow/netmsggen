/**
 * @file memberbitset64.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "memberbitset64.h"
#include "registermember.h"

using namespace o3d;
using namespace o3d::nmg;

RegisterMember<MemberBitSet64>::R memberBitSet64;

MemberBitSet64::MemberBitSet64(Member *parent) :
    MemberHelper(parent)
{
}

String MemberBitSet64::getTypeName() const
{
    return "bitset64";
}

String MemberBitSet64::getOutTypeName() const
{
    return "o3d::BitSet64";
}

String MemberBitSet64::getReadMethod() const
{
    return "readUInt64";
}

String MemberBitSet64::getWriteMethod() const
{
    return "writeUInt64";
}

void MemberBitSet64::writeRead(OutStream *os)
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

    line += prefixedName + ".set(buffer->" + getReadMethod() + "());";

    os->writeLine(line);
}

String MemberBitSet64::getHeader() const
{
    return "<o3d/core/TemplateBitSet.h>";
}

String MemberBitSet64::getIfTest(const Member *param) const
{
    O3D_ASSERT(param != nullptr);
    return ".getBit(" + param->getName() + ")";
}

UInt32 MemberBitSet64::getNewUIntId()
{
    return m_uintId.getID();
}

void MemberBitSet64::writeSetterDecl(OutStream *os)
{

}

void MemberBitSet64::writeSetterImpl(OutStream *os)
{

}

String MemberBitSet64::getSetTo(const Member *param, SetValue value) const
{
    O3D_ASSERT(param != nullptr);
    if (value == SET_TRUE)
        return ".enable(" + param->getName() + ")";
    else
        return ".disable(" + param->getName() + ")";
}

UInt32 MemberBitSet64::getMinSize() const
{
    return 8;
}

String MemberBitSet64::getSizeOf() const
{
    return "8";
}
