/**
 * @file memberbitset32.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERBITSET32_H
#define _O3D_NMG_MEMBERBITSET32_H

#include "member.h"
#include <o3d/core/idmanager.h>

namespace o3d {
namespace nmg {

class MemberBitSet32 : public MemberHelper<MemberBitSet32, Member::TYPE_BITSET32>
{
public:

    MemberBitSet32(Member *parent);

    virtual String getTypeName() const;
    virtual String getOutTypeName() const;

    virtual String getReadMethod() const;
    virtual String getWriteMethod() const;

    virtual void writeRead(OutStream *os);

    virtual String getHeader() const;

    virtual String getIfTest(const Member *param) const;
    virtual UInt32 getNewUIntId();

    virtual void writeSetterDecl(OutStream *os);
    virtual void writeSetterImpl(OutStream *os);

    virtual String getSetTo(const Member *param, SetValue value) const;

    virtual UInt32 getMinSize() const;
    virtual String getSizeOf() const;

private:

    IDManager m_uintId;
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBERBITSET32_H
