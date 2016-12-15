/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmisc

#include "samplesjson.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/stringutils.h"

#include <stdio.h>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QtDebug>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackSample
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
        l.convertFromJson(json);
        qDebug() << json << l;
        qDebug() << "-------";
        QDateTime dtFrom = QDateTime::currentDateTimeUtc();
        QDateTime dtUntil = dtFrom.addSecs(60 * 60); // 1 hour
        CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
        CAtcStation station(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                            CFrequency(118.7, CFrequencyUnit::MHz()), geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);
        json = station.toJson();
        QJsonDocument doc(json);
        qDebug() << doc.toJson(QJsonDocument::Indented);
        qDebug() << "-------";

        station = CAtcStation();
        station.convertFromJson(json);
        qDebug() << station;
        qDebug() << "------- Enter -----";
        cin.readLine();

        CAircraftSituation situation(geoPos, CHeading(10, CHeading::True, CAngleUnit::deg()),
                                     CAngle(12, CAngleUnit::deg()), CAngle(5, CAngleUnit::deg()),
                                     CSpeed(111, CSpeedUnit::km_h()));
        CSimulatedAircraft aircraft(CCallsign("DAMBZ"), CUser("123456", "Joe Pilot"), situation);
        aircraft.setCom1System(CComSystem::getCom1System(122.8, 118.75));
        aircraft.setCom2System(CComSystem::getCom2System(123.8, 124.00));
        aircraft.setTransponder(CTransponder::getStandardTransponder(7000, CTransponder::ModeC));
        aircraft.setIcaoCodes(CAircraftIcaoCode("B737", "L2J"), CAirlineIcaoCode("DLH"));

        json = aircraft.toJson();
        doc = QJsonDocument(json);
        qDebug() << doc.toJson(QJsonDocument::Indented);
        qDebug() << "-------";

        aircraft = CSimulatedAircraft();
        aircraft.convertFromJson(json);
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
        stations.convertFromJson(json);
        qDebug() << stations;
        qDebug() << "------- Enter -----";
        cin.readLine();

        // testing escaping special characters
        CUser specialCharacters("123456", "With quote \"", "With double quote\"\"", "foobar");
        json = specialCharacters.toJson();
        doc.setObject(json);
        qDebug() << doc.toJson(QJsonDocument::Indented);

        qDebug() << "------- Enter ---------------------------------";
        cin.readLine();

        return 0;
    }

} // namespace
