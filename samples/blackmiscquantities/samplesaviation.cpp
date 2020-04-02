/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscquantities

#include "samplesaviation.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/navsystem.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/compare.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/test/testdata.h"
#include "blackmisc/range.h"
#include "blackmisc/stringutils.h"

#include <QDateTime>
#include <QString>
#include <QTextStream>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Test;

namespace BlackSample
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
        atcList = atcList.findBy(&CAtcStation::getCallsign, "eddm_twr", &CAtcStation::getFrequency, CFrequency(118.7, CFrequencyUnit::MHz()));
        atcList = atcList.sortedBy(&CAtcStation::getBookedFromUtc, &CAtcStation::getCallsign, &CAtcStation::getControllerRealName);
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

        CMetaMemberComparator cmp;
        QList<QPair<QString, bool>> list = cmp(station1, station3);
        for (const auto &member : as_const(list)) { out << member.first << (member.second ? " equal" : " NOT equal") << Qt::endl; }
        out << Qt::endl;
        list = cmp(station1, station3, { "controller" });
        for (const auto &member : as_const(list)) { out << member.first << (member.second ? " equal" : " NOT equal") << Qt::endl; }
        out << Qt::endl;
        list = cmp(station1, station3, { "controller", "homebase" });
        for (const auto &member : as_const(list)) { out << member.first << (member.second ? " equal" : " NOT equal") << Qt::endl; }
        out << "-----------------------------------------------" << Qt::endl;

        return 0;
    }
} // namespace
