/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TEST_TESTING_H
#define BLACKMISC_TEST_TESTING_H

#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/blackmiscexport.h"
#include <QString>

//! Generate data for testing purposes.
namespace BlackMisc::Test
{
    //! Generate data for testing aviation classes
    class BLACKMISC_EXPORT CTesting
    {
    public:
        //! Generate number of ATC stations
        static BlackMisc::Aviation::CAtcStationList createAtcStations(int number, bool byPropertyIndex = false);

        //! Single station, annotated by index
        static BlackMisc::Aviation::CAtcStation createStation(int index, bool byPropertyIndex = false);

        //! Get aircraft cfg entries
        static BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList getAircraftCfgEntries(int number);

        //! Get airports
        static BlackMisc::Aviation::CAirportList getAirports(int number);

        //! Get clients
        static BlackMisc::Network::CClientList getClients(int number);
    };
} // ns

#endif // guard
