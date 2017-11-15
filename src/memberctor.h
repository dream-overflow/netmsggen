/**
 * @file memberctor.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERCTOR_H
#define _O3D_NMG_MEMBERCTOR_H

#include "member.h"

namespace o3d {
namespace nmg {

class MemberCtor : public MemberHelper<MemberCtor, Member::TYPE_IF>
{
public:

    MemberCtor(Member *parent);
    virtual ~MemberCtor();

    virtual String getTypeName() const;
    virtual String getOutTypeName() const;

    virtual void writeDecl(OutStream *os);

    virtual String getReadMethod() const;
    virtual String getWriteMethod() const;

    virtual void writeRead(OutStream *os);
    virtual void writeWrite(OutStream *os);

    virtual void writeSetterDecl(OutStream *os);
    virtual void writeSetterImpl(OutStream *os);

    virtual void setValue(const String &value);

private:

    String m_value;
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBERCTOR_H
