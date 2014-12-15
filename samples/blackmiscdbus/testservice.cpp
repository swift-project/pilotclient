/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testservice.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackSim::FsCommon;

namespace BlackMiscTest
{

    const QString Testservice::ServiceName = QString(BLACKMISCKTEST_TESTSERVICE_INTERFACENAME);
    const QString Testservice::ServicePath = QString(BLACKMISCKTEST_TESTSERVICE_SERVICEPATH);

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
    BlackMisc::Aviation::CAtcStation Testservice::getAtcStation() const
    {
        CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CLength(1487, CLengthUnit::ft())); // Munich
        CAtcStation station(CCallsign("eddm_twr"), CUser("654321", "client"),
                            CFrequency(118.7, CFrequencyUnit::MHz()),
                            geoPos, CLength(50, CLengthUnit::km()));
        return station;
    }

    /*
     * Receive COM unit
     */
    void Testservice::receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received COM:" << comUnit;
    }

    /*
     * Receive altitude
     */
    void Testservice::receiveAltitude(const BlackMisc::Aviation::CAltitude &altitude)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received altitude:" << altitude;
    }

    /*
     * Receive matrix
     */
    void Testservice::receiveMatrix(const BlackMisc::Math::CMatrix3x3 &matrix)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received matrix:" << matrix;
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
    void Testservice::receiveTransponder(const BlackMisc::Aviation::CTransponder &transponder) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received transponder:" << transponder;
    }

    /*
     * Receive track
     */
    void Testservice::receiveTrack(const BlackMisc::Aviation::CTrack &track) const
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
        foreach(QVariant lv, lengthsVariantList)
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
        foreach(QVariant lv, lengthsList)
        {
            BlackMisc::PhysicalQuantities::CLength l;
            lv.value<QDBusArgument>() >> l;
            qDebug() << "    Received length in list:" << l;
        }
    }

    /*
     * Receive callsign
     */
    void Testservice::receiveCallsign(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received callsign:" << callsign;
    }

    /*
     * Receive ATC list
     */
    void Testservice::receiveAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList) const
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
    void Testservice::receiveAtcStation(const BlackMisc::Aviation::CAtcStation &station) const
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
            paths.append(QDBusObjectPath(BLACKMISCKTEST_TESTSERVICE_SERVICEPATH));
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
    BlackMisc::Aviation::CAtcStationList Testservice::pingAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping ATCs:" << atcStationList;
        return atcStationList;
    }

    /*
     * Ping aircrafts
     */
    CAircraftList Testservice::pingAircraftList(const CAircraftList &aircraftList)
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
    BlackMisc::Aviation::CAltitude Testservice::pingAltitude(const BlackMisc::Aviation::CAltitude &altitude)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping alt:" << altitude;
        return altitude;
    }

    /*
     * Ping ATC station
     */
    BlackMisc::Aviation::CAtcStation Testservice::pingAtcStation(const BlackMisc::Aviation::CAtcStation &station)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping ATC:" << station;
        return station;
    }

    /*
     * Ping aircraft
     */
    BlackMisc::Aviation::CAircraft Testservice::pingAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "ping aircraft:" << aircraft;
        return aircraft;
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
