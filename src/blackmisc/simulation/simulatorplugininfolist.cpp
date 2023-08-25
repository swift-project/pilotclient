// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/range.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"

#include <algorithm>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Simulation, CSimulatorPluginInfo, CSimulatorPluginInfoList)

namespace BlackMisc::Simulation
{
    CSimulatorPluginInfoList::CSimulatorPluginInfoList() {}

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
} // namespace
