/**
 * @file memberbit.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERBIT_H
#define _O3D_NMG_MEMBERBIT_H

#include "member.h"

namespace o3d {
namespace nmg {

class MemberBit : public MemberHelper<MemberBit, Member::TYPE_BIT>
{
public:

    MemberBit(Member *parent);
    virtual ~MemberBit();

    virtual String getTypeName() const;
    virtual String getOutTypeName() const;

    virtual void writeDecl(OutStream *os);

    virtual String getReadMethod() const;
    virtual String getWriteMethod() const;

    virtual void writeRead(OutStream *os);
    virtual void writeWrite(OutStream *os);

    virtual void setCond(Member *var, Member *varParam);

    virtual void setValue(const String &value);

    virtual void writeSetterDecl(OutStream *os);

private:

    String m_value;

    Member *m_var;
    Member *m_varParam;
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBERBIT_H
