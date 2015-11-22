/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCTEST_SERVICETOOL_H
#define BLACKMISCTEST_SERVICETOOL_H

#include "blackcore/dbusserver.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>

namespace BlackMiscTest
{
    class Testservice; // forward declaration

    /*!
     * \brief Supporting / helper functions for running the tests
     */
    class ServiceTool
    {
    private:
        ServiceTool() {}

    public:
        /*!
         * \brief Get process id
         * \return
         */
        static qint64 getPid()
        {
            return QCoreApplication::applicationPid();
        }

        /*!
         * \brief Client side of data transfer test
         * \param address
         */
        static void dataTransferTestClient(const QString &address);

        /*!
         * \brief Server side of data transfer test
         */
        static void dataTransferTestServer(BlackCore::CDBusServer *dBusServer);

        /*!
         * \brief Start a new process
         * \param executable
         * \param arguments
         * \param parent
         * \return
         */
        static QProcess *startNewProcess(const QString &executable, const QStringList &arguments = QStringList(), QObject *parent = 0);

        /*!
         * \brief Loop to send data to test service (slots on server)
         * \param connection
         */
        static void sendDataToTestservice(const QDBusConnection &connection);

        /*!
         * \brief Display QDBusArgument
         * \param arg
         */
        static void displayQDBusArgument(const QDBusArgument &arg, qint32 level = 0);

        /*!
         * \brief Register testservice with connection
         * \param connection
         * \param parent
         * \return test service object
         */
        static Testservice *registerTestservice(QDBusConnection &connection, QObject *parent = 0);

        /*!
         * \brief Get a random callsign
         * \return
         */
        static BlackMisc::Aviation::CCallsign getRandomAtcCallsign();

        /*!
         * \brief Get stations
         * \param number
         * \return
         */
        static BlackMisc::Aviation::CAtcStationList getStations(int number);

        /*!
         * \brief Get aircraft cfg entries
         * \param number
         * \return
         */
        static BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList getAircraftCfgEntries(int number);

        /*!
         * \brief Get airports
         * \param number
         * \return
         */
        static BlackMisc::Aviation::CAirportList getAirports(int number);

        /*!
         * \brief Get clients
         * \param number
         * \return
         */
        static BlackMisc::Network::CClientList getClients(int number);

    };

} // namespace

#endif // guard
