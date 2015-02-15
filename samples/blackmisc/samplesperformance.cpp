/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesperformance.h"
#include "blackmisc/avaircraftsituationlist.h"
#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/avaltitude.h"
#include "blackmisc/testing.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesPerformance::samplesMisc(QTextStream &out)
    {
        QTime timer;
        int ms, number;
        BlackMisc::Aviation::CTesting::copy10kStations(1); // init

        // ATC stations, tradionally created
        timer.start();
        CAtcStationList atcs1 = BlackMisc::Aviation::CTesting::createAtcStations(10000);
        ms = timer.elapsed();
        out << "created (copy) " << atcs1.size() << " ATC stations in " << ms << "ms" << endl;

        timer.start();
        CAtcStationList atcs2 = BlackMisc::Aviation::CTesting::createAtcStations(100000);
        ms = timer.elapsed();
        out << "created (copy) " << atcs2.size() << " ATC stations in " << ms << "ms" << endl;

        // ATC stations, property index created
        timer.start();
        CAtcStationList atcs3 = BlackMisc::Aviation::CTesting::createAtcStations(10000, true);
        ms = timer.elapsed();
        out << "created (propertyIndex) " << atcs3.size() << " ATC stations in " << ms << "ms" << endl;

        timer.start();
        CAtcStationList atcs4 = BlackMisc::Aviation::CTesting::createAtcStations(100000, true);
        ms = timer.elapsed();
        out << "created (propertyIndex) " << atcs4.size() << " ATC stations in " << ms << "ms" << endl;

        // Sort by
        timer.start();
        atcs1.sortBy(&CAtcStation::getCallsign);
        ms = timer.elapsed();
        out << "Sorted by callsign " << atcs1.size() << " ATC stations in " << ms << "ms" << endl;

        timer.start();
        atcs2.sortBy(&CAtcStation::getCallsign);
        ms = timer.elapsed();
        out << "Sorted by callsign " << atcs2.size() << " ATC stations in " << ms << "ms" << endl;

        // Read data, this is what all our models do when displaying in a table view
        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs1, false);
        ms = timer.elapsed();
        out << "Read (getters) " << atcs1.size() << " ATC stations in " << ms << "ms" << endl;

        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs2, false);
        ms = timer.elapsed();
        out << "Read (getters) " << atcs2.size() << " ATC stations in " << ms << "ms"  << endl;

        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs1, true);
        ms = timer.elapsed();
        out << "Read (propertyIndex) " << atcs1.size() << " ATC stations in " << ms << "ms" << endl;

        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs2, true);
        ms = timer.elapsed();
        out << "Read (propertyIndex) " << atcs2.size() << " ATC stations in " << ms << "ms" << endl;

        // calculate
        number = 10000;
        timer.start();
        BlackMisc::Aviation::CTesting::calculateDistance(number);
        ms = timer.elapsed();
        out << "Calculated distances " << number << " in " << ms << "ms" << endl;

        number = 100000;
        timer.start();
        BlackMisc::Aviation::CTesting::calculateDistance(number);
        ms = timer.elapsed();
        out << "Calculated distances " << number << "in " << ms << "ms" << endl;

        // parse
        number = 100000;
        timer.start();
        BlackMisc::Aviation::CTesting::parseWgs(number);
        ms = timer.elapsed();
        out << "Parse WGS coordinates " << number << " in " << ms << "ms" << endl;

        // copy
        timer.start();
        number = 20;
        BlackMisc::Aviation::CTesting::copy10kStations(number);
        ms = timer.elapsed();
        out << "Copied 10k stations " << number << " times in " << ms << "ms" << endl;

        timer.start();
        number = 100;
        BlackMisc::Aviation::CTesting::copy10kStations(number);
        ms = timer.elapsed();
        out << "Copied 10k stations " << number << " times in " << ms << "ms" << endl;

        // Regex pattern matching with lists of 10000 strings containing random hex numbers
        auto generator = []() { return QString::number(qrand() | (qrand() << 16), 16); };
        QStringList strList1, strList2, strList3, strList4;
        std::generate_n(std::back_inserter(strList1), 100000, generator);
        std::generate_n(std::back_inserter(strList2), 100000, generator);
        std::generate_n(std::back_inserter(strList3), 100000, generator);
        std::generate_n(std::back_inserter(strList4), 100000, generator);
        QRegularExpression newRegex("^.*aaa.*$", QRegularExpression::CaseInsensitiveOption);
        QRegExp fullRegex(".*aaa.*", Qt::CaseInsensitive);
        QRegExp wildcardRegex("*aaa*", Qt::CaseInsensitive, QRegExp::Wildcard);
        QString containsStr("aaa");
        number = 0;
        timer.start();
        for (const auto &str : strList1) { if (newRegex.match(str).hasMatch()) number++; }
        ms = timer.elapsed();
        out << "new regex matched " << number << " of" << strList1.size() << " strings in " << ms << "ms"  << endl;
        number = 0;
        timer.start();
        for (const auto &str : strList2) { if (fullRegex.exactMatch(str)) number++; }
        ms = timer.elapsed();
        out << "full regex matched " << number << " of" << strList2.size() << " strings in " << ms << "ms"  << endl;
        number = 0;
        timer.start();
        for (const auto &str : strList3) { if (wildcardRegex.exactMatch(str)) number++; }
        ms = timer.elapsed();
        out << "wildcard matched " << number << " of " << strList3.size() << " strings in " << ms << "ms"  << endl;
        number = 0;
        timer.start();
        for (const auto &str : strList4) { if (str.contains(containsStr)) number++; }
        ms = timer.elapsed();
        out << "contains matched " << number << " of " << strList4.size() << " strings in " << ms << "ms" << endl;

        out << "-----------------------------------------------"  << endl;
        return 0;
    }

    int CSamplesPerformance::samplesImplementationType(QTextStream &out, int numberOfCallsigns, int numberOfTimes)
    {
        const QDateTime baseTime = QDateTime::currentDateTimeUtc();
        const qint64 baseTimeEpoch = baseTime.toMSecsSinceEpoch();
        CAircraftSituationList situations;

        for (int cs = 0; cs < numberOfCallsigns; cs++)
        {
            CCallsign callsign("CS" + QString::number(cs));
            CCoordinateGeodetic coordinate(cs, cs, cs);
            CAltitude alt(cs, CAltitude::MeanSeaLevel, CLengthUnit::m());
            for (int t = 0; t < numberOfTimes; t++)
            {
                CAircraftSituation s(callsign, coordinate, alt);
                s.setMSecsSinceEpoch(baseTimeEpoch + 10 * t);
                situations.push_back(s);
            }
        }

        QTime timer;
        out << "Created " << situations.size() << " situations" << endl;

        timer.start();
        for (int i = 0; i < 10; i++)
        {
            for (int cs = 0; cs < numberOfCallsigns; cs++)
            {
                CCallsign callsign("CS" + QString::number(cs));
                CAircraftSituationList r = situations.findByCallsign(callsign);
                Q_ASSERT(r.size() == numberOfTimes);
            }
        }
        out << "Reads by callsigns: " << timer.elapsed() << "ms" << endl;

        timer.start();
        for (int i = 0; i < 10; i++)
        {
            QMap<CCallsign, CAircraftSituationList> splitList = situations.splitPerCallsign();
            Q_ASSERT(splitList.size() == numberOfCallsigns);
            for (const CAircraftSituationList &slcs : splitList.values())
            {
                Q_ASSERT(slcs.size() == numberOfTimes);
            }
        }
        out << "Reads by callsigns split: " << timer.elapsed() << "ms" << endl;

        timer.start();
        for (int i = 0; i < 10; i++)
        {
            for (int t = 0; t < numberOfTimes; t++)
            {
                CAircraftSituationList r = situations.findBefore(baseTimeEpoch + 1 + (10 * t));
                Q_ASSERT(r.size() == numberOfCallsigns * (t + 1));
            }
        }
        out << "Reads by times: " << timer.elapsed() << "ms" << endl;

        timer.start();
        for (int t = 0; t < numberOfTimes; t++)
        {
            for (int cs = 0; cs < numberOfCallsigns; cs++)
            {
                CCallsign callsign("CS" + QString::number(cs));
                CAircraftSituationList r = situations.findByCallsign(callsign).findBefore(baseTimeEpoch + 1 + (10 * t));
                Q_UNUSED(r);
            }
        }
        out << "Reads by callsigns / times: " << timer.elapsed() << "ms" << endl;

        timer.start();
        for (int t = 0; t < numberOfTimes; t++)
        {
            for (int cs = 0; cs < numberOfCallsigns; cs++)
            {
                CCallsign callsign("CS" + QString::number(cs));
                CAircraftSituationList r = situations.findBefore(baseTimeEpoch + 1 + (10 * t)).findByCallsign(callsign);
                Q_UNUSED(r);
            }
        }
        out << "Reads by times / callsigns: " << timer.elapsed() << "ms" << endl;

        timer.start();
        for (int t = 0; t < numberOfTimes; t++)
        {
            QMap<CCallsign, CAircraftSituationList> splitList = situations.splitPerCallsign();
            Q_ASSERT(splitList.size() == numberOfCallsigns);
            for (const CAircraftSituationList &slcs : splitList.values())
            {
                CAircraftSituationList r = slcs.findBefore(baseTimeEpoch + 1 + (10 * t));
                Q_UNUSED(r);
            }
        }
        out << "Split reads by callsigns / times: " << timer.elapsed() << "ms" << endl;

        situations.changeImpl<QVector<CAircraftSituation> >();
        out << "Changed to QVector" << endl;
        timer.start();
        for (int i = 0; i < 10; i++)
        {
            for (int cs = 0; cs < numberOfCallsigns; cs++)
            {
                CCallsign callsign("CS" + QString::number(cs));
                CAircraftSituationList r = situations.findByCallsign(callsign);
                Q_ASSERT(r.size() == numberOfTimes);
            }
        }
        out << "Reads by callsigns: " << timer.elapsed() << "ms" << endl;

        timer.start();
        for (int i = 0; i < 10; i++)
        {
            for (int t = 0; t < numberOfTimes; t++)
            {
                CAircraftSituationList r = situations.findBefore(baseTimeEpoch + 1 + (10 * t));
                Q_ASSERT(r.size() == numberOfCallsigns * (t + 1));
            }
        }
        out << "Reads by times: " << timer.elapsed() << "ms" << endl << endl;
        return 0;
    }

} // namespace
