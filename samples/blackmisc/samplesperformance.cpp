/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesperformance.h"
#include "blackmisc/testing.h"

using namespace BlackMisc::Aviation;

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesPerformance::samples()
    {
        QTime timer;
        int ms, number;
        BlackMisc::Aviation::CTesting::copy10kStations(1); // init

        // ATC stations, tradionally created
        timer.start();
        CAtcStationList atcs1 = BlackMisc::Aviation::CTesting::createAtcStations(10000);
        ms = timer.elapsed();
        qDebug() << "created (copy)" << atcs1.size() << "ATC stations in" << ms << "ms";

        timer.start();
        CAtcStationList atcs2 = BlackMisc::Aviation::CTesting::createAtcStations(100000);
        ms = timer.elapsed();
        qDebug() << "created (copy)" << atcs2.size() << "ATC stations in" << ms << "ms";

        // ATC stations, property index created
        timer.start();
        CAtcStationList atcs3 = BlackMisc::Aviation::CTesting::createAtcStations(10000, true);
        ms = timer.elapsed();
        qDebug() << "created (propertyIndex)" << atcs3.size() << "ATC stations in" << ms << "ms";

        timer.start();
        CAtcStationList atcs4 = BlackMisc::Aviation::CTesting::createAtcStations(100000, true);
        ms = timer.elapsed();
        qDebug() << "created (propertyIndex)" << atcs4.size() << "ATC stations in" << ms << "ms";

        // Sort by
        timer.start();
        atcs1.sortBy(&CAtcStation::getCallsign);
        ms = timer.elapsed();
        qDebug() << "Sorted by callsign" << atcs1.size() << "ATC stations in" << ms << "ms";

        timer.start();
        atcs2.sortBy(&CAtcStation::getCallsign);
        ms = timer.elapsed();
        qDebug() << "Sorted by callsign" << atcs2.size() << "ATC stations in" << ms << "ms";

        // Read data, this is what all our models do when displaying in a table view
        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs1, false);
        ms = timer.elapsed();
        qDebug() << "Read (getters)" << atcs1.size() << "ATC stations in" << ms << "ms";

        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs2, false);
        ms = timer.elapsed();
        qDebug() << "Read (getters)" << atcs2.size() << "ATC stations in" << ms << "ms";

        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs1, true);
        ms = timer.elapsed();
        qDebug() << "Read (propertyIndex)" << atcs1.size() << "ATC stations in" << ms << "ms";

        timer.start();
        BlackMisc::Aviation::CTesting::readStations(atcs2, true);
        ms = timer.elapsed();
        qDebug() << "Read (propertyIndex)" << atcs2.size() << "ATC stations in" << ms << "ms";

        // calculate
        number = 10000;
        timer.start();
        BlackMisc::Aviation::CTesting::calculateDistance(number);
        ms = timer.elapsed();
        qDebug() << "Calculated distances " << number << "in" << ms << "ms";

        number = 100000;
        timer.start();
        BlackMisc::Aviation::CTesting::calculateDistance(number);
        ms = timer.elapsed();
        qDebug() << "Calculated distances " << number << "in" << ms << "ms";

        // parse
        number = 100000;
        timer.start();
        BlackMisc::Aviation::CTesting::parseWgs(number);
        ms = timer.elapsed();
        qDebug() << "Parse WGS coordinates" << number << "in" << ms << "ms";

        // copy
        timer.start();
        number = 20;
        BlackMisc::Aviation::CTesting::copy10kStations(number);
        ms = timer.elapsed();
        qDebug() << "Copied 10k stations" << number << "times in" << ms << "ms";

        timer.start();
        number = 100;
        BlackMisc::Aviation::CTesting::copy10kStations(number);
        ms = timer.elapsed();
        qDebug() << "Copied 10k stations" << number << "times in" << ms << "ms";

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
        qDebug() << "new regex matched" << number << "of" << strList1.size() << "strings in" << ms << "ms";
        number = 0;
        timer.start();
        for (const auto &str : strList2) { if (fullRegex.exactMatch(str)) number++; }
        ms = timer.elapsed();
        qDebug() << "full regex matched" << number << "of" << strList2.size() << "strings in" << ms << "ms";
        number = 0;
        timer.start();
        for (const auto &str : strList3) { if (wildcardRegex.exactMatch(str)) number++; }
        ms = timer.elapsed();
        qDebug() << "wildcard matched" << number << "of" << strList3.size() << "strings in" << ms << "ms";
        number = 0;
        timer.start();
        for (const auto &str : strList4) { if (str.contains(containsStr)) number++; }
        ms = timer.elapsed();
        qDebug() << "contains matched" << number << "of" << strList4.size() << "strings in" << ms << "ms";

        qDebug() << "-----------------------------------------------";
        return 0;
    }

} // namespace
