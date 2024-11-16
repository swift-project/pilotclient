// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscquantities

#include "samplesaviation.h"

#include <QDateTime>
#include <QString>
#include <QTextStream>

#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/heading.h"
#include "misc/aviation/navsystem.h"
#include "misc/aviation/transponder.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/range.h"
#include "misc/stringutils.h"
#include "misc/test/testdata.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::test;

namespace swift::sample
{
    int CSamplesAviation::samples(QTextStream &out)
    {
        CHeading h1(180, CHeading::Magnetic, CAngleUnit::deg());
        CHeading h2(180, CHeading::True, CAngleUnit::deg());

        out << h1 << Qt::endl;
        out << h1 << " " << h2 << " " << (h1 == h2) << " " << (h1 != h2) << " " << (h1 == h1) << Qt::endl;

        // COM system
        CComSystem c1 = CComSystem::getCom1System(125.3);
        out << c1 << Qt::endl;
        c1.setActiveUnicom();
        out << c1 << Qt::endl;

        // NAV system
        CNavSystem nav1 = CNavSystem::getNav1System(110.0);
        out << nav1 << Qt::endl;

        // Transponder tests
        CTransponder tr1(7000, CTransponder::StateStandby);
        CTransponder tr2("4532", CTransponder::ModeMil3);
        out << tr1 << " " << tr2 << Qt::endl;

        // Callsign and ATC station
        CCallsign callsign1("d-ambz");
        CCallsign callsign2("DAmbz");
        out << callsign1 << " " << callsign2 << " " << (callsign1 == callsign2) << Qt::endl;

        CAtcStation station1 = CTestData::getFrankfurtTower();
        CAtcStation station2(station1);
        CAtcStation station3(station1);
        station3.setController(CTestData::getRandomController());

        out << station1 << " " << station2 << " " << (station1.getCallsign() == station2.getCallsign()) << Qt::endl;

        // User parsing
        CUser user("12345", "Joe KING KGLC");
        out << user.getRealName() << user.getHomeBase() << Qt::endl;

        // ATC List
        CAtcStationList atcList = CTestData::getAtcStations();
        atcList.push_back(CTestData::getAtcStations());
        atcList = atcList.findBy(&CAtcStation::getCallsign, "eddm_twr", &CAtcStation::getFrequency,
                                 CFrequency(118.7, CFrequencyUnit::MHz()));
        atcList = atcList.sortedBy(&CAtcStation::getCallsign, &CAtcStation::getControllerRealName);
        out << atcList << Qt::endl;
        out << "-----------------------------------------------" << Qt::endl;

        // flight plan
        CAltitude alt("FL110");
        CAltitude altMsl(alt);
        altMsl.toMeanSeaLevel();

        out << alt << " " << altMsl << Qt::endl;
        CAirportIcaoCode frankfurt("eddf");
        out << frankfurt << Qt::endl;
        out << "-----------------------------------------------" << Qt::endl;

        return 0;
    }
} // namespace swift::sample
