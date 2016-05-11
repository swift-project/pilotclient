/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/simulatorinfolist.h"

#include <QList>
#include <tuple>

namespace BlackMisc
{
    namespace Simulation
    {
        CSimulatorInfoList::CSimulatorInfoList() { }

        CSimulatorInfoList::CSimulatorInfoList(const CSequence<CSimulatorInfo> &other) :
            CSequence<CSimulatorInfo>(other)
        { }

        CSimulatorInfoList CSimulatorInfoList::withNoDuplicates() const
        {
            if (this->isEmpty()) { return CSimulatorInfoList(); }
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
            if (this->isEmpty()) { return CSimulatorInfoList(); }
            CSimulatorInfoList newList;
            for (const CSimulatorInfo &simulator : *this)
            {
                if (simulator.isUnspecified() || simulator.isNoSimulator()) { continue; }
                if (simulator.isSingleSimulator())
                {
                    newList.push_back(simulator);
                }
                else
                {
                    newList.push_back(splitIntoSingleSimulators(simulator));
                }
            }
            return newList;
        }

        CSimulatorInfoList CSimulatorInfoList::splitIntoSingleSimulators(const CSimulatorInfo &sim)
        {
            CSimulatorInfoList sims;
            if (sim.fs9()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::FS9)); }
            if (sim.fsx()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::FSX)); }
            if (sim.p3d()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::P3D)); }
            if (sim.xplane()) { sims.push_back(CSimulatorInfo(CSimulatorInfo::XPLANE)); }
            return sims;
        }

    } // namespace
} // namespace
