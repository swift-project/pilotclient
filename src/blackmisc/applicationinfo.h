/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_APPLICATIONINFO_H
#define BLACKMISC_APPLICATIONINFO_H

#include "blackmisc/processinfo.h"
#include <QMetaType>
#include <QFlags>

namespace BlackMisc
{
    /*!
     * Description of a swift application.
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

        //! Flags describing application modes
        enum ApplicationModeFlag
        {
            None      = 0,
            Developer = 1 << 0,
            BetaTest  = 1 << 1
        };
        Q_DECLARE_FLAGS(ApplicationMode, ApplicationModeFlag)

        //! Default constructor.
        CApplicationInfo();

        //! Constructor.
        CApplicationInfo(Application app, ApplicationMode mode, const QString &exePath, const QString &version, const CProcessInfo &process);

        //! Set application.
        void setApplication(Application app) { m_app = static_cast<int>(app); }

        //! Get application.
        Application application() const { return static_cast<Application>(m_app); }

        //! Set application mode.
        void setApplicationMode(ApplicationMode mode) { m_mode = static_cast<int>(mode); }

        //! Get application mode.
        ApplicationMode applicationMode() const { return static_cast<ApplicationMode>(m_mode); }

        //! Set executable path.
        void setExecutablePath(const QString &exePath) { m_exePath = exePath; }

        //! Get executable path.
        const QString &executablePath() const { return m_exePath; }

        //! Set version string.
        void setVersionString(const QString &version) { m_version = version; }

        //! Get version string.
        const QString &versionString() const { return m_version; }

        //! Set process info.
        void setProcessInfo(const CProcessInfo &process) { m_process = process; }

        //! Get process info.
        const CProcessInfo &processInfo() const { return m_process; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Name of pilot client GUI
        static const QString &swiftPilotClientGui();

        //! Name of swift launcher
        static const QString &swiftLauncher();

        //! Name of swift mapping tool
        static const QString &swiftMappingTool();

        //! Name of swift core
        static const QString &swiftCore();

    private:
        int m_app = static_cast<int>(Unknown);
        int m_mode = 0;
        QString m_exePath;
        QString m_version;
        CProcessInfo m_process;

        BLACK_METACLASS(
            CApplicationInfo,
            BLACK_METAMEMBER(app),
            BLACK_METAMEMBER(mode),
            BLACK_METAMEMBER(exePath),
            BLACK_METAMEMBER(version),
            BLACK_METAMEMBER(process)
        );
    };
}

Q_DECLARE_METATYPE(BlackMisc::CApplicationInfo)

#endif
