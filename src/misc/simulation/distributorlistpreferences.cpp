// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/distributorlistpreferences.h"

#include <QtGlobal>

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation, CDistributorListPreferences)

namespace swift::misc::simulation
{
    const CDistributorList &CDistributorListPreferences::getDistributors(const CSimulatorInfo &simulator) const
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FSX: return m_distributorsFsx;
        case CSimulatorInfo::P3D: return m_distributorsP3d;
        case CSimulatorInfo::FS9: return m_distributorsFs9;
        case CSimulatorInfo::FG: return m_distributorsFG;
        case CSimulatorInfo::XPLANE: return m_distributorsXPlane;
        case CSimulatorInfo::MSFS: return m_distributorsMsfs;
        case CSimulatorInfo::MSFS2024: return m_distributorsMsfs2024;
        default: Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong simulator"); break;
        }

        static const CDistributorList empty;
        return empty;
    }

    CDistributor CDistributorListPreferences::getFirstOrDefaultDistributor(const CSimulatorInfo &simulator) const
    {
        return this->getDistributors(simulator).frontOrDefault();
    }

    void CDistributorListPreferences::setDistributors(const CDistributorList &distributors,
                                                      const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        CDistributorList d(distributors);
        d.sortAscendingByOrder(); // make sure we are sorted by order
        m_lastUpdatedSimulator = simulator;

        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FSX: m_distributorsFsx = d; break;
        case CSimulatorInfo::P3D: m_distributorsP3d = d; break;
        case CSimulatorInfo::FS9: m_distributorsFs9 = d; break;
        case CSimulatorInfo::FG: m_distributorsFG = d; break;
        case CSimulatorInfo::XPLANE: m_distributorsXPlane = d; break;
        case CSimulatorInfo::MSFS: m_distributorsMsfs = d; break;
        case CSimulatorInfo::MSFS2024: m_distributorsMsfs2024 = d; break;
        default: Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong simulator"); break;
        }
    }

    QString CDistributorListPreferences::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        static const QString p("Preferences FSX %1, P3D %2, FS9 %3, XP %4 FG %5");
        return p.arg(m_distributorsFsx.size())
            .arg(m_distributorsP3d.size())
            .arg(m_distributorsFs9.size())
            .arg(m_distributorsXPlane.size())
            .arg(m_distributorsFG.size());
    }
} // namespace swift::misc::simulation
