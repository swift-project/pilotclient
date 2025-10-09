// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_CATEGORYMATCHER_H
#define SWIFT_MISC_SIMULATION_CATEGORYMATCHER_H

#include "misc/aviation/aircraftcategorylist.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    class CStatusMessageList;

    namespace simulation
    {
        class CSimulatedAircraft;
        class CAircraftMatcherSetup;

        //! Category matcher, uses the DB categories
        class SWIFT_MISC_EXPORT CCategoryMatcher
        {
        public:
            //! Log categories
            static const QStringList &getLogCategories();

            //! Constructor
            CCategoryMatcher() = default;

            //! Used categories
            void setCategories(const aviation::CAircraftCategoryList &categories);

            //! @{
            //! Specialized categories
            const aviation::CAircraftCategoryList &gliderCategories() const { return m_gliders; }
            const aviation::CAircraftCategoryList &militaryWingCategories() const { return m_militaryWingAircraft; }
            const aviation::CAircraftCategoryList &militaryRotorCategories() const { return m_militaryRotorAircraft; }
            //! @}

            //! Reduce by categories
            CAircraftModelList reduceByCategories(const CAircraftModelList &alreadyMatchedModels,
                                                  const CAircraftModelList &modelSet,
                                                  const CAircraftMatcherSetup &setup,
                                                  const CSimulatedAircraft &remoteAircraft, bool &reduced,
                                                  bool shortLog, CStatusMessageList *log = nullptr) const;

        private:
            //! Glider?
            bool isGlider(const aviation::CAircraftIcaoCode &icao) const;

            //! Get the glider top level
            int gliderFirstLevel() const;

            aviation::CAircraftCategoryList m_all; //!< sorted by level
            aviation::CAircraftCategoryList m_gliders; //!< sorted by level
            aviation::CAircraftCategoryList m_militaryWingAircraft;
            aviation::CAircraftCategoryList m_militaryRotorAircraft;
        };
    } // namespace simulation
} // namespace swift::misc

#endif // SWIFT_MISC_SIMULATION_CATEGORYMATCHER_H
