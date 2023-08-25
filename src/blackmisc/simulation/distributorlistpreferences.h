// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_DISTRIBUTORLISTPREFRENCES_H
#define BLACKMISC_SIMULATION_DISTRIBUTORLISTPREFRENCES_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/containerbase.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/valueobject.h"

#include <QHash>
#include <QMetaType>
#include <QString>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Simulation, CDistributorListPreferences)

namespace BlackMisc::Simulation
{
    //! Preferences for distributors
    class BLACKMISC_EXPORT CDistributorListPreferences :
        public CValueObject<CDistributorListPreferences>
    {
    public:
        //! Default constructor.
        CDistributorListPreferences();

        //! Get distributors
        const CDistributorList &getDistributors(const CSimulatorInfo &simulator) const;

        //! Get distributor for simulator
        CDistributor getFirstOrDefaultDistributor(const CSimulatorInfo &simulator) const;

        //! Last updated simulator
        const CSimulatorInfo &getLastUpdatedSimulator() const { return m_lastUpdatedSimulator; }

        //! Get distributors
        void setDistributors(const CDistributorList &distributors, const CSimulatorInfo &simulator);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        CDistributorList m_distributorsFsx;
        CDistributorList m_distributorsP3d;
        CDistributorList m_distributorsFs9;
        CDistributorList m_distributorsXPlane;
        CDistributorList m_distributorsFG;
        CSimulatorInfo m_lastUpdatedSimulator;

        BLACK_METACLASS(
            CDistributorListPreferences,
            BLACK_METAMEMBER(distributorsFsx),
            BLACK_METAMEMBER(distributorsP3d),
            BLACK_METAMEMBER(distributorsFs9),
            BLACK_METAMEMBER(distributorsXPlane),
            BLACK_METAMEMBER(distributorsFG),
            BLACK_METAMEMBER(lastUpdatedSimulator)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CDistributorListPreferences)

#endif // guard
