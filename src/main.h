/**
 * @file main.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NMG_MAIN_H
#define _O3D_NMG_MAIN_H

#include <o3d/core/evt.h>
#include <o3d/core/baseobject.h>

#include <o3d/core/memorymanager.h>
#include <o3d/core/stringlist.h>
#include <o3d/core/stringmap.h>

#include <o3d/core/idmanager.h>

#include "messagefile.h"

namespace o3d {
namespace nmg {

/**
 * @brief The Main class
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-19
 */
class Main : public BaseObject
{
public:

    //! Get singleton.
    static Main* instance();

    //! Default constructor.
    Main();

    //! Destructor.
    virtual ~Main();

    void init();

    void run();

    Int32 command();

    void renameMessage(
            const String &filenameFrom,
            const String &filenameTo,
            const String &from,
            const String &to);

    void renameMessageHeader(
            const String &filenameFrom,
            const String &filenameTo,
            const String &from,
            const String &to);

    void renameMessageImpl(const String &filenameFrom,
            const String &filenameTo,
            const String &from,
            const String &to);

    //-----------------------------------------------------------------------------------
    // Accessors
    //-----------------------------------------------------------------------------------

    enum Profile
    {
        CLIENT = 0,
        SERVER = 1
    };

    const String& getInPath() const { return m_inPath; }
    const String& getOutPath(Profile p) const { return m_outPath[p]; }

    const String& getMsgExt() const { return m_msgExt; }
    const String& getHppExt() const { return m_hppExt; }
    const String& getCppExt() const { return m_cppExt; }

    const T_StringList& getLicenseTpl() const { return m_licenseTpl; }
    const T_StringList& getHppTpl() const { return m_hppTpl; }
    const T_StringList& getCppTpl() const { return m_cppTpl; }
    const T_StringList& getMessageInClassTpl() const { return m_messageInClassTpl; }
    const T_StringList& getMessageOutClassTpl() const { return m_messageOutClassTpl; }
    const T_StringList& getMessageInImplTpl() const { return m_messageInImplTpl; }
    const T_StringList& getMessageInImplRunlTpl() const { return m_messageInImplRunTpl; }
    const T_StringList& getMessageOutImplTpl() const { return m_messageOutImplTpl; }

    UInt32 getVersion() const { return m_version; }

    UInt32 getNextMessageId();
    void registerMessageId(UInt32 messageId);

    Bool isBuild(Profile p) const { return m_build[p]; }

    const String& getNamespace(Profile p) const { return m_namespace[p]; }
    const String& getAuthor() const { return m_author; }

    const String& getVersionName() const { return m_versionName; }

    const String& getYear() const { return m_year; }
    const String& getMonth() const { return m_month; }
    const String& getDay() const { return m_day; }

    const String& getMessageFactoryInstance(Profile p) const { return m_messageFactoryInstance[p]; }
    const String& getMessageFactoryHeader(Profile p) const { return m_messageFactoryHeader[p]; }

private:

    static Main *ms_instance;

    String m_inPath;
    String m_tplPath;
    String m_outPath[2];

    String m_msgExt;
    String m_hppExt;
    String m_cppExt;

    UInt32 m_version;

    Bool m_build[2];

    IDManager m_messageId;

    T_StringList m_licenseTpl;
    T_StringList m_hppTpl;
    T_StringList m_cppTpl;
    T_StringList m_messageInClassTpl;
    T_StringList m_messageOutClassTpl;
    T_StringList m_messageInImplTpl;
    T_StringList m_messageInImplRunTpl;
    T_StringList m_messageOutImplTpl;

    String m_versionName;

    String m_namespace[2];
    String m_author;

    String m_messageFactoryInstance[2];
    String m_messageFactoryHeader[2];

    String m_year;
    String m_month;
    String m_day;

    void readTemplate(const String &filename, T_StringList &lines);
    void readConfig(const String &filename);

public:

    static Int32 main();
};

} // namespace nmg
} // namespace o3d

#endif // _O3D_NMG_MAIN_H
