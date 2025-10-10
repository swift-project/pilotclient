// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/simulatorinfolist.h"

#include <tuple>

#include <QList>

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::simulation, CSimulatorInfo, CSimulatorInfoList)

namespace swift::misc::simulation
{
    CSimulatorInfoList::CSimulatorInfoList(const CSequence<CSimulatorInfo> &other) : CSequence<CSimulatorInfo>(other) {}

    CSimulatorInfoList CSimulatorInfoList::withNoDuplicates() const
    {
        if (this->isEmpty()) { return {}; }
        QList<int> simIndexes;
        CSimulatorInfoList newList;
        for (const CSimulatorInfo &simulator : *this)
        {
            const int i = static_cast<int>(simulator.getSimulator());
            if (simIndexes.contains(i)) { continue; }
            newList.push_back(simulator);
            simIndexes.append(i);
        }
        return newList;
    }

    CSimulatorInfoList CSimulatorInfoList::splitIntoSingleSimulators() const
    {
        if (this->isEmpty()) { return {}; }
        CSimulatorInfoList newList;
        for (const CSimulatorInfo &simulator : *this)
        {
            if (simulator.isUnspecified() || simulator.isNoSimulator()) { continue; }
            if (simulator.isSingleSimulator()) { newList.push_back(simulator); }
            else { newList.push_back(splitIntoSingleSimulators(simulator)); }
        }
        return newList;
    }

    CSimulatorInfoList CSimulatorInfoList::splitIntoSingleSimulators(const CSimulatorInfo &sim)
    {
        CSimulatorInfoList sims;
        if (sim.isFS9()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::FS9)); }
        if (sim.isFSX()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::FSX)); }
        if (sim.isP3D()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::P3D)); }
        if (sim.isMSFS()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::MSFS)); }
        if (sim.isMSFS2024()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::MSFS2024)); }
        if (sim.isXPlane()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::XPLANE)); }
        return sims;
    }
} // namespace swift::misc::simulation
