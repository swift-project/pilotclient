/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TEST_TESTING_H
#define BLACKMISC_TEST_TESTING_H

#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/blackmiscexport.h"
#include <QString>

//! Generate data for testing purposes.
namespace BlackMisc
{
    namespace Test
    {
        //! Generate data for testing aviation classes
        class BLACKMISC_EXPORT CTesting
        {
        public:
            //! Generate number of ATC stations
            static BlackMisc::Aviation::CAtcStationList createAtcStations(int number, bool byPropertyIndex = false);

            //! Single station, annotated by index
            static BlackMisc::Aviation::CAtcStation createStation(int index, bool byPropertyIndex = false);

            //! Generate number of ATC stations
            static void readStations(const BlackMisc::Aviation::CAtcStationList &stations, bool byPropertyIndex = false);

            //! Read properties of a station and concatenate them
            static QString readStation(const BlackMisc::Aviation::CAtcStation &station, bool byPropertyIndex = false);

            //! Calculate n times distance (greater circle distance)
            static void calculateDistance(int n);

            //! Copy 10k stations n times
            static void copy10kStations(int times);

            //! Const 10000 stations
            static const BlackMisc::Aviation::CAtcStationList &stations10k();

            //! parse coordinates from WGS
            static void parseWgs(int times);
        };
    } // ns
} // ns

#endif // guard
