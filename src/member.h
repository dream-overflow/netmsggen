/**
 * @file member.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBER_H
#define _O3D_NMG_MEMBER_H

#include <o3d/core/string.h>
#include <o3d/core/stringmap.h>
#include <o3d/core/outstream.h>

namespace o3d {
namespace nmg {

class Member
{
public:

    enum Type
    {
        TYPE_BOOL,
        TYPE_INT8,
        TYPE_UINT8,
        TYPE_INT16,
        TYPE_UINT16,
        TYPE_INT32,
        TYPE_UINT32,
        TYPE_INT64,
        TYPE_UINT64,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_BITSET8,
        TYPE_BITSET16,
        TYPE_BITSET32,
        TYPE_BITSET64,
        TYPE_ARRAY8,
        TYPE_UINT8_ARRAY,
        TYPE_STRUCT,
        TYPE_LOOP,
        TYPE_IF,
        TYPE_IFNOT,
        TYPE_IFEQ,
        TYPE_IFNEQ,
        TYPE_BIT,
        TYPE_CONST_INT16,
        TYPE_CONST_UINT32,
        TYPE_IMMEDIATE_UINT32,
        TYPE_CTOR
    };

    enum SetValue
    {
        SET_FALSE,
        SET_TRUE
    };

    Member(Member *parent);

    virtual ~Member();

    virtual UInt32 getType() const = 0;
    virtual String getTypeName() const = 0;

    virtual String getOutTypeName() const = 0;
    virtual String getReadMethod() const = 0;
    virtual String getWriteMethod() const = 0;

    //! Get the related include file (default is "" none)
    virtual String getHeader() const;

    //! Make an instance of the member
    virtual Member* makeInstance(Member *parent) const = 0;

    void setName(const String &name) { m_name = name; }
    const String& getName() const { return m_name; }

    Member* getParent() { return m_parent; }
    const Member* getParent() const { return m_parent; }

    virtual String getIfTest(const Member *param) const;

    virtual UInt32 getNewUIntId();

    Bool isParent() const { return m_parent != nullptr; }

    virtual void addMember(Member *member);

    //! Find a direct child member, or if not found up to the parent recursively.
    virtual Member* findMember(const String &name) const;

    virtual void setCond(Member *var, Member *varParam);

    //! Default is private.
    virtual void setPublic();
    virtual void setPrivate();

    virtual Bool isPrivate() const;
    virtual Bool isPublic() const;

    virtual void setValue(const String &value);

    virtual void writeSetterDecl(OutStream *os);
    virtual void writeSetterImpl(OutStream *os);

    virtual void writeDecl(OutStream *os);

    virtual void writeRead(OutStream *os);
    virtual void writeWrite(OutStream *os);

    //! Return the number of ident
    virtual UInt32 getIdent() const;

    //! Get the read/write children prefix
    virtual String getPrefix() const;

    //! Get the size in byte
    virtual UInt32 getMinSize() const;

    //! Reference in param (reference and not copy)
    virtual Bool isRef() const;

    //! get a "Set to TRUE, FALSE or any other value".
    virtual String getSetTo(const Member *param, SetValue value) const;

    //! Get size of the member.
    virtual String getSizeOf() const;

private:

    Member *m_parent;

    String m_name;
    Bool m_public;
};

/**
 * @brief Abstract member class helper
 * @date 2013-11-19
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
template <class T, UInt32 E>
class MemberHelper : public Member
{
public:

    MemberHelper(Member *parent) :
        Member(parent)
    {

    }

    virtual UInt32 getType() const { return (UInt32)E; }
    static MemberHelper* createInstance(Member *parent) { return new T(parent); }
    virtual MemberHelper* makeInstance(Member *parent) const { return new T(parent); }
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBER_H
