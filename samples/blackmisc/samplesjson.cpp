/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesjson.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraft.h"
#include <QJsonDocument>
#include <QTextStream>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesJson::samples()
    {
        QTextStream cin(stdin);
        CLength l(123.456, CLengthUnit::NM());
        QJsonObject json = l.toJson();
        l = CLength(); //convert back
        l.fromJson(json);
        qDebug() << json << l;
        qDebug() << "-------";
        QDateTime dtFrom = QDateTime::currentDateTimeUtc();
        QDateTime dtUntil = dtFrom.addSecs(60 * 60.0); // 1 hour
        CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CLength(1487, CLengthUnit::ft()));
        CAtcStation station(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                            CFrequency(118.7, CFrequencyUnit::MHz()), geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);
        json = station.toJson();
        QJsonDocument doc(json);
        qDebug() << doc.toJson(QJsonDocument::Indented);
        qDebug() << "-------";

        station = CAtcStation();
        station.fromJson(json);
        qDebug() << station;
        qDebug() << "------- Enter -----";
        cin.readLine();

        CAircraftSituation situation(geoPos, CAltitude(1000, CAltitude::AboveGround, CLengthUnit::ft()),
                                     CHeading(10, CHeading::True, CAngleUnit::deg()),
                                     CAngle(12, CAngleUnit::deg()), CAngle(5, CAngleUnit::deg()),
                                     CSpeed(111, CSpeedUnit::km_h()));
        CAircraft aircraft(CCallsign("DAMBZ"), CUser("123456", "Joe Pilot"), situation);
        aircraft.setCom1System(CComSystem::getCom1System(122.8, 118.75));
        aircraft.setCom2System(CComSystem::getCom2System(123.8, 124.00));
        aircraft.setTransponder(CTransponder::getStandardTransponder(7000, CTransponder::ModeC));
        aircraft.setIcaoInfo(CAircraftIcao("B737", "L2J", "DLH", "FREIGHT", "CCDDFF"));

        json = aircraft.toJson();
        doc = QJsonDocument(json);
        qDebug() << doc.toJson(QJsonDocument::Indented);
        qDebug() << "-------";

        aircraft = CAircraft();
        aircraft.fromJson(json);
        qDebug() << aircraft;
        qDebug() << "------- Enter -----";
        cin.readLine();

        CAtcStationList stations;
        stations.push_back(station);
        station.setCallsign(CCallsign("eddn_gnd"));
        stations.push_back(station);
        json = stations.toJson();
        doc.setObject(json);
        qDebug() << doc.toJson(QJsonDocument::Indented);
        qDebug() << "------- Enter -----";
        cin.readLine();

        stations.clear();
        stations.fromJson(json);
        qDebug() << stations;

        qDebug() << "-----------------------------------------------";
        return 0;
    }

} // namespace
