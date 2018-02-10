/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/distributorfilter.h"
#include "blackmisc/simulation/distributor.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Models
    {
        CDistributorFilter::CDistributorFilter(const Simulation::CSimulatorInfo &simulator) :
            m_simulator(simulator)
        {
            this->m_valid = true;
        }

        CDistributorList CDistributorFilter::filter(const CDistributorList &inDistributors) const
        {
            if (!this->isValid()) { return inDistributors; }
            CDistributorList outContainer;
            for (const CDistributor &distributor : inDistributors)
            {
                if (!distributor.getSimulator().matchesAny(m_simulator)) { continue; }
                outContainer.push_back(distributor);
            }
            return outContainer;
        }
    } // namespace
} // namespace
