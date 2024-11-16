// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_TEST_TESTING_H
#define SWIFT_MISC_TEST_TESTING_H

#include <QString>

#include "misc/aviation/airportlist.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/network/clientlist.h"
#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "misc/swiftmiscexport.h"

//! Generate data for testing purposes.
namespace swift::misc::test
{
    //! Generate data for testing aviation classes
    class SWIFT_MISC_EXPORT CTesting
    {
    public:
        //! Generate number of ATC stations
        static swift::misc::aviation::CAtcStationList createAtcStations(int number, bool byPropertyIndex = false);

        //! Single station, annotated by index
        static swift::misc::aviation::CAtcStation createStation(int index, bool byPropertyIndex = false);

        //! Get aircraft cfg entries
        static swift::misc::simulation::fscommon::CAircraftCfgEntriesList getAircraftCfgEntries(int number);

        //! Get airports
        static swift::misc::aviation::CAirportList getAirports(int number);

        //! Get clients
        static swift::misc::network::CClientList getClients(int number);
    };
} // namespace swift::misc::test

#endif
