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

#include "testservice.h"
#include "servicetool.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/track.h"
#include "blackmisc/dbus.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/stringutils.h"

#include <QDBusArgument>
#include <QDBusObjectPath>
#include <QDebug>
#include <QVariant>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

namespace BlackSample
{

    const QString Testservice::InterfaceName = QString(BLACKSAMPLE_TESTSERVICE_INTERFACENAME);
    const QString Testservice::ObjectPath = QString(BLACKSAMPLE_TESTSERVICE_OBJECTPATH);

    /*
     * Constructor
     */
    Testservice::Testservice(QObject *parent) : QObject(parent)
    {
        // void
    }

    /*
     * Slot to receive messages
     */
    void Testservice::receiveStringMessage(const QString &message)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received message:" << message;
    }

    /*
     * Receive variant
     */
    void Testservice::receiveVariant(const CVariant &variant)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received variant:" << variant;
    }

    /*
     * Receive speed
     */
    void Testservice::receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received speed:" << speed;
    }

    /*
     * Ping speed
     */
    BlackMisc::PhysicalQuantities::CSpeed Testservice::pingSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed)
    {
        return speed;
    }

    /*
     * Get speed
     */
    BlackMisc::PhysicalQuantities::CSpeed Testservice::getSpeed() const
    {
        return BlackMisc::PhysicalQuantities::CSpeed(666, BlackMisc::PhysicalQuantities::CSpeedUnit::km_h());
    }

    /*
     * Station
     */
    CAtcStation Testservice::getAtcStation() const
    {
        CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() }); // Munich
        CAtcStation station(CCallsign("eddm_twr"), CUser("654321", "client"),
                            CFrequency(118.7, CFrequencyUnit::MHz()),
                            geoPos, CLength(50, CLengthUnit::km()));
        return station;
    }

    /*
     * Receive COM unit
     */
    void Testservice::receiveComUnit(const CComSystem &comUnit)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received COM:" << comUnit;
    }

    /*
     * Receive altitude
     */
    void Testservice::receiveAltitude(const CAltitude &altitude)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received altitude:" << altitude;
    }

    /*
     * Receive a list
     */
    void Testservice::receiveList(const QList<double> &list)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received list:" << list;
    }

    /*
     * Receive a geo position
     */
    void Testservice::receiveGeoPosition(const BlackMisc::Geo::CCoordinateGeodetic &geo) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received geo data:" << geo;
    }

    /*
     * Receive transponder
     */
    void Testservice::receiveTransponder(const CTransponder &transponder) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received transponder:" << transponder;
    }

    /*
     * Receive track
     */
    void Testservice::receiveTrack(const CTrack &track) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received track:" << track;
    }

    /*
     * Receive a length
     */
    void Testservice::receiveLength(const BlackMisc::PhysicalQuantities::CLength &length) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received length:" << length;
    }

    /*
     * Receive lengths
     */
    void Testservice::receiveLengthsQvl(const QVariantList &lengthsVariantList) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << lengthsVariantList;
        foreach (QVariant lv, lengthsVariantList)
        {
            BlackMisc::PhysicalQuantities::CLength l;
            lv.value<QDBusArgument>() >> l;
            qDebug() << "    Received length in list:" << l;
        }
    }

    /*
     * Receive lengths
     */
    void Testservice::receiveLengthsQl(const QList<QVariant> &lengthsList) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received " << lengthsList;
        foreach (QVariant lv, lengthsList)
        {
            BlackMisc::PhysicalQuantities::CLength l;
            lv.value<QDBusArgument>() >> l;
            qDebug() << "    Received length in list:" << l;
        }
    }

    /*
     * Receive callsign
     */
    void Testservice::receiveCallsign(const CCallsign &callsign) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received callsign:" << callsign;
    }

    /*
     * Receive ATC list
     */
    void Testservice::receiveAtcStationList(const CAtcStationList &atcStationList) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received ATC list:" << atcStationList;
    }

    /*
     * Receive value map
     */
    void Testservice::receiveValueMap(const BlackMisc::CPropertyIndexVariantMap &valueMap) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received value map:" << valueMap;
    }

    /*
     * Receive callsign
     */
    void Testservice::receiveAtcStation(const CAtcStation &station) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received ATC station:" << station;
    }

    /*
     * Object paths
     */
    QList<QDBusObjectPath> Testservice::getObjectPaths(int number) const
    {
        QList<QDBusObjectPath> paths;
        for (int i = 0; i < number; i++)
        {
            paths.append(QDBusObjectPath(BLACKSAMPLE_TESTSERVICE_OBJECTPATH));
        }
        return paths;
    }

    /*
     * Get ATC station list
     */
    CAtcStationList Testservice::getAtcStationList(int number) const
    {
        return CAtcStationList(ServiceTool::getStations(number));
    }

    /*
     * Get aircraft entries
     */
    CAircraftCfgEntriesList Testservice::getAircraftCfgEntriesList(int number) const
    {
        return CAircraftCfgEntriesList(ServiceTool::getAircraftCfgEntries(number));
    }

    /*
     * Ping ATC list
     */
    CAtcStationList Testservice::pingAtcStationList(const CAtcStationList &atcStationList) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping ATCs:" << atcStationList;
        return atcStationList;
    }

    /*
     * Ping aircraft
     */
    CSimulatedAircraftList Testservice::pingAircraftList(const CSimulatedAircraftList &aircraftList)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping aircrafts:" << aircraftList;
        return aircraftList;
    }

    /*
     * Ping airports
     */
    CAirportList Testservice::pingAirportList(const CAirportList &airportList)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping airports:" << airportList;
        return airportList;
    }

    /*
     * Ping property index
     */
    CPropertyIndex Testservice::pingPropertyIndex(const CPropertyIndex &properties)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping properties:" << properties;
        return properties;
    }

    /*
     * Ping index variant map
     */
    CPropertyIndexVariantMap Testservice::pingIndexVariantMap(const CPropertyIndexVariantMap &indexVariantMap)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping properties:" << indexVariantMap;
        return indexVariantMap;
    }

    /*
     * NW client
     */
    CClient Testservice::pingClient(const CClient &client)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping client:" << client;
        return client;
    }

    /*
     * NW clients
     */
    CClientList Testservice::pingClientList(const CClientList &clientList)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping clients:" << clientList;
        return clientList;
    }

    /*
     * Ping altitude
     */
    CAltitude Testservice::pingAltitude(const CAltitude &altitude)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping alt:" << altitude;
        return altitude;
    }

    /*
     * Ping user
     */
    CUser Testservice::pingUser(const CUser &user)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping user:" << user;
        return user;
    }

    /*
     * Ping situation
     */
    CAircraftSituation Testservice::pingSituation(const CAircraftSituation &situation)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping situation:" << situation;
        return situation;
    }

    /*
     * Ping transponder
     */
    CTransponder Testservice::pingTransponder(const CTransponder &transponder)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping transponder:" << transponder;
        return transponder;
    }

    /*
     * Ping ATC station
     */
    CAtcStation Testservice::pingAtcStation(const CAtcStation &station)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping ATC:" << station;
        return station;
    }

    /*
     * Ping aircraft
     */
    CSimulatedAircraft Testservice::pingAircraft(const CSimulatedAircraft &aircraft)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping aircraft:" << aircraft;
        return aircraft;
    }

    /*
     * Ping simulated aircraft
     */
    CSimulatedAircraft Testservice::pingSimulatedAircraft(const CSimulatedAircraft &aircraft)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping simulated aircraft:" << aircraft;
        return aircraft;
    }

    /*
     * Ping ICAO data
     */
    CAircraftIcaoCode Testservice::pingAircraftIcaoData(const CAircraftIcaoCode &icao)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping ICAO data:" << icao;
        return icao;
    }

    /*
     * Ping info
     */
    CSimulatorPluginInfo Testservice::pingPluginInfo(const CSimulatorPluginInfo &info)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "info:" << info;
        return info;
    }

    /*
     * Ping variant
     */
    BlackMisc::CVariant Testservice::pingCVariant(const BlackMisc::Network::CClient &client)
    {
        // we receive as QVariant, but return as CVariant!
        qDebug() << "Pid:" << ServiceTool::getPid() << "client sent back as CVariant:" << client.toQString();
        return BlackMisc::CVariant::fromValue(client);
    }

} // namespace
