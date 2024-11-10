// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_DISTRIBUTORLISTPREFRENCES_H
#define SWIFT_MISC_SIMULATION_DISTRIBUTORLISTPREFRENCES_H

#include "misc/swiftmiscexport.h"
#include "misc/containerbase.h"
#include "misc/dictionary.h"
#include "misc/metaclass.h"
#include "misc/simulation/distributorlist.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/valueobject.h"

#include <QHash>
#include <QMetaType>
#include <QString>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CDistributorListPreferences)

namespace swift::misc::simulation
{
    //! Preferences for distributors
    class SWIFT_MISC_EXPORT CDistributorListPreferences :
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

        //! \copydoc swift::misc::mixin::String::toQString
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

Q_DECLARE_METATYPE(swift::misc::simulation::CDistributorListPreferences)

#endif // guard
