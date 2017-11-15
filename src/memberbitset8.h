/**
 * @file memberbitset8.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERBITSET8_H
#define _O3D_NMG_MEMBERBITSET8_H

#include "member.h"
#include <o3d/core/idmanager.h>

namespace o3d {
namespace nmg {

class MemberBitSet8 : public MemberHelper<MemberBitSet8, Member::TYPE_BITSET8>
{
public:

    MemberBitSet8(Member *parent);

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

#endif // _O3D_NMG_MEMBERBITSET8_H
