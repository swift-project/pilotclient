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
    int CSamplesPerformance::samples(QTextStream &out)
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

} // namespace
