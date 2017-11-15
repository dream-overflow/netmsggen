/**
 * @file memberuint64.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "memberuint64.h"
#include "registermember.h"

using namespace o3d;
using namespace o3d::nmg;

RegisterMember<MemberUInt64>::R memberUInt64;

MemberUInt64::MemberUInt64(Member *parent) :
    MemberHelper(parent)
{
}

String MemberUInt64::getTypeName() const
{
    return "uint64";
}

String MemberUInt64::getOutTypeName() const
{
    return "o3d::UInt64";
}

String MemberUInt64::getReadMethod() const
{
    return "readUInt64";
}

String MemberUInt64::getWriteMethod() const
{
    return "writeUInt64";
}

UInt32 MemberUInt64::getMinSize() const
{
    return 8;
}

String MemberUInt64::getSizeOf() const
{
    return "8";
}
