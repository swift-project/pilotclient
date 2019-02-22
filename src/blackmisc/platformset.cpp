/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "platformset.h"
#include "blackmisc/containerbase.h"
#include "blackmisc/dbus.h"
#include "blackmisc/variant.h"

#include <QDBusMetaType>
#include <QJsonObject>
#include <QString>

namespace BlackMisc
{
    CPlatformSet::CPlatformSet() { }

    CPlatformSet::CPlatformSet(std::initializer_list<CPlatform> il) : CCollection<CPlatform>(il)
    { }

    CPlatformSet::CPlatformSet(const CCollection<CPlatform> &other) : CCollection<CPlatform>(other)
    { }

    QStringList CPlatformSet::getPlatformNames() const
    {
        QStringList names;
        for (const CPlatform &p : *this)
        {
            names.append(p.getPlatformName());
        }
        return names;
    }

    CPlatformSet CPlatformSet::matchesAny(CPlatform::Platform platform) const
    {
        CPlatformSet set;
        for (const CPlatform &p : *this)
        {
            if (!p.matchesAny(platform)) continue;
            set.insert(p);
        }
        return set;
    }

    CPlatformSet CPlatformSet::exactMatch(CPlatform::Platform platform) const
    {
        CPlatformSet set;
        for (const CPlatform &p : *this)
        {
            if (p.getPlatform() != platform) continue;
            set.insert(p);
        }
        return set;
    }

    bool CPlatformSet::containsName(const QString &platformName) const
    {
        return this->contains(&CPlatform::getPlatformName, platformName);
    }

    const CPlatformSet &CPlatformSet::allPlatforms()
    {
        static const CPlatformSet platforms({ CPlatform::win32Platform(), CPlatform::win64Platform(), CPlatform::linuxPlatform(), CPlatform::macOSPlatform() });
        return platforms;
    }

    void CPlatformSet::registerMetadata()
    {
        qRegisterMetaType<BlackMisc::CSequence<CPlatform>>();
        qDBusRegisterMetaType<BlackMisc::CSequence<CPlatform>>();
        qRegisterMetaType<BlackMisc::CCollection<CPlatform>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CPlatform>>();
        qRegisterMetaType<CPlatformSet>();
        qDBusRegisterMetaType<CPlatformSet>();
        registerMetaValueType<CPlatformSet>();
    }
} // namespace

//! \endcond
