// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
#include "blackmisc/test/testdata.h"
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
using namespace BlackMisc::Test;

namespace BlackSample
{
    int CSamplesJson::samples()
    {
        QTextStream cin(stdin);
        CLength l(123.456, CLengthUnit::NM());
        QJsonObject json = l.toJson();
        l = CLength(); // convert back
        l.convertFromJson(json);
        qDebug() << json << l;
        qDebug() << "-------";
        CAtcStation station = CTestData::getMunichTower();
        json = station.toJson();
        QJsonDocument doc(json);
        qDebug().noquote() << doc.toJson(QJsonDocument::Indented);
        qDebug() << "-------";

        station = CAtcStation();
        station.convertFromJson(json);
        qDebug() << station;
        qDebug() << "------- Enter -----";
        cin.readLine();

        CSimulatedAircraft aircraft(CCallsign("DAMBZ"), CUser("123456", "Joe Pilot"), CTestData::getAircraftSituationAboveMunichTower());
        aircraft.setCom1System(CComSystem::getCom1System(122.8, 118.75));
        aircraft.setCom2System(CComSystem::getCom2System(123.8, 124.00));
        aircraft.setTransponder(CTransponder::getStandardTransponder(7000, CTransponder::ModeC));
        aircraft.setIcaoCodes(CTestData::getDBAircraftIcaoB737(), CAirlineIcaoCode("DLH"));

        json = aircraft.toJson();
        doc = QJsonDocument(json);
        qDebug() << doc.toJson(QJsonDocument::Indented);
        qDebug() << "-------";

        aircraft = CSimulatedAircraft();
        aircraft.convertFromJson(json);
        qDebug() << aircraft;
        qDebug() << "------- Enter -----";
        cin.readLine();

        CAtcStationList stations = CTestData::getAtcStations();
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
