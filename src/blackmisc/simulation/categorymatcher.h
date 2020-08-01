/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_CATEGORYMATCHER_H
#define BLACKMISC_SIMULATION_CATEGORYMATCHER_H

#include "aircraftmodellist.h"
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
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Constructor
            CCategoryMatcher() {}

            //! Used categories
            void setCategories(const Aviation::CAircraftCategoryList &categories);

            //! Specialized categories
            //! @{
            const Aviation::CAircraftCategoryList &gliderCategories() const { return m_gliders; }
            const Aviation::CAircraftCategoryList &militaryWingCategories()  const { return m_militaryWingAircraft; }
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
