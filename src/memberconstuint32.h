/**
 * @file memberconstuint32.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERCONSTUINT32_H
#define _O3D_NMG_MEMBERCONSTUINT32_H

#include "member.h"

namespace o3d {
namespace nmg {

class MemberConstUInt32 : public MemberHelper<MemberConstUInt32, Member::TYPE_CONST_UINT32>
{
public:

    MemberConstUInt32(Member *parent);

    virtual String getTypeName() const;
    virtual String getOutTypeName() const;

    virtual String getReadMethod() const;
    virtual String getWriteMethod() const;

    virtual void writeDecl(OutStream *os);

    virtual void writeRead(OutStream *os);
    virtual void writeWrite(OutStream *os);

    virtual void setValue(const String &value);

    virtual void writeSetterDecl(OutStream *os);
    virtual void writeSetterImpl(OutStream *os);

private:

    String m_value;
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBERCONSTUINT32_H
