/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_APPLICATIONINFO_H
#define BLACKMISC_APPLICATIONINFO_H

#include "blackmisc/processinfo.h"
#include <QString>
#include <QMetaType>
#include <QFlags>

namespace BlackMisc
{
    /*!
     * Description of a swift application.
     *
     * This is basically a CValueObject compliant version of CBuildConfig combined with CProcessInfo
     * so we can store the information as JSON and transfer them via DBus.
     */
    class BLACKMISC_EXPORT CApplicationInfo : public CValueObject<CApplicationInfo>
    {
    public:
        //! Enumeration of application roles
        enum Application
        {
            Unknown,
            Laucher,
            PilotClientCore,
            PilotClientGui,
            MappingTool,
            UnitTest,
            Sample
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexApplication = CPropertyIndexRef::GlobalIndexCApplicationInfo,
            IndexApplicationAsString,
            IndexApplicationDataPath,
            IndexCompileInfo,
            IndexExecutablePath,
            IndexExecutablePathExisting,
            IndexPlatformInfo,
            IndexProcessInfo,
            IndexVersionString,
            IndexWordSize
        };

        //! Default constructor.
        CApplicationInfo();

        //! Constructor.
        CApplicationInfo(Application app);

        //! Set application.
        void setApplication(Application app) { m_app = static_cast<int>(app); }

        //! Get application.
        Application getApplication() const { return static_cast<Application>(m_app); }

        //! get application as string
        const QString &getApplicationAsString() const;

        //! Set executable path.
        void setExecutablePath(const QString &exePath) { m_exePath = exePath; }

        //! Get executable path.
        const QString &getExecutablePath() const { return m_exePath; }

        //! Is the executable path existing?
        //! \remark this indicates if the swift version is still existing
        //! \remark file check, relatively slow
        bool isExecutablePathExisting() const;

        //! Set application data dir
        //! \remark rootdir of settings, cache and logs
        void setApplicationDataDirectory(const QString &appDataDir) { m_applicationDataDir = appDataDir; }

        //! Set application data dir
        //! \remark rootdir of settings, cache and logs
        const QString &getApplicationDataDirectory() const { return m_applicationDataDir; }

        //! Set version string.
        void setVersionString(const QString &version) { m_version = version; }

        //! Get version string.
        const QString &getVersionString() const { return m_version; }

        //! Set process info.
        void setProcessInfo(const CProcessInfo &process) { m_process = process; }

        //! Get process info.
        const CProcessInfo &getProcessInfo() const { return m_process; }

        //! Set platform.
        void setPlatformInfo(const QString &platform) { m_platform = platform; }

        //! Get platform.
        const QString &getPlatform() const { return m_platform; }

        //! Word size
        int getWordSize() const { return m_wordSize; }

        //! Word size
        void setWordSize(int size) { m_wordSize = size; }

        //! Compile info
        const QString &getCompileInfo() const { return m_compileInfo; }

        //! Compile info
        void setCompileInfo(const QString &info) { m_compileInfo = info; }

        //! Sample or unit test
        bool isSampleOrUnitTest() const;

        //! Unit test
        bool isUnitTest() const;

        //! Null object
        bool isNull() const;

        //! Formatted info
        QString asOtherSwiftVersionString(const QString &separator = " | ") const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Icon::toIcon()
        CIcons::IconIndex toIcon() const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CApplicationInfo &compareValue) const;

        //! Name of pilot client GUI
        static const QString &swiftPilotClientGui();

        //! Name of swift launcher
        static const QString &swiftLauncher();

        //! Name of swift mapping tool
        static const QString &swiftMappingTool();

        //! Name of swift core
        static const QString &swiftCore();

        //! Info automatically initalized
        static const CApplicationInfo &autoInfo();

        //! File name of the application info file
        static const QString &fileName();

        //! NULL info
        static const CApplicationInfo &null();

    private:
        int m_app = static_cast<int>(Unknown);
        int m_wordSize;
        QString m_exePath;
        QString m_applicationDataDir;
        QString m_version;
        QString m_compileInfo;
        QString m_platform;
        CProcessInfo m_process;

        //! Guess Application
        static Application guessApplication();

        BLACK_METACLASS(
            CApplicationInfo,
            BLACK_METAMEMBER(app),
            BLACK_METAMEMBER(wordSize),
            BLACK_METAMEMBER(exePath),
            BLACK_METAMEMBER(applicationDataDir),
            BLACK_METAMEMBER(version),
            BLACK_METAMEMBER(compileInfo),
            BLACK_METAMEMBER(platform),
            BLACK_METAMEMBER(process)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CApplicationInfo)

#endif
