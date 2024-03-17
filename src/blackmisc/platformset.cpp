// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "blackmisc/platformset.h"
#include "blackmisc/containerbase.h"

#include <QDBusMetaType>
#include <QString>

BLACK_DEFINE_COLLECTION_MIXINS(BlackMisc, CPlatform, CPlatformSet)

namespace BlackMisc
{
    CPlatformSet::CPlatformSet() {}

    CPlatformSet::CPlatformSet(const CCollection<CPlatform> &other) : CCollection<CPlatform>(other)
    {}

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
