// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_TEST_TESTSERVICE_H
#define SWIFT_MISC_TEST_TESTSERVICE_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QList>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantList>

#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/flightplan.h"
#include "misc/aviation/transponder.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/network/client.h"
#include "misc/network/clientlist.h"
#include "misc/network/user.h"
#include "misc/pq/length.h"
#include "misc/pq/speed.h"
#include "misc/propertyindexref.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"
#include "misc/variantlist.h"

// clazy:excludeall=const-signal-or-slot

class QDBusObjectPath;
class QVariant;

namespace swift::misc::aviation
{
    class CCallsign;
    class CComSystem;
    class CTrack;
} // namespace swift::misc::aviation

//! DBus interface for test service
#define SWIFT_MISC_TESTSERVICE_INTERFACENAME "misc.test"

namespace swift::misc::test
{
    //! Testservice for PQ / CValueObject DBus tests. This part is the callee.
    //! \remark corresponds with swift::misc::Test::ITestServiceInterface
    class SWIFT_MISC_EXPORT CTestService : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", SWIFT_MISC_TESTSERVICE_INTERFACENAME)

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
        void receiveVariant(const swift::misc::CVariant &variant) const;

        //! Receive speed
        void receiveSpeed(const swift::misc::physical_quantities::CSpeed &speed) const;

        //! Receive com unit
        void receiveComUnit(const swift::misc::aviation::CComSystem &comUnit) const;

        //! Receive altitude
        void receiveAltitude(const swift::misc::aviation::CAltitude &altitude) const;

        //! Receive list
        void receiveList(const QList<double> &list) const;

        //! Receive a geo position
        void receiveGeoPosition(const swift::misc::geo::CCoordinateGeodetic &geo) const;

        //! Receive transponder
        void receiveTransponder(const swift::misc::aviation::CTransponder &transponder) const;

        //! Receive track
        void receiveTrack(const swift::misc::aviation::CTrack &track) const;

        //! Receive a length
        void receiveLength(const swift::misc::physical_quantities::CLength &length) const;

        //! Receive variant list
        void receiveVariantList(const swift::misc::CVariantList &variantList) const;

        //! Receive ATC station
        void receiveAtcStation(const swift::misc::aviation::CAtcStation &station) const;

        //! Receive callsign
        void receiveCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Receive ATC list
        void receiveAtcStationList(const swift::misc::aviation::CAtcStationList &atcStationList) const;

        //! Receive an value map
        void receiveValueMap(const swift::misc::CPropertyIndexVariantMap &valueMap) const;

        //! Ping speed
        swift::misc::physical_quantities::CSpeed pingSpeed(const swift::misc::physical_quantities::CSpeed &speed) const;

        //! Ping altitude
        swift::misc::aviation::CAltitude pingAltitude(const swift::misc::aviation::CAltitude &altitude) const;

        //! Ping user
        swift::misc::network::CUser pingUser(const swift::misc::network::CUser &user) const;

        //! Ping situation
        swift::misc::aviation::CAircraftSituation
        pingSituation(const swift::misc::aviation::CAircraftSituation &situation) const;

        //! Ping ATC station
        swift::misc::aviation::CAtcStation pingAtcStation(const swift::misc::aviation::CAtcStation &station) const;

        //! Ping transponder
        swift::misc::aviation::CTransponder
        pingTransponder(const swift::misc::aviation::CTransponder &transponder) const;

        //! Ping aircraft lights
        swift::misc::aviation::CAircraftLights
        pingAircraftLights(const swift::misc::aviation::CAircraftLights &aircraftLights) const;

        //! Ping parts
        swift::misc::aviation::CAircraftParts
        pingAircraftParts(const swift::misc::aviation::CAircraftParts &aircraftParts) const;

        //! Ping engine
        swift::misc::aviation::CAircraftEngine
        pingAircraftEngine(const swift::misc::aviation::CAircraftEngine &aircraftEngine) const;

        //! Ping model
        swift::misc::simulation::CAircraftModel
        pingAircraftModel(const swift::misc::simulation::CAircraftModel &aircraftModel) const;

        //! Ping model
        swift::misc::simulation::CAircraftModelList
        pingAircraftModelList(const swift::misc::simulation::CAircraftModelList &aircraftModels) const;

        //! Ping simulated aircraft
        swift::misc::simulation::CSimulatedAircraft
        pingSimulatedAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft) const;

        //! Ping ATC list
        swift::misc::aviation::CAtcStationList
        pingAtcStationList(const swift::misc::aviation::CAtcStationList &atcStationList) const;

        //! Ping aircraft list
        swift::misc::simulation::CSimulatedAircraftList
        pingAircraftList(const swift::misc::simulation::CSimulatedAircraftList &aircraftList) const;

        //! Ping airports list
        swift::misc::aviation::CAirportList
        pingAirportList(const swift::misc::aviation::CAirportList &airportList) const;

        //! Ping property index
        swift::misc::CPropertyIndex pingPropertyIndex(const swift::misc::CPropertyIndex &properties) const;

        //! Ping index variant map
        swift::misc::CPropertyIndexVariantMap
        pingIndexVariantMap(const swift::misc::CPropertyIndexVariantMap &indexVariantMap) const;

        //! Ping client
        swift::misc::network::CClient pingClient(const swift::misc::network::CClient &client) const;

        //! Ping NW clients list
        swift::misc::network::CClientList pingClientList(const swift::misc::network::CClientList &clientList) const;

        //! Ping plugin info
        swift::misc::simulation::CSimulatorPluginInfo
        pingPluginInfo(const swift::misc::simulation::CSimulatorPluginInfo &info) const;

        //! Ping ICAO data object
        swift::misc::aviation::CAircraftIcaoCode
        pingAircraftIcaoData(const swift::misc::aviation::CAircraftIcaoCode &icao) const;

        //! Ping flight plan
        swift::misc::aviation::CFlightPlan pingFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) const;

        //! Ping CVariant
        swift::misc::CVariant pingCVariant(const swift::misc::CVariant &variant) const;

        //! Return stations via DBus
        swift::misc::aviation::CAtcStationList getAtcStationList(int n) const;

        //! Return stations via DBus
        swift::misc::simulation::fscommon::CAircraftCfgEntriesList getAircraftCfgEntriesList(int n) const;

        //! Return paths via DBus
        QList<QDBusObjectPath> getObjectPaths(int n) const;

    private:
        static QTextStream &out();
        static QTextStream &err();
        bool m_verbose = true;
    };
} // namespace swift::misc::test

#pragma pop_macro("interface")

#endif // SWIFT_MISC_TEST_TESTSERVICE_H
