/**
 * @file memberuint16.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "memberuint16.h"
#include "registermember.h"

using namespace o3d;
using namespace o3d::nmg;

RegisterMember<MemberUInt16>::R memberUInt16;

MemberUInt16::MemberUInt16(Member *parent) :
    MemberHelper(parent)
{
}

String MemberUInt16::getTypeName() const
{
    return "uint16";
}

String MemberUInt16::getOutTypeName() const
{
    return "o3d::UInt16";
}

String MemberUInt16::getReadMethod() const
{
    return "readUInt16";
}

String MemberUInt16::getWriteMethod() const
{
    return "writeUInt16";
}

UInt32 MemberUInt16::getMinSize() const
{
    return 2;
}

String MemberUInt16::getSizeOf() const
{
    return "2";
}
