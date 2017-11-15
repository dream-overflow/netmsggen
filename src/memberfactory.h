/**
 * @file memberfactory.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MEMBERFACTORY_H
#define _O3D_NMG_MEMBERFACTORY_H

#include "member.h"
#include <o3d/core/stringmap.h>

namespace o3d {
namespace nmg {

class MemberFactory
{
public:

    virtual ~MemberFactory();
    virtual Member* buildFromTypeName(const String &type, Member *parent);

    void registerMember(Member *member);

    static MemberFactory* instance();
    static void destroy();

private:

    MemberFactory();

    StringMap<Member*> m_members;

    static MemberFactory *ms_instance;
};

} // namespace o3d
} // namespace nmg

#endif // _O3D_NMG_MEMBERFACTORY_H
