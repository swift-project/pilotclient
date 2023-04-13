/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
