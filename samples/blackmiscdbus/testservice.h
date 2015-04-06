/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCKTEST_TESTSERVICEPQAV_H
#define BLACKMISCKTEST_TESTSERVICEPQAV_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "blackmisc/aviation/aviation.h"
#include "blackmisc/network/network.h"
#include "blackmisc/pq/pq.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/math/math.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "servicetool.h"
#include <QObject>
#include <QDebug>
#include <QDBusVariant>
#include <QDBusConnection>
#include <QDBusObjectPath>

#define BLACKMISCKTEST_TESTSERVICE_INTERFACENAME "blackmisctest.testservice"
#define BLACKMISCKTEST_TESTSERVICE_SERVICEPATH "/ts"

namespace BlackMiscTest
{

    /*!
     * Testservice for PQ / CValueObject DBus tests
     */
    class Testservice : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKMISCKTEST_TESTSERVICE_INTERFACENAME)

        // For some reasons the interface name in the XML is not set correctly
        // to the above name

    signals:
        //! Send string message
        void sendStringMessage(const QString &message);

    public slots:

        //! Receive string message
        void receiveStringMessage(const QString &message);

        //! Receive a QVariant
        void receiveVariant(const BlackMisc::CVariant &variant);

        //! Receive speed
        void receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed);

        //! Receive com unit
        void receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit);

        //! Receive altitude
        void receiveAltitude(const BlackMisc::Aviation::CAltitude &altitude);

        //! Receive list
        void receiveList(const QList<double> &list);

        //! Receive a geo position
        void receiveGeoPosition(const BlackMisc::Geo::CCoordinateGeodetic &geo) const;

        //! Receive transponder
        void receiveTransponder(const BlackMisc::Aviation::CTransponder &transponder) const;

        //! Receive track
        void receiveTrack(const BlackMisc::Aviation::CTrack &track) const;

        //! Receive a length (not working)
        void receiveLength(const BlackMisc::PhysicalQuantities::CLength &length) const;

        //! Receive lengths
        void receiveLengthsQl(const QList<QVariant> &lengthsList) const;

        //! Receive lengths
        void receiveLengthsQvl(const QVariantList &lengthsVariantList) const;

        //! Receive ATC station
        void receiveAtcStation(const BlackMisc::Aviation::CAtcStation &station) const;

        //! Receive callsign
        void receiveCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Receive ATC list
        void receiveAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList) const;

        //! Receive an value map
        void receiveValueMap(const BlackMisc::CPropertyIndexVariantMap &valueMap) const;

        //! Receive speed
        BlackMisc::PhysicalQuantities::CSpeed pingSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed);

        //! Receive altitude
        BlackMisc::Aviation::CAltitude pingAltitude(const BlackMisc::Aviation::CAltitude &altitude);

        //! Ping ATC station
        BlackMisc::Aviation::CAtcStation pingAtcStation(const BlackMisc::Aviation::CAtcStation &station);

        //! Ping aircraft
        BlackMisc::Aviation::CAircraft pingAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        //! Ping simulated aircraft
        BlackMisc::Simulation::CSimulatedAircraft pingSimulatedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Ping ATC list
        BlackMisc::Aviation::CAtcStationList pingAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList) const;

        //! Ping aircrafts list
        BlackMisc::Aviation::CAircraftList pingAircraftList(const BlackMisc::Aviation::CAircraftList &aircraftList);

        //! Ping airports list
        BlackMisc::Aviation::CAirportList pingAirportList(const BlackMisc::Aviation::CAirportList &airportList);

        //! Ping property index
        BlackMisc::CPropertyIndex pingPropertyIndex(const BlackMisc::CPropertyIndex &properties);

        //! Ping index variant map
        BlackMisc::CPropertyIndexVariantMap pingIndexVariantMap(const BlackMisc::CPropertyIndexVariantMap &indexVariantMap);

        //! Ping client
        BlackMisc::Network::CClient pingClient(const BlackMisc::Network::CClient &client);

        //! Ping NW clients list
        BlackMisc::Network::CClientList pingClientList(const BlackMisc::Network::CClientList &clientList);

        //! Ping CVariant
        BlackMisc::CVariant pingCVariant(const BlackMisc::Network::CClient &client);

        //! Get speed
        BlackMisc::PhysicalQuantities::CSpeed getSpeed() const;

        //! Get station
        BlackMisc::Aviation::CAtcStation getAtcStation() const;

        //! return n ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationList(int number) const;

        //! return n aircraft cfg entries
        BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList getAircraftCfgEntriesList(int number) const;

        //! Get object paths
        QList<QDBusObjectPath> getObjectPaths(int number) const;

    public:
        static const QString ServiceName;
        static const QString ServicePath;

        //! Constructor
        explicit Testservice(QObject *parent = nullptr);

    private:
        BlackMisc::Aviation::CAtcStationList m_someAtcStations;

    };

} // namespace

#pragma pop_macro("interface")

#endif // guard
