// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#include "samplesjson.h"

#include <stdio.h>

#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QtDebug>

#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/heading.h"
#include "misc/aviation/transponder.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/network/user.h"
#include "misc/pq/angle.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/speed.h"
#include "misc/pq/units.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/stringutils.h"
#include "misc/test/testdata.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::test;

namespace swift::sample
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
} // namespace swift::sample
