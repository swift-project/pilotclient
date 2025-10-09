// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/simulatorplugininfolist.h"

#include <algorithm>

#include "misc/range.h"
#include "misc/simulation/simulatorplugininfo.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::simulation, CSimulatorPluginInfo, CSimulatorPluginInfoList)

namespace swift::misc::simulation
{
    bool CSimulatorPluginInfoList::supportsSimulator(const QString &simulator) const
    {
        return std::find_if(begin(), end(), [&simulator](const CSimulatorPluginInfo &info) {
                   return info.getSimulator() == simulator;
               }) != end();
    }

    QStringList CSimulatorPluginInfoList::toStringList(bool i18n) const
    {
        return this->transform([i18n](const CSimulatorPluginInfo &info) { return info.toQString(i18n); });
    }

    CSimulatorPluginInfo CSimulatorPluginInfoList::findByIdentifier(const QString &identifier) const
    {
        return this->findFirstByOrDefault(&CSimulatorPluginInfo::getIdentifier, identifier);
    }

    CSimulatorPluginInfo CSimulatorPluginInfoList::findBySimulator(const CSimulatorInfo &simulator) const
    {
        for (const CSimulatorPluginInfo &info : *this)
        {
            if (info.getSimulatorInfo() == simulator) { return info; }
        }
        return CSimulatorPluginInfo();
    }
} // namespace swift::misc::simulation
