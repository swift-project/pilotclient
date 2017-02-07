/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_TEST_TESTUTILS_H
#define BLACKCORE_TEST_TESTUTILS_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/network/clientlist.h"
#include <QDBusArgument>

namespace BlackCore
{
    namespace Test
    {
        //! Utils for UNIT tests / samples
        class BLACKCORE_EXPORT CTestUtils
        {
        public:
            //! Get QDBusArgument signature (formatted)
            static QString getQDBusArgumentSignature(const QDBusArgument &arg, int level = 0);

            //! Signature for BlackMisc::CValueObject
            template<typename ValueObj>
            static QString dBusSignature(const ValueObj &obj)
            {
                QDBusArgument arg;
                obj.marshallToDbus(arg);
                return arg.currentSignature();
            }

            //! Type as string
            static QString dbusTypeAsString(QDBusArgument::ElementType type);

            //! Show some (important) DBus signatures
            static void showDBusSignatures(QTextStream &out);

            // ----------------- some test data --------------------

            //! Get a random callsign
            static BlackMisc::Aviation::CCallsign getRandomAtcCallsign();

            //! Get ATC station
            static BlackMisc::Aviation::CAtcStation getAtcStation();

            //! Get speed
            static BlackMisc::PhysicalQuantities::CSpeed getSpeed();

            //! Get ATC stations
            static BlackMisc::Aviation::CAtcStationList getStations(int number);

            //! Get aircraft cfg entries
            static BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList getAircraftCfgEntries(int number);

            //! Get airports
            static BlackMisc::Aviation::CAirportList getAirports(int number);

            //! Get clients
            static BlackMisc::Network::CClientList getClients(int number);
        };
    } // ns
} // ns

#endif // guard
