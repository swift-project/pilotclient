// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_CATEGORYMATCHER_H
#define BLACKMISC_SIMULATION_CATEGORYMATCHER_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    class CStatusMessageList;

    namespace Simulation
    {
        class CSimulatedAircraft;
        class CAircraftMatcherSetup;

        //! Category matcher, uses the DB categories
        class BLACKMISC_EXPORT CCategoryMatcher
        {
        public:
            //! Log categories
            static const QStringList &getLogCategories();

            //! Constructor
            CCategoryMatcher() {}

            //! Used categories
            void setCategories(const Aviation::CAircraftCategoryList &categories);

            //! @{
            //! Specialized categories
            const Aviation::CAircraftCategoryList &gliderCategories() const { return m_gliders; }
            const Aviation::CAircraftCategoryList &militaryWingCategories() const { return m_militaryWingAircraft; }
            const Aviation::CAircraftCategoryList &militaryRotorCategories() const { return m_militaryRotorAircraft; }
            //! @}

            //! Reduce by categories
            CAircraftModelList reduceByCategories(
                const CAircraftModelList &alreadyMatchedModels, const CAircraftModelList &modelSet,
                const CAircraftMatcherSetup &setup, const CSimulatedAircraft &remoteAircraft, bool &reduced, bool shortLog, CStatusMessageList *log = nullptr) const;

        private:
            //! Glider?
            bool isGlider(const Aviation::CAircraftIcaoCode &icao) const;

            //! Get the glider top level
            int gliderFirstLevel() const;

            Aviation::CAircraftCategoryList m_all; //!< sorted by level
            Aviation::CAircraftCategoryList m_gliders; //!< sorted by level
            Aviation::CAircraftCategoryList m_militaryWingAircraft;
            Aviation::CAircraftCategoryList m_militaryRotorAircraft;
        };
    } // ns
} // namespace

#endif // guard
