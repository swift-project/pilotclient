/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/distributorlistpreferences.h"

#include <QtGlobal>

namespace BlackMisc
{
    namespace Simulation
    {
        CDistributorListPreferences::CDistributorListPreferences() { }

        const CDistributorList &CDistributorListPreferences::getDistributors(const CSimulatorInfo &simulator) const
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            switch (simulator.getSimulator())
            {
            case CSimulatorInfo::FSX: return this->m_distributorsFsx;
            case CSimulatorInfo::P3D: return this->m_distributorsP3d;
            case CSimulatorInfo::FS9: return this->m_distributorsFs9;
            case CSimulatorInfo::XPLANE: return this->m_distributorsXPlane;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong simulator");
                break;
            }

            static const CDistributorList empty;
            return empty;
        }

        void CDistributorListPreferences::setDistributors(const CDistributorList &distributors, const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            CDistributorList d(distributors);
            d.sortAscendingByOrder(); // make sure we are sorted by order
            this->m_lastUpdatedSimulator = simulator;

            switch (simulator.getSimulator())
            {
            case CSimulatorInfo::FSX: this->m_distributorsFsx = d; break;
            case CSimulatorInfo::P3D:  this->m_distributorsP3d  = d; break;
            case CSimulatorInfo::FS9: this->m_distributorsFs9 = d; break;
            case CSimulatorInfo::XPLANE: this->m_distributorsXPlane = d; break;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong simulator");
                break;
            }
        }

        QString CDistributorListPreferences::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            static const QString p("Preferences FSX %1, P3D %2, FS9 %3, XP %4");
            return p.arg(this->m_distributorsFsx.size()).arg(this->m_distributorsP3d.size()).arg(this->m_distributorsFs9.size()).arg(this->m_distributorsXPlane.size());
        }
    } // namespace
} // namespace
