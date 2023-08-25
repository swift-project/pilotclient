// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/distributorfilter.h"
#include "blackmisc/simulation/distributor.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui::Models
{
    CDistributorFilter::CDistributorFilter(const CSimulatorInfo &simulator) : m_simulator(simulator)
    {
        m_valid = true;
    }

    CDistributorList CDistributorFilter::filter(const CDistributorList &inDistributors) const
    {
        if (!this->isValid()) { return inDistributors; }
        if (this->ignoreSimulator()) { return inDistributors; }

        CDistributorList outContainer;
        for (const CDistributor &distributor : inDistributors)
        {
            if (!distributor.getSimulator().matchesAnyOrNone(m_simulator)) { continue; }
            outContainer.push_back(distributor);
        }
        return outContainer;
    }

    bool CDistributorFilter::ignoreSimulator() const
    {
        return (m_simulator.isNoSimulator() || m_simulator.isAllSimulators());
    }
} // namespace
