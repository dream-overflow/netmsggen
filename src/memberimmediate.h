/**
 * @file memberimmediate.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERIMMEDIATEUINT32_H
#define _O3D_NMG_MEMBERIMMEDIATEUINT32_H

#include "member.h"

namespace o3d {
namespace nmg {

/**
 * @brief The MemberImmediate have its name for the immediate value.
 */
class MemberImmediate : public MemberHelper<MemberImmediate, Member::TYPE_IMMEDIATE_UINT32>
{
public:

    MemberImmediate(Member *parent);

    virtual String getTypeName() const;
    virtual String getOutTypeName() const;

    virtual String getReadMethod() const;
    virtual String getWriteMethod() const;

    virtual void writeDecl(OutStream *os);

    virtual void writeRead(OutStream *os);
    virtual void writeWrite(OutStream *os);

    virtual void writeSetterDecl(OutStream *os);
    virtual void writeSetterImpl(OutStream *os);
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBERIMMEDIATEUINT32_H
