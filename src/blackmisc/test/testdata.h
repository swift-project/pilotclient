/* Copyright (C) 2017
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TEST_TESTDATA_H
#define BLACKMISC_TEST_TESTDATA_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    namespace Aviation
    {
        class CAtcStation;
        class CAtcStationList;
        class CCallsign;
    }
    namespace Geo
    {
        class CCoordinateGeodetic;
    }

    namespace Test
    {
        /*!
         * Testdata for unit tests/data
         */
        class BLACKMISC_EXPORT CTestData
        {
        public:
            //! Munich tower coordinate
            static const BlackMisc::Geo::CCoordinateGeodetic &coordinateMunichTower();

            //! Frankfurt tower coordinate
            static const BlackMisc::Geo::CCoordinateGeodetic &coordinateFrankfurtTower();

            //! Tower stations
            static const BlackMisc::Aviation::CAtcStationList &getTowerStations();

            //! Get ATC station
            static const BlackMisc::Aviation::CAtcStation &getAtcStation();

            //! Get a random callsign (ATC)
            static const BlackMisc::Aviation::CCallsign &getRandomAtcCallsign();

            //! Get a random callsign (aircraft)
            static const BlackMisc::Aviation::CCallsign &getRandomAircraftCallsign();
        };
    } // ns
} // ns

#endif // guard
