/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testservice.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

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
    void Testservice::receiveVariant(const QDBusVariant &variant, int localMetaType)
    {
        QVariant qv = variant.variant();
        QVariant qvF = BlackMisc::fixQVariantFromDbusArgument(qv, localMetaType);
        QString s = BlackMisc::qVariantToString(qvF, true);
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received variant:" << s;
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
     * Ping altitude
     */
    BlackMisc::Aviation::CAltitude Testservice::pingAltitude(const BlackMisc::Aviation::CAltitude &altitude)
    {
        return altitude;
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
    void Testservice::receiveAtcStationList(const BlackMisc::Aviation::CAtcStationList &AtcStationList) const
    {
        qDebug() << "Pid:" << ServiceTool::getPid() << "Received ATC list:" << AtcStationList;
    }

    /*
     * Receive ATC list
     */
    BlackMisc::Aviation::CAtcStationList Testservice::pingAtcStationList(const BlackMisc::Aviation::CAtcStationList &AtcStationList) const
    {
        return AtcStationList;
    }

    /*
     * Receive value map
     */
    void Testservice::receiveValueMap(const BlackMisc::CValueMap &valueMap) const
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
     * Ping ATC station
     */
    const BlackMisc::Aviation::CAtcStationList Testservice::getAtcStationList(const qint32 number) const
    {
        return BlackMisc::Aviation::CAtcStationList(ServiceTool::getStations(number));
    }

    /*
     * Object paths
     */
    const QList<QDBusObjectPath> Testservice::getObjectPaths(const qint32 number) const
    {
        QList<QDBusObjectPath> paths;
        for (int i = 0; i < number; i++)
        {
            paths.append(QDBusObjectPath(BLACKMISCKTEST_TESTSERVICE_SERVICEPATH));
        }
        return paths;
    }

    /*
     * Ping ATC station
     */
    BlackMisc::Aviation::CAtcStation Testservice::pingAtcStation(const BlackMisc::Aviation::CAtcStation &station)
    {
        return station;
    }

} // namespace
