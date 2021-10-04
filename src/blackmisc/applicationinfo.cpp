/* Copyright (C) 2016
* swift project Community / Contributors
*
* This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
* directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
*/

#include "blackmisc/applicationinfo.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/comparefunctions.h"
#include "blackconfig/buildconfig.h"

#include <QDir>
#include <QStringBuilder>

using namespace BlackConfig;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc, CApplicationInfo)

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

    const QString &CApplicationInfo::getApplicationAsString() const
    {
        static const QString unknown("unknown");
        static const QString launcher("launcher");
        static const QString core("core");
        static const QString gui("gui");
        static const QString mapping("mapping tool");
        static const QString unitTest("unit test");
        static const QString sample("sample");

        switch (getApplication())
        {
        case Laucher: return launcher;
        case PilotClientCore: return core;
        case PilotClientGui: return gui;
        case MappingTool: return mapping;
        case UnitTest: return unitTest;
        case Sample: return sample;
        default: break;
        }
        return unknown;
    }

    bool CApplicationInfo::isExecutablePathExisting() const
    {
        if (this->getExecutablePath().isEmpty()) { return false; }
        const QDir d(this->getExecutablePath());
        return d.exists();
    }

    bool CApplicationInfo::isSampleOrUnitTest() const
    {
        const Application a = this->getApplication();
        return a == CApplicationInfo::Sample || a == CApplicationInfo::UnitTest;
    }

    bool CApplicationInfo::isUnitTest() const
    {
        const Application a = this->getApplication();
        return a == CApplicationInfo::UnitTest;
    }

    bool CApplicationInfo::isNull() const
    {
        return this->getApplication() == Unknown && m_exePath.isNull();
    }

    QString CApplicationInfo::asOtherSwiftVersionString(const QString &separator) const
    {
        return u"Version; "  % this->getVersionString() % u" os: " % this->getPlatform() % separator %
               u"exe.path: " % this->getExecutablePath() % separator %
               u"app.data: " % this->getApplicationDataDirectory();
    }

    QString CApplicationInfo::convertToQString(bool i18n) const
    {
        return QStringLiteral("{ %1, %2, %3, %4 }").arg(this->getApplicationAsString(), m_exePath, m_version, m_process.convertToQString(i18n));
    }

    CIcons::IconIndex CApplicationInfo::toIcon() const
    {
        switch (getApplication())
        {
        case Laucher: return CIcons::SwiftLauncher16;
        case PilotClientCore: return CIcons::SwiftCore16;
        case PilotClientGui: return CIcons::Swift16;
        case MappingTool: return CIcons::SwiftDatabase16;
        default: break;
        }
        return CIcons::StandardIconUnknown16;
    }

    QVariant CApplicationInfo::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexApplication: return QVariant::fromValue(m_app);
        case IndexApplicationAsString: return QVariant::fromValue(this->getApplicationAsString());
        case IndexApplicationDataPath: return QVariant::fromValue(this->getApplicationDataDirectory());
        case IndexCompileInfo: return QVariant::fromValue(this->getCompileInfo());
        case IndexExecutablePath: return QVariant::fromValue(this->getExecutablePath());
        case IndexExecutablePathExisting: return QVariant::fromValue(this->isExecutablePathExisting());
        case IndexPlatformInfo: return QVariant::fromValue(this->getPlatform());
        case IndexProcessInfo: return m_process.propertyByIndex(index.copyFrontRemoved());
        case IndexVersionString: return QVariant::fromValue(this->getVersionString());
        case IndexWordSize: return QVariant::fromValue(this->getWordSize());
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CApplicationInfo::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CApplicationInfo>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexApplication: this->setApplication(static_cast<Application>(variant.toInt())); break;
        case IndexApplicationAsString: break;
        case IndexApplicationDataPath: this->setApplicationDataDirectory(variant.toString()); break;
        case IndexCompileInfo: this->setCompileInfo(variant.toString()); break;
        case IndexExecutablePath: this->setExecutablePath(variant.toString()); break;
        case IndexExecutablePathExisting: break;
        case IndexPlatformInfo: this->setPlatformInfo(variant.toString()); break;
        case IndexProcessInfo: m_process.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexVersionString: this->setVersionString(variant.toString()); break;
        case IndexWordSize: this->setWordSize(variant.toInt()); break;
        default: break;
        }
        CValueObject::setPropertyByIndex(index, variant);
    }

    int CApplicationInfo::comparePropertyByIndex(CPropertyIndexRef index, const CApplicationInfo &compareValue) const
    {
        if (index.isMyself()) { return this->getExecutablePath().compare(compareValue.getExecutablePath()); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexApplicationDataPath: return this->getApplicationDataDirectory().compare(compareValue.getApplicationDataDirectory());
        case IndexCompileInfo: return this->getCompileInfo().compare(compareValue.getCompileInfo());
        case IndexExecutablePath: return this->getExecutablePath().compare(compareValue.getExecutablePath());
        case IndexExecutablePathExisting: return Compare::compare(this->isExecutablePathExisting(), compareValue.isExecutablePathExisting());
        case IndexPlatformInfo: return this->getPlatform().compare(compareValue.getPlatform());
        case IndexProcessInfo: return this->getProcessInfo().processName().compare(compareValue.getProcessInfo().processName());
        case IndexVersionString: return this->getVersionString().compare(compareValue.getVersionString());
        case IndexWordSize: return Compare::compare(this->getWordSize(), compareValue.getWordSize());
        case IndexApplication:
        case IndexApplicationAsString:
            return Compare::compare(m_app, compareValue.m_app);
        default: return CValueObject::comparePropertyByIndex(index.copyFrontRemoved(), compareValue);
        }
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

    const CApplicationInfo &CApplicationInfo::null()
    {
        static const CApplicationInfo n;
        return n;
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
} // ns
