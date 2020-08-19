/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/platform.h"
#include "blackmisc/icons.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/comparefunctions.h"
#include "blackconfig/buildconfig.h"

#include <QJsonValue>
#include <Qt>
#include <QtGlobal>

using namespace BlackConfig;

namespace BlackMisc
{
    CPlatform::CPlatform(const QString &p) : m_platform(stringToPlatform(p))
    { }

    CPlatform::CPlatform(Platform p) : m_platform(p)
    { }

    CPlatform::PlatformFlag CPlatform::getPlatformFlag() const
    {
        if (this->isSinglePlatform()) { return static_cast<PlatformFlag>(m_platform); }
        return UnknownOs;
    }

    bool CPlatform::matchesAny(const CPlatform &otherPlatform) const
    {
        return (this->m_platform & otherPlatform.m_platform) > 0;
    }

    int CPlatform::numberPlatforms() const
    {
        const Platform p = this->getPlatform();
        int c = 0;
        if (p.testFlag(Win32)) c++;
        if (p.testFlag(Win64)) c++;
        if (p.testFlag(MacOS)) c++;
        if (p.testFlag(Linux)) c++;
        return c;
    }

    bool CPlatform::isSinglePlatform() const
    {
        return this->numberPlatforms() == 1;
    }

    bool CPlatform::isAnyWindows() const
    {
        const Platform p = this->getPlatform();
        return p.testFlag(Win32) || p.testFlag(Win64);
    }

    QString CPlatform::getPlatformName() const
    {
        return this->convertToQString(true);
    }

    CIcon CPlatform::toIcon() const
    {
        if (this->getPlatform() == All) { return CIcon::iconByIndex(CIcons::OSAll); }
        switch (this->getPlatformFlag())
        {
        case Win32:
        case Win64: return CIcon::iconByIndex(CIcons::OSWindows);
        case Linux: return CIcon::iconByIndex(CIcons::OSLinux);
        case MacOS: return CIcon::iconByIndex(CIcons::OSMacOs);
        default: break;
        }
        return CIcon::iconByIndex(CIcons::StandardIconEmpty);
    }

    QString CPlatform::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        switch (m_platform)
        {
        case Win32: return QStringLiteral("Win32");
        case Win64: return QStringLiteral("Win64");
        case Linux: return QStringLiteral("Linux");
        case MacOS: return QStringLiteral("MacOSX");
        default: break;
        }
        return QStringLiteral("unknown");
    }

    CVariant CPlatform::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexPlatform: return CVariant::fromValue(m_platform);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CPlatform::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.to<CPlatform>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexPlatform: this->setPlatform(static_cast<Platform>(variant.toInt())); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CPlatform::comparePropertyByIndex(const CPropertyIndex &index, const CPlatform &compareValue) const
    {
        if (index.isMyself()) { return Compare::compare(m_platform, compareValue.m_platform); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexPlatform: return Compare::compare(m_platform, compareValue.m_platform);
        default: Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison possible");
        }
        return 0;
    }

    CPlatform::Platform CPlatform::stringToPlatform(const QString &str)
    {
        const QString s(str.trimmed().toLower());
        if (s.contains("win"))
        {
            if (s.contains("32")) return Win32;
            return Win64;
        }
        if (s.contains("linux")) return Linux;
        if (s.contains("mac") || s.contains("osx")) return MacOS;

        // special ones
        if (s.contains("debian")) return Linux;
        if (s.contains("darwin")) return MacOS;
        if (s.contains("all")) return All;
        if (s.contains("independent")) return Independent;

        return UnknownOs;
    }

    const CPlatform &CPlatform::stringToPlatformObject(const QString &str)
    {
        switch (stringToPlatform(str))
        {
        case Win32: return CPlatform::win32Platform();
        case Win64: return CPlatform::win64Platform();
        case Linux: return CPlatform::linuxPlatform();
        case MacOS: return CPlatform::macOSPlatform();
        default: break;
        }
        return unknownOs();
    }

    namespace Private
    {
        const CPlatform &currentPlatformImpl()
        {
            if (CBuildConfig::isRunningOnWindowsNtPlatform())
            {
                const int wordSize = CBuildConfig::buildWordSize();
                return wordSize == 64 ? CPlatform::win64Platform() : CPlatform::win32Platform();
            }

            if (CBuildConfig::isRunningOnLinuxPlatform()) { return CPlatform::linuxPlatform(); }
            if (CBuildConfig::isRunningOnMacOSPlatform()) { return CPlatform::macOSPlatform(); }
            return CPlatform::unknownOs();
        }
    }

    const CPlatform &CPlatform::currentPlatform()
    {
        static const CPlatform p = Private::currentPlatformImpl();
        return p;
    }

    bool CPlatform::isCurrentPlatform(const QString &platform)
    {
        if (platform.isEmpty()) { return false; }
        return isCurrentPlatform(CPlatform::stringToPlatform(platform));
    }

    bool CPlatform::isCurrentPlatform(const CPlatform &platform)
    {
        return platform == CPlatform::currentPlatform();
    }

    bool CPlatform::canRunOnCurrentPlatform(const CPlatform &platform)
    {
        if (platform == currentPlatform()) { return true; }
        if (platform.isAnyWindows() && currentPlatform().isAnyWindows()) { return true; }
        if (platform == CPlatform::allOs()) { return true; }
        if (platform == CPlatform::independent()) { return true; }
        return false;
    }

    const CPlatform &CPlatform::win32Platform()
    {
        static const CPlatform p(Win32);
        return p;
    }

    const CPlatform &CPlatform::win64Platform()
    {
        static const CPlatform p(Win64);
        return p;
    }

    const CPlatform &CPlatform::linuxPlatform()
    {
        static const CPlatform p(Linux);
        return p;
    }

    const CPlatform &CPlatform::macOSPlatform()
    {
        static const CPlatform p(MacOS);
        return p;
    }

    const CPlatform &CPlatform::unknownOs()
    {
        static const CPlatform p(UnknownOs);
        return p;
    }

    const CPlatform &CPlatform::allOs()
    {
        static const CPlatform p(All);
        return p;
    }

    const CPlatform &CPlatform::independent()
    {
        static const CPlatform p(All);
        return p;
    }
} // namespace
