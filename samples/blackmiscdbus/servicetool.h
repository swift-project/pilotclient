/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscdbus

#ifndef BLACKSAMPLE_SERVICETOOL_H
#define BLACKSAMPLE_SERVICETOOL_H

#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"

#include <QCoreApplication>
#include <QDBusArgument>
#include <QString>
#include <QStringList>
#include <QtGlobal>

class QDBusConnection;
class QObject;
class QProcess;

namespace BlackMisc { class CDBusServer; }
namespace BlackSample
{
    class Testservice; // forward declaration

    /*!
     * Supporting / helper functions for running the tests
     */
    class ServiceTool
    {
    private:
        ServiceTool() {}

    public:
        //! Process id
        static qint64 getPid()
        {
            return QCoreApplication::applicationPid();
        }

        //! Client side of data transfer test
        static void dataTransferTestClient(const QString &address);

        //! Server side of data transfer test
        static void dataTransferTestServer(BlackMisc::CDBusServer *dBusServer);

        //! Start a new process
        static QProcess *startNewProcess(const QString &executable, const QStringList &arguments = QStringList(), QObject *parent = 0);

        //! Loop to send data to test service (slots on server)
        static void sendDataToTestservice(const QDBusConnection &connection);

        //! Display QDBusArgument
        static void displayQDBusArgument(const QDBusArgument &arg, qint32 level = 0);

        //! Register testservice with connection
        static Testservice *registerTestservice(QDBusConnection &connection, QObject *parent = 0);

        //! Get a random callsign
        static BlackMisc::Aviation::CCallsign getRandomAtcCallsign();

        //! Get stations
        static BlackMisc::Aviation::CAtcStationList getStations(int number);

        //! Get aircraft cfg entries
        static BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList getAircraftCfgEntries(int number);

        //! Get airports
        static BlackMisc::Aviation::CAirportList getAirports(int number);

        //! Get clients
        static BlackMisc::Network::CClientList getClients(int number);
    };
} // namespace

#endif // guard
