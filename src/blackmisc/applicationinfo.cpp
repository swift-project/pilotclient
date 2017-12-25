/* Copyright (C) 2016
* swift project Community / Contributors
*
* This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
* directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
* including this file, may be copied, modified, propagated, or distributed except according to the terms
* contained in the LICENSE file.
*/

//! \file

#include "blackconfig/buildconfig.h"
#include "blackmisc/applicationinfo.h"

using namespace BlackConfig;

namespace BlackMisc
{
    CApplicationInfo::CApplicationInfo() {}

    CApplicationInfo::CApplicationInfo(Application app) :
        m_app(app),
        m_wordSize(CBuildConfig::buildWordSize()),
        m_exePath(QCoreApplication::applicationDirPath()),
        m_version(CBuildConfig::getVersionString()),
        m_compileInfo(CBuildConfig::compiledWithInfo()),
        m_platform(CBuildConfig::getPlatformString()),
        m_process(CProcessInfo::currentProcess())
    {
        if (app == CApplicationInfo::Unknown)
        {
            m_app = guessApplication();
        }
    }

    bool CApplicationInfo::isSampleOrUnitTest() const
    {
        const Application a = this->application();
        return a == CApplicationInfo::Sample || a == CApplicationInfo::UnitTest;
    }

    bool CApplicationInfo::isUnitTest() const
    {
        const Application a = this->application();
        return a == CApplicationInfo::UnitTest;
    }

    bool CApplicationInfo::isNull() const
    {
        return this->application() == Unknown && m_exePath.isNull();
    }

    QString CApplicationInfo::convertToQString(bool i18n) const
    {
        return QString("{ %1, %2, %3, %4 }").arg(QString::number(m_app), m_exePath, m_version, m_process.convertToQString(i18n));
    }

    const QString &CApplicationInfo::swiftPilotClientGui()
    {
        static const QString s("swift pilot client GUI");
        return s;
    }

    const QString &CApplicationInfo::swiftLauncher()
    {
        static const QString s("swift launcher");
        return s;
    }

    const QString &CApplicationInfo::swiftMappingTool()
    {
        static const QString s("swift mapping tool");
        return s;
    }

    const QString &CApplicationInfo::swiftCore()
    {
        static const QString s("swift core");
        return s;
    }

    const CApplicationInfo &CApplicationInfo::autoInfo()
    {
        static const CApplicationInfo info(CApplicationInfo::Unknown);
        return info;
    }

    const QString &CApplicationInfo::fileName()
    {
        static const QString fn("appinfo.json");
        return fn;
    }

    CApplicationInfo::Application CApplicationInfo::guessApplication()
    {
        const QString a(QCoreApplication::instance()->applicationName().toLower());
        if (a.contains("test"))     { return CApplicationInfo::UnitTest; } // names like testcore
        if (a.contains("sample"))   { return CApplicationInfo::Sample; }
        if (a.contains("core"))     { return CApplicationInfo::PilotClientCore; }
        if (a.contains("launcher")) { return CApplicationInfo::Laucher; }
        if (a.contains("gui"))      { return CApplicationInfo::PilotClientGui; }
        if (a.contains("data") || a.contains("mapping")) { return CApplicationInfo::MappingTool; }
        return CApplicationInfo::Unknown;
    }
}
