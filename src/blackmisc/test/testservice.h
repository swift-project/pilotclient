// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_TEST_TESTSERVICE_H
#define BLACKMISC_TEST_TESTSERVICE_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/variantlist.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantList>
#include <QDBusConnection>
#include <QDBusObjectPath>

// clazy:excludeall=const-signal-or-slot

class QDBusObjectPath;
class QVariant;

namespace BlackMisc::Aviation
{
    class CCallsign;
    class CComSystem;
    class CTrack;
}

//! DBus interface for test service
#define BLACKMISC_TESTSERVICE_INTERFACENAME "blackmisc.test"

namespace BlackMisc::Test
{
    //! Testservice for PQ / CValueObject DBus tests. This part is the callee.
    //! \remark corresponds with BlackMisc::Test::ITestServiceInterface
    class BLACKMISC_EXPORT CTestService : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKMISC_TESTSERVICE_INTERFACENAME)

        // For some reasons the interface name in the XML is not set correctly
        // to the above name

    public:
        //! Constructor
        explicit CTestService(QObject *parent = nullptr);

        //! Constructor
        explicit CTestService(bool verbose, QObject *parent = nullptr);

        //! Can the testservice be registered?
        static bool canRegisterTestService(QDBusConnection &connection);

        //! Register testservice with connection
        static CTestService *registerTestService(QDBusConnection &connection, bool verbose, QObject *parent = nullptr);

        //! Can the testservice be registered?
        static bool unregisterTestService(QDBusConnection &connection);

        //! Process id
        static qint64 getPid() { return QCoreApplication::applicationPid(); }

        //! DBus interface name
        static const QString &InterfaceName();

        //! DBus object path
        static const QString &ObjectPath();

    signals:
        //! Send string message
        void sendStringMessage(const QString &message);

    public slots:
        //! Receive string message
        void receiveStringMessage(const QString &message) const;

        //! Receive a QVariant
        void receiveVariant(const BlackMisc::CVariant &variant) const;

        //! Receive speed
        void receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed) const;

        //! Receive com unit
        void receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit) const;

        //! Receive altitude
        void receiveAltitude(const BlackMisc::Aviation::CAltitude &altitude) const;

        //! Receive list
        void receiveList(const QList<double> &list) const;

        //! Receive a geo position
        void receiveGeoPosition(const BlackMisc::Geo::CCoordinateGeodetic &geo) const;

        //! Receive transponder
        void receiveTransponder(const BlackMisc::Aviation::CTransponder &transponder) const;

        //! Receive track
        void receiveTrack(const BlackMisc::Aviation::CTrack &track) const;

        //! Receive a length
        void receiveLength(const BlackMisc::PhysicalQuantities::CLength &length) const;

        //! Receive variant list
        void receiveVariantList(const BlackMisc::CVariantList &variantList) const;

        //! Receive ATC station
        void receiveAtcStation(const BlackMisc::Aviation::CAtcStation &station) const;

        //! Receive callsign
        void receiveCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Receive ATC list
        void receiveAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList) const;

        //! Receive an value map
        void receiveValueMap(const BlackMisc::CPropertyIndexVariantMap &valueMap) const;

        //! Ping speed
        BlackMisc::PhysicalQuantities::CSpeed pingSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed) const;

        //! Ping altitude
        BlackMisc::Aviation::CAltitude pingAltitude(const BlackMisc::Aviation::CAltitude &altitude) const;

        //! Ping user
        BlackMisc::Network::CUser pingUser(const BlackMisc::Network::CUser &user) const;

        //! Ping situation
        BlackMisc::Aviation::CAircraftSituation pingSituation(const BlackMisc::Aviation::CAircraftSituation &situation) const;

        //! Ping ATC station
        BlackMisc::Aviation::CAtcStation pingAtcStation(const BlackMisc::Aviation::CAtcStation &station) const;

        //! Ping transponder
        BlackMisc::Aviation::CTransponder pingTransponder(const BlackMisc::Aviation::CTransponder &transponder) const;

        //! Ping aircraft lights
        BlackMisc::Aviation::CAircraftLights pingAircraftLights(const BlackMisc::Aviation::CAircraftLights &aircraftLights) const;

        //! Ping parts
        BlackMisc::Aviation::CAircraftParts pingAircraftParts(const BlackMisc::Aviation::CAircraftParts &aircraftParts) const;

        //! Ping engine
        BlackMisc::Aviation::CAircraftEngine pingAircraftEngine(const BlackMisc::Aviation::CAircraftEngine &aircraftEngine) const;

        //! Ping model
        BlackMisc::Simulation::CAircraftModel pingAircraftModel(const BlackMisc::Simulation::CAircraftModel &aircraftModel) const;

        //! Ping model
        BlackMisc::Simulation::CAircraftModelList pingAircraftModelList(const BlackMisc::Simulation::CAircraftModelList &aircraftModels) const;

        //! Ping simulated aircraft
        BlackMisc::Simulation::CSimulatedAircraft pingSimulatedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) const;

        //! Ping ATC list
        BlackMisc::Aviation::CAtcStationList pingAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList) const;

        //! Ping aircraft list
        BlackMisc::Simulation::CSimulatedAircraftList pingAircraftList(const BlackMisc::Simulation::CSimulatedAircraftList &aircraftList) const;

        //! Ping airports list
        BlackMisc::Aviation::CAirportList pingAirportList(const BlackMisc::Aviation::CAirportList &airportList) const;

        //! Ping property index
        BlackMisc::CPropertyIndex pingPropertyIndex(const BlackMisc::CPropertyIndex &properties) const;

        //! Ping index variant map
        BlackMisc::CPropertyIndexVariantMap pingIndexVariantMap(const BlackMisc::CPropertyIndexVariantMap &indexVariantMap) const;

        //! Ping client
        BlackMisc::Network::CClient pingClient(const BlackMisc::Network::CClient &client) const;

        //! Ping NW clients list
        BlackMisc::Network::CClientList pingClientList(const BlackMisc::Network::CClientList &clientList) const;

        //! Ping plugin info
        BlackMisc::Simulation::CSimulatorPluginInfo pingPluginInfo(const BlackMisc::Simulation::CSimulatorPluginInfo &info) const;

        //! Ping ICAO data object
        BlackMisc::Aviation::CAircraftIcaoCode pingAircraftIcaoData(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const;

        //! Ping flight plan
        BlackMisc::Aviation::CFlightPlan pingFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) const;

        //! Ping CVariant
        BlackMisc::CVariant pingCVariant(const BlackMisc::CVariant &variant) const;

        //! Return stations via DBus
        BlackMisc::Aviation::CAtcStationList getAtcStationList(int n) const;

        //! Return stations via DBus
        BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList getAircraftCfgEntriesList(int n) const;

        //! Return paths via DBus
        QList<QDBusObjectPath> getObjectPaths(int n) const;

    private:
        static QTextStream &out();
        static QTextStream &err();
        bool m_verbose = true;
    };
} // ns

#pragma pop_macro("interface")
#endif // guard
