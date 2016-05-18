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

#ifndef BLACKSAMPLE_TESTSERVICE_H
#define BLACKSAMPLE_TESTSERVICE_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/variant.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QVariantList>

class QDBusObjectPath;
class QVariant;

namespace BlackMisc
{
    namespace Aviation
    {
        class CCallsign;
        class CComSystem;
        class CTrack;
    }
}

//! DBus interface for test service
#define BLACKSAMPLE_TESTSERVICE_INTERFACENAME "blackmisctest.testservice"

//! DBus object path for test service
#define BLACKSAMPLE_TESTSERVICE_OBJECTPATH "/ts"

namespace BlackSample
{

    //! Testservice for PQ / CValueObject DBus tests
    class Testservice : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKSAMPLE_TESTSERVICE_INTERFACENAME)

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

        //! Ping speed
        BlackMisc::PhysicalQuantities::CSpeed pingSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed);

        //! Ping altitude
        BlackMisc::Aviation::CAltitude pingAltitude(const BlackMisc::Aviation::CAltitude &altitude);

        //! Ping user
        BlackMisc::Network::CUser pingUser(const BlackMisc::Network::CUser &user);

        //! Ping situation
        BlackMisc::Aviation::CAircraftSituation pingSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Ping ATC station
        BlackMisc::Aviation::CAtcStation pingAtcStation(const BlackMisc::Aviation::CAtcStation &station);

        //! Ping transponder
        BlackMisc::Aviation::CTransponder pingTransponder(const BlackMisc::Aviation::CTransponder &transponder);

        //! Ping aircraft
        BlackMisc::Simulation::CSimulatedAircraft pingAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Ping simulated aircraft
        BlackMisc::Simulation::CSimulatedAircraft pingSimulatedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Ping ATC list
        BlackMisc::Aviation::CAtcStationList pingAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList) const;

        //! Ping aircraft list
        BlackMisc::Simulation::CSimulatedAircraftList pingAircraftList(const BlackMisc::Simulation::CSimulatedAircraftList &aircraftList);

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

        //! Ping plugin info
        BlackMisc::Simulation::CSimulatorPluginInfo pingPluginInfo(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        //! Ping ICAO data object
        BlackMisc::Aviation::CAircraftIcaoCode pingAircraftIcaoData(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

        //! Ping CVariant
        BlackMisc::CVariant pingCVariant(const BlackMisc::Network::CClient &client);

        //! Get speed
        BlackMisc::PhysicalQuantities::CSpeed getSpeed() const;

        //! Get station
        BlackMisc::Aviation::CAtcStation getAtcStation() const;

        //! Return n ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationList(int number) const;

        //! Return n aircraft cfg entries
        BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList getAircraftCfgEntriesList(int number) const;

        //! Get object paths
        QList<QDBusObjectPath> getObjectPaths(int number) const;

    public:
        //! DBus interface name
        static const QString InterfaceName;
        //! DBus object path
        static const QString ObjectPath;

        //! Constructor
        explicit Testservice(QObject *parent = nullptr);

    private:
        BlackMisc::Aviation::CAtcStationList m_someAtcStations;

    };

} // namespace

#pragma pop_macro("interface")

#endif // guard
