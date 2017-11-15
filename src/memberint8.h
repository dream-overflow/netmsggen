/**
 * @file memberint8.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERINT8_H
#define _O3D_NMG_MEMBERINT8_H

#include "member.h"

namespace o3d {
namespace nmg {

class MemberInt8 : public MemberHelper<MemberInt8, Member::TYPE_INT8>
{
public:

    MemberInt8(Member *parent);

    virtual String getTypeName() const;
    virtual String getOutTypeName() const;

    virtual String getReadMethod() const;
    virtual String getWriteMethod() const;

    virtual UInt32 getMinSize() const;
    virtual String getSizeOf() const;

private:
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBERINT8_H
