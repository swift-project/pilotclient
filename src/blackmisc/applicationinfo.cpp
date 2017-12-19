/* Copyright (C) 2016
* swift project Community / Contributors
*
* This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
* directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
* including this file, may be copied, modified, propagated, or distributed except according to the terms
* contained in the LICENSE file.
*/

//! \file

#include "blackmisc/applicationinfo.h"

namespace BlackMisc
{
    CApplicationInfo::CApplicationInfo() {}

    CApplicationInfo::CApplicationInfo(Application app, const QString &exePath, const QString &version, const CProcessInfo &process) :
        m_app(app),
        m_exePath(exePath),
        m_version(version),
        m_process(process)
    {}

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
}
