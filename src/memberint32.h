/**
 * @file memberint32.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERINT32_H
#define _O3D_NMG_MEMBERINT32_H

#include "member.h"

namespace o3d {
namespace nmg {

class MemberInt32 : public MemberHelper<MemberInt32, Member::TYPE_INT32>
{
public:

    MemberInt32(Member *parent);

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

#endif // _O3D_NMG_MEMBERINT32_H
