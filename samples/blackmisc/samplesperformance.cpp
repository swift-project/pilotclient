/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmisc

#include "samplesperformance.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/test/testing.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/stringutils.h"

#include <QDateTime>
#include <QHash>
#include <QList>
#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <QStringList>
#include <QStringBuilder>
#include <QTextStream>
#include <QElapsedTimer>
#include <QVector>
#include <Qt>
#include <algorithm>
#include <iterator>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Test;
using namespace BlackCore::Db;

namespace BlackSample
{
    int CSamplesPerformance::samplesMisc(QTextStream &out)
    {
        QElapsedTimer timer;
        int ms, number;
        CSamplesPerformance::copy10kStations(1); // init

        // ATC stations, tradionally created
        timer.start();
        CAtcStationList atcs1 = CTesting::createAtcStations(10000);
        ms = timer.elapsed();
        out << "created (copy) " << atcs1.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        timer.start();
        CAtcStationList atcs2 = CTesting::createAtcStations(100000);
        ms = timer.elapsed();
        out << "created (copy) " << atcs2.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        // ATC stations, property index created
        timer.start();
        CAtcStationList atcs3 = CTesting::createAtcStations(10000, true);
        ms = timer.elapsed();
        out << "created (propertyIndex) " << atcs3.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        timer.start();
        CAtcStationList atcs4 = CTesting::createAtcStations(100000, true);
        ms = timer.elapsed();
        out << "created (propertyIndex) " << atcs4.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        // Sort by
        timer.start();
        atcs1.sortBy(&CAtcStation::getCallsign);
        ms = timer.elapsed();
        out << "Sorted by callsign " << atcs1.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        timer.start();
        atcs2.sortBy(&CAtcStation::getCallsign);
        ms = timer.elapsed();
        out << "Sorted by callsign " << atcs2.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        // Read data, this is what all our models do when displaying in a table view
        timer.start();
        CSamplesPerformance::accessStationsData(atcs1, false);
        ms = timer.elapsed();
        out << "Read (getters) " << atcs1.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        timer.start();
        CSamplesPerformance::accessStationsData(atcs2, false);
        ms = timer.elapsed();
        out << "Read (getters) " << atcs2.size() << " ATC stations in " << ms << "ms"  << Qt::endl;

        timer.start();
        CSamplesPerformance::accessStationsData(atcs1, true);
        ms = timer.elapsed();
        out << "Read (propertyIndex) " << atcs1.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        timer.start();
        CSamplesPerformance::accessStationsData(atcs2, true);
        ms = timer.elapsed();
        out << "Read (propertyIndex) " << atcs2.size() << " ATC stations in " << ms << "ms" << Qt::endl;

        // calculate
        number = 10000;
        timer.start();
        CSamplesPerformance::calculateDistance(number);
        ms = timer.elapsed();
        out << "Calculated distances " << number << " in " << ms << "ms" << Qt::endl;

        number = 100000;
        timer.start();
        CSamplesPerformance::calculateDistance(number);
        ms = timer.elapsed();
        out << "Calculated distances " << number << "in " << ms << "ms" << Qt::endl;

        // parse
        number = 100000;
        timer.start();
        CSamplesPerformance::parseWgs(number);
        ms = timer.elapsed();
        out << "Parse WGS coordinates " << number << " in " << ms << "ms" << Qt::endl;

        // copy
        timer.start();
        number = 20;
        CSamplesPerformance::copy10kStations(number);
        ms = timer.elapsed();
        out << "Copied 10k stations " << number << " times in " << ms << "ms" << Qt::endl;

        timer.start();
        number = 100;
        CSamplesPerformance::copy10kStations(number);
        ms = timer.elapsed();
        out << "Copied 10k stations " << number << " times in " << ms << "ms" << Qt::endl;

        // Regex pattern matching with lists of 10000 strings containing random hex numbers
        auto generator = []() { return QString::number(CMathUtils::randomGenerator().generate(), 16); };
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
        for (const auto &str : std::as_const(strList1)) { if (newRegex.match(str).hasMatch()) number++; }
        ms = timer.elapsed();
        out << "new regex matched " << number << " of" << strList1.size() << " strings in " << ms << "ms"  << Qt::endl;
        number = 0;
        timer.start();
        for (const auto &str : std::as_const(strList2)) { if (fullRegex.exactMatch(str)) number++; }
        ms = timer.elapsed();
        out << "full regex matched " << number << " of" << strList2.size() << " strings in " << ms << "ms"  << Qt::endl;
        number = 0;
        timer.start();
        for (const auto &str : std::as_const(strList3)) { if (wildcardRegex.exactMatch(str)) number++; }
        ms = timer.elapsed();
        out << "wildcard matched " << number << " of " << strList3.size() << " strings in " << ms << "ms"  << Qt::endl;
        number = 0;
        timer.start();
        for (const auto &str : std::as_const(strList4)) { if (str.contains(containsStr)) number++; }
        ms = timer.elapsed();
        out << "contains matched " << number << " of " << strList4.size() << " strings in " << ms << "ms" << Qt::endl;

        out << "-----------------------------------------------"  << Qt::endl;
        return EXIT_SUCCESS;
    }

    int CSamplesPerformance::interpolatorScenario(QTextStream &out, int numberOfCallsigns, int numberOfTimes)
    {
        const qint64 baseTimeEpoch = QDateTime::currentMSecsSinceEpoch();
        CAircraftSituationList situations = createSituations(baseTimeEpoch, numberOfCallsigns, numberOfTimes);
        CAircraftSituationList situationsBefore;
        CAircraftSituationList situationsAfter;

        qint64 halfTime = baseTimeEpoch + DeltaTime * numberOfTimes / 2;

        QElapsedTimer timer;
        timer.start();
        for (int cs = 0; cs < numberOfCallsigns; cs++)
        {
            CCallsign callsign("CS" + QString::number(cs));
            situationsBefore = situations.findBefore(halfTime).findByCallsign(callsign);
            situationsAfter = situations.findAfter(halfTime - 1).findByCallsign(callsign);
        }
        out << "Reads by time, callsigns: " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        situationsBefore = situations.findBefore(halfTime);
        situationsAfter = situations.findAfter(halfTime - 1);
        for (int cs = 0; cs < numberOfCallsigns; cs++)
        {
            CCallsign callsign("CS" + QString::number(cs));
            CAircraftSituationList csSituationsBefore = situationsBefore.findByCallsign(callsign);
            CAircraftSituationList csSituationsAfter = situationsAfter.findByCallsign(callsign);
            Q_UNUSED(csSituationsBefore);
            Q_UNUSED(csSituationsAfter);
        }
        out << "Split by time upfront, then callsigns: " << timer.elapsed() << "ms" << Qt::endl;
        int b = situationsBefore.size();
        int a = situationsAfter.size();
        Q_ASSERT(a + b == numberOfTimes * numberOfCallsigns);
        Q_UNUSED(a);
        Q_UNUSED(b);

        timer.start();
        const QHash<CCallsign, CAircraftSituationList> csSituations = situations.splitPerCallsign();
        out << "Split by " << csSituations.size() << " callsigns, " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        for (const CAircraftSituationList &csl : csSituations)
        {
            CAircraftSituationList csSituationsBefore = csl.findBefore(halfTime);
            CAircraftSituationList csSituationsAfter = csl.findAfter(halfTime - 1);
            a = csSituationsBefore.size();
            b = csSituationsAfter.size();
            Q_ASSERT(a + b == numberOfTimes);
            Q_UNUSED(csSituationsBefore);
            Q_UNUSED(csSituationsAfter);
        }
        out << "Split by callsign, by time: " << timer.elapsed() << "ms" << Qt::endl;

        out << Qt::endl;
        return EXIT_SUCCESS;
    }

    int CSamplesPerformance::samplesJson(QTextStream &out)
    {
        QElapsedTimer timer;
        auto situations = createSituations(0, 10000, 10);
        auto models = createModels(10000, 100);

        timer.start();
        QJsonObject json = situations.toJson();
        out << "Convert 100,000 aircraft situations to JSON:        " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        situations.convertFromJson(json);
        out << "Convert 100,000 aircraft situations from JSON:      " << timer.elapsed() << "ms" << endl << Qt::endl;

        timer.start();
        json = models.toJson();
        out << "Convert 10,000 aircraft models to JSON (naive):     " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        models.convertFromJson(json);
        out << "Convert 10,000 aircraft models from JSON (naive):   " << timer.elapsed() << "ms" << endl << Qt::endl;

        timer.start();
        json = models.toMemoizedJson();
        out << "Convert 10,000 aircraft models to JSON (memoize):   " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        models.convertFromMemoizedJson(json);
        out << "Convert 10,000 aircraft models from JSON (memoize): " << timer.elapsed() << "ms" << endl << Qt::endl;

        return EXIT_SUCCESS;
    }

    int CSamplesPerformance::samplesJsonModelAndLivery(QTextStream &out)
    {
        const QString dir = CSwiftDirectories::staticDbFilesDirectory();
        const QString modelFileName = QDir(dir).filePath("models.json");
        const QString liveriesFileName = QDir(dir).filePath("liveries.json");

        QFile modelFile(modelFileName);
        Q_ASSERT_X(modelFile.exists(), Q_FUNC_INFO, "Model file does not exist");
        QFile liveryFile(liveriesFileName);
        Q_ASSERT_X(liveryFile.exists(), Q_FUNC_INFO, "Liveries file does not exist");

        out << "Loaded DB JSON model file " << modelFile.fileName() << Qt::endl;
        const QString modelData = CFileUtils::readFileToString(modelFile.fileName());
        Q_ASSERT_X(!modelData.isEmpty(), Q_FUNC_INFO, "Model file empty");

        out << "Loaded DB JSON livery file " << liveryFile.fileName() << Qt::endl;
        const QString liveryData = CFileUtils::readFileToString(liveryFile.fileName());
        Q_ASSERT_X(!liveryData.isEmpty(), Q_FUNC_INFO, "Livery file empty");

        // DB format, all models denormalized in DB JSON format
        CDatabaseReader::JsonDatastoreResponse response;
        QElapsedTimer timer;

        CDatabaseReader::stringToDatastoreResponse(liveryData, response);
        timer.start();
        const CLiveryList dbLiveries = CLiveryList::fromDatabaseJson(response);
        int ms = timer.elapsed();
        out << "Read via DB JSON format: " << dbLiveries.size() << " liveries in " << ms << "ms" << Qt::endl;

        // does not result in better performance, liveries/airlines have almost a 1:1 ratio
        // unlike models' fromDatabaseJsonCaching not many airlines will be recycled
        timer.start();
        const CLiveryList dbLiveries2 = CLiveryList::fromDatabaseJsonCaching(response);
        ms = timer.elapsed();
        out << "Read via DB JSON format (new): " << dbLiveries2.size() << " liveries in " << ms << "ms" << Qt::endl;

        const CAirlineIcaoCodeList liveryAirlines = dbLiveries2.getAirlines();
        timer.start();
        const CLiveryList dbLiveries3 = CLiveryList::fromDatabaseJsonCaching(response, liveryAirlines);
        ms = timer.elapsed();
        out << "Read via DB JSON format (new, passing airlines): " << dbLiveries3.size() << " liveries in " << ms << "ms" << Qt::endl;

        CDatabaseReader::stringToDatastoreResponse(modelData, response);
        timer.start();
        const CAircraftModelList dbModels = CAircraftModelList::fromDatabaseJson(response);
        ms = timer.elapsed();
        out << "Read via DB JSON format: " << dbModels.size() << " models in " << ms << "ms" << Qt::endl;

        timer.start();
        const CAircraftModelList dbModels2 = CAircraftModelList::fromDatabaseJsonCaching(response);
        ms = timer.elapsed();
        out << "Read via DB JSON format (new): " << dbModels2.size() << " models in " << ms << "ms" << Qt::endl;

        // swift JSON format
        const QJsonObject swiftJsonObject = dbModels.toJson();
        out << "Converted to swift JSON" << Qt::endl;

        CAircraftModelList swiftModels;
        timer.start();
        swiftModels.convertFromJson(swiftJsonObject);
        ms = timer.elapsed();
        out << "Read via swift JSON format: " << swiftModels.size() << " models in " << ms << "ms" << Qt::endl;

        Q_ASSERT_X(swiftModels.size() == dbModels.size(), Q_FUNC_INFO, "Mismatching container size");
        return EXIT_SUCCESS;
    }

    int CSamplesPerformance::samplesStringUtilsVsRegEx(QTextStream &out)
    {
        QElapsedTimer timer;
        static const QString chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~_-=+!\"@#$%^&*()[]{} \t;:\\/?,.<>";
        QStringList strings;
        std::generate_n(std::back_inserter(strings), 100000, []
        {
            QString s;
            std::generate_n(std::back_inserter(s), 10, [] { return chars[CMathUtils::randomInteger(0, chars.size() - 1)]; });
            return s;
        });
        QString bigString = strings.join("\n");

        QRegularExpression upperRegex("[A-Z]");
        upperRegex.optimize();

        timer.start();
        for (const QString &s : std::as_const(strings))
        {
            auto c = containsChar(s, [](QChar c) { return c.isUpper(); });
            Q_UNUSED(c);
        }
        out << "Check 100,000 strings for containing uppercase letter: (utility)     " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        for (const QString &s : std::as_const(strings))
        {
            auto c = s.contains(upperRegex);
            Q_UNUSED(c);
        }
        out << "Check 100,000 strings for containing uppercase letter: (regex)       " << timer.elapsed() << "ms" << endl << Qt::endl;

        timer.start();
        for (const QString &s : std::as_const(strings))
        {
            auto i = indexOfChar(s, [](QChar c) { return c.isUpper(); });
            Q_UNUSED(i);
        }
        out << "Check 100,000 strings for index of first uppercase letter: (utility) " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        for (const QString &s : std::as_const(strings))
        {
            auto i = s.indexOf(upperRegex);
            Q_UNUSED(i);
        }
        out << "Check 100,000 strings for index of first uppercase letter: (regex)   " << timer.elapsed() << "ms" << endl << Qt::endl;

        auto temp = strings;
        timer.start();
        for (QString &s : strings)
        {
            removeChars(s, [](QChar c) { return c.isUpper(); });
        }
        out << "Remove from 100,000 strings all uppercase letters: (utility)         " << timer.elapsed() << "ms" << Qt::endl;
        strings = temp;

        timer.start();
        for (QString &s : strings)
        {
            s.remove(upperRegex);
        }
        out << "Remove from 100,000 strings all uppercase letters: (regex)           " << timer.elapsed() << "ms" << endl << Qt::endl;

        timer.start();
        {
            auto lines = splitLines(bigString);
            Q_UNUSED(lines);
        }
        out << "Split 100,000 line string into list of lines: (QStringList)          " << timer.elapsed() << "ms" << Qt::endl;

        timer.start();
        {
            auto lines = splitLinesRefs(bigString);
            Q_UNUSED(lines);
        }
        out << "Split 100,000 line string into list of lines: (QList<QStringRef>)    " << timer.elapsed() << "ms" << Qt::endl;

        return EXIT_SUCCESS;
    }

    int CSamplesPerformance::samplesStringConcat(QTextStream &out)
    {
        const int loop = 250000;
        QString x;
        const QString x1 = "11-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x2 = "22-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x3 = "33-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x4 = "44-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x5 = "55-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x6 = "66-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x7 = "77-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x8 = "88-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString x9 = "99-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

        QElapsedTimer time;
        time.start();
        for (int i = 0; i < loop; i++)
        {
            x += "12-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        }
        out << "+= String " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x += QLatin1String("12-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        }
        out << "+= QLatin1String " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x += QStringLiteral("12-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        }
        out << "+= QStringLiteral " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9;
        }
        out << "+ String multiple " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = x1 % x2 % x3 % x4 % x5 % x6 % x7 % x8 % x9;
        }
        out << "% String multiple " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = x.append(x1).append(x2).append(x3).append(x4).append(x5).append(x6).append(x7).append(x8).append(x9);
        }
        out << "append String multiple " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        static const QString xArgString("%1 %2 %3 %4 %5 %6 %7 %8 %9");
        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = xArgString.arg(x1, x2, x3, x4, x5, x6, x7, x8, x9);
        }
        out << "arg String multiple " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = QStringLiteral("%1 %2 %3 %4 %5 %6 %7 %8 %9").arg(x1, x2, x3, x4, x5, x6, x7, x8, x9);
        }
        out << "arg QStringLiteral multiple " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        return EXIT_SUCCESS;
    }

    int CSamplesPerformance::samplesStringLiteralVsConstQString(QTextStream &out)
    {
        const int loop = 1e7;
        QElapsedTimer time;
        QString x;
        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = fooString();
        }
        out << "by constQString " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = fooStringLiteral();
        }
        out << "by QStringLiteral " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = QString("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer nec odio. Praesent libero. Sed cursus ante dapibus diam. Sed nisi.");
        }
        out << "by QString(\"...\") " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            x = QStringLiteral("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer nec odio. Praesent libero. Sed cursus ante dapibus diam. Sed nisi.");
        }
        out << "by QStringLiteral(\"...\") " << time.elapsed() << "ms" << Qt::endl;
        x.clear();

        time.start();
        for (int i = 0; i < loop; i++)
        {
            QStringList foo = generateList();
            Q_UNUSED(foo.size());
        }
        out << "generated list " << time.elapsed() << "ms" << Qt::endl;

        time.start();
        for (int i = 0; i < loop; i++)
        {
            QStringList foo = replacedList();
            Q_UNUSED(foo.size());
        }
        out << "replaced list " << time.elapsed() << "ms" << Qt::endl;

        return EXIT_SUCCESS;
    }

    int CSamplesPerformance::sampleQMapVsQHashByCallsign(QTextStream &out)
    {
        const CCallsignSet cs10 = CSamplesPerformance::callsigns(10);
        const CCallsignSet cs25 = CSamplesPerformance::callsigns(25);
        const CCallsignSet cs50 = CSamplesPerformance::callsigns(50);

        const QMap<CCallsign, CAircraftSituation> m10 = CSamplesPerformance::situationsMap(cs10);
        const QMap<CCallsign, CAircraftSituation> m25 = CSamplesPerformance::situationsMap(cs25);
        const QMap<CCallsign, CAircraftSituation> m50 = CSamplesPerformance::situationsMap(cs50);

        const QHash<CCallsign, CAircraftSituation> h10 = CSamplesPerformance::situationsHash(cs10);
        const QHash<CCallsign, CAircraftSituation> h25 = CSamplesPerformance::situationsHash(cs25);
        const QHash<CCallsign, CAircraftSituation> h50 = CSamplesPerformance::situationsHash(cs50);

        Q_ASSERT(m10.size() == 10 && h10.size() == 10);
        Q_ASSERT(m25.size() == 25 && h25.size() == 25);
        Q_ASSERT(m50.size() == 50 && h50.size() == 50);

        // QList since we have to add callsigns multiple times, set does not allow that
        QList<CCallsign> cs_10_100_rnd;
        QList<CCallsign> cs_25_100_rnd;
        QList<CCallsign> cs_50_100_rnd;

        for (int i = 0; i < 20; ++i)
        {
            cs_10_100_rnd.append(cs10.randomElements(5).toQList());
            cs_25_100_rnd.append(cs25.randomElements(5).toQList());
            cs_50_100_rnd.append(cs50.randomElements(5).toQList());
        }

        Q_ASSERT(cs_10_100_rnd.size() == 100);
        Q_ASSERT(cs_25_100_rnd.size() == 100);
        Q_ASSERT(cs_50_100_rnd.size() == 100);

        QElapsedTimer time;
        time.start();
        for (int i = 1; i < 10000; ++i)
        {
            for (const CCallsign &cs : cs_10_100_rnd)
            {
                CAircraftSituation s = m10[cs];
                Q_ASSERT_X(s.getCallsign() == cs, Q_FUNC_INFO, "Wromg callsign");
            }
        }
        out << "map 100 out of 10: " << time.elapsed() << "ms" << Qt::endl;

        time.start();
        for (int i = 1; i < 10000; ++i)
        {
            for (const CCallsign &cs : cs_10_100_rnd)
            {
                CAircraftSituation s = h10[cs];
                Q_ASSERT_X(s.getCallsign() == cs, Q_FUNC_INFO, "Wromg callsign");
            }
        }
        out << "hash 100 out of 10: " << time.elapsed() << "ms" << Qt::endl;

        time.start();
        for (int i = 1; i < 10000; ++i)
        {
            for (const CCallsign &cs : cs_25_100_rnd)
            {
                CAircraftSituation s = m25[cs];
                Q_ASSERT_X(s.getCallsign() == cs, Q_FUNC_INFO, "Wromg callsign");
            }
        }
        out << "map 100 out of 25: " << time.elapsed() << "ms" << Qt::endl;

        time.start();
        for (int i = 1; i < 10000; ++i)
        {
            for (const CCallsign &cs : cs_25_100_rnd)
            {
                CAircraftSituation s = h25[cs];
                Q_ASSERT_X(s.getCallsign() == cs, Q_FUNC_INFO, "Wromg callsign");
            }
        }
        out << "hash 100 out of 25: " << time.elapsed() << "ms" << Qt::endl;

        time.start();
        for (int i = 1; i < 10000; ++i)
        {
            for (const CCallsign &cs : cs_50_100_rnd)
            {
                CAircraftSituation s = m50[cs];
                Q_ASSERT_X(s.getCallsign() == cs, Q_FUNC_INFO, "Wromg callsign");
            }
        }
        out << "map 100 out of 50: " << time.elapsed() << "ms" << Qt::endl;

        time.start();
        for (int i = 1; i < 10000; ++i)
        {
            for (const CCallsign &cs : cs_50_100_rnd)
            {
                CAircraftSituation s = h50[cs];
                Q_ASSERT_X(s.getCallsign() == cs, Q_FUNC_INFO, "Wromg callsign");
            }
        }
        out << "hash 100 out of 50: " << time.elapsed() << "ms" << Qt::endl;

        return EXIT_SUCCESS;
    }

    CAircraftSituationList CSamplesPerformance::createSituations(qint64 baseTimeEpoch, int numberOfCallsigns, int numberOfTimes)
    {
        CAircraftSituationList situations;
        for (int cs = 0; cs < numberOfCallsigns; cs++)
        {
            CCallsign callsign("CS" + QString::number(cs));
            CCoordinateGeodetic coordinate(cs, cs, cs);
            for (int t = 0; t < numberOfTimes; t++)
            {
                CAircraftSituation s(callsign, coordinate);
                s.setMSecsSinceEpoch(baseTimeEpoch + DeltaTime * t);
                situations.push_back(s);
            }
        }
        return situations;
    }

    CAircraftModelList CSamplesPerformance::createModels(int numberOfModels, int numberOfMemoParts)
    {
        CAircraftIcaoCodeList aircraftIcaos;
        CLiveryList liveries;
        CDistributorList distributors;
        for (int i = 0; i < numberOfMemoParts; ++i)
        {
            aircraftIcaos.push_back(CAircraftIcaoCode("A" + QString::number(i), "A" + QString::number(i), "L1P", "Lego", "Foo", "M", false, false, false, 0));
            liveries.push_back(CLivery("A" + QString::number(i), CAirlineIcaoCode("A" + QString::number(i), "Foo", CCountry("DE", "Germany"), "Foo", false, false), "Foo", "red", "blue", false));
            distributors.push_back(CDistributor(QString::number(i), "Foo", {}, {}, CSimulatorInfo::FSX));
        }

        CAircraftModelList models;
        for (int i = 0; i < numberOfModels; ++i)
        {
            const auto &aircraftIcao = aircraftIcaos[CMathUtils::randomInteger(0, numberOfMemoParts - 1)];
            const auto &livery = liveries[CMathUtils::randomInteger(0, numberOfMemoParts - 1)];
            const auto &distributor = distributors[CMathUtils::randomInteger(0, numberOfMemoParts - 1)];
            models.push_back(CAircraftModel(QString::number(i), CAircraftModel::TypeUnknown, CSimulatorInfo::FSX, QString::number(i), QString::number(i), aircraftIcao, livery));
            models.back().setDistributor(distributor);
        }
        return models;
    }

    void CSamplesPerformance::calculateDistance(int n)
    {
        if (n < 1) { return; }
        CAtcStation atc = CTesting::createStation(1);
        const QList<CCoordinateGeodetic> pos(
        {
            CCoordinateGeodetic(10.0, 10.0, 10.0),
            CCoordinateGeodetic(20.0, 20.0, 20.0),
            CCoordinateGeodetic(30.0, 30.0, 30.0),
            CCoordinateGeodetic(40.0, 40.0, 40.0),
            CCoordinateGeodetic(50.0, 50.0, 50.0),
            CCoordinateGeodetic(60.0, 60.0, 60.0),
            CCoordinateGeodetic(70.0, 70.0, 70.0)
        }
        );
        const int s = pos.size();
        for (int i = 0; i < n; i++)
        {
            int p = i % s;
            atc.calculcateAndUpdateRelativeDistance(pos.at(p));
        }
    }

    void CSamplesPerformance::copy10kStations(int times)
    {
        CAtcStationList stations;
        for (int i = 0; i < times; i++)
        {
            stations = stations10k();
            stations.pop_back(); // make sure stations are really copied (copy-on-write)
        }
    }

    void CSamplesPerformance::parseWgs(int times)
    {
        static QStringList wgsLatLng(
        {
            "12° 11′ 10″ N", "11° 22′ 33″ W",
            "48° 21′ 13″ N", "11° 47′ 09″ E",
            " 8° 21′ 13″ N", "11° 47′ 09″ W",
            "18° 21′ 13″ S", "11° 47′ 09″ E",
            "09° 12′ 13″ S", "11° 47′ 09″ W"
        });

        CCoordinateGeodetic c;
        const CAltitude a(333, CLengthUnit::m());
        for (int i = 0; i < times; i++)
        {
            int idx = (i % 5) * 2;
            c = CCoordinateGeodetic::fromWgs84(wgsLatLng.at(idx), wgsLatLng.at(idx + 1), a);
        }
    }

    const QString &CSamplesPerformance::fooString()
    {
        static const QString s("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer nec odio. Praesent libero. Sed cursus ante dapibus diam. Sed nisi.");
        return s;
    }

    QString CSamplesPerformance::fooStringLiteral()
    {
        return QStringLiteral("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer nec odio. Praesent libero. Sed cursus ante dapibus diam. Sed nisi.");
    }

    QStringList CSamplesPerformance::generateList()
    {
        return QStringList({"1", "2", "3", "4"});
    }

    QStringList CSamplesPerformance::replacedList()
    {
        static const QStringList l({"1", "2", "3", "4"});
        QStringList lc(l);
        lc[1] = QStringLiteral("6");
        lc[3] = QStringLiteral("7");
        return lc;
    }

    CCallsignSet CSamplesPerformance::callsigns(int number)
    {
        CCallsignSet set;
        static const QString cs("FOO%1");
        for (int i = 0; i < number; i++)
        {
            set.insert(CCallsign(cs.arg(i)));
        }
        return set;
    }

    const QMap<CCallsign, CAircraftSituation> CSamplesPerformance::situationsMap(const CCallsignSet &callsigns)
    {
        QMap<CCallsign, CAircraftSituation> situations;
        for (const CCallsign &cs : callsigns)
        {
            const CAircraftSituation s(cs);
            situations.insert(cs, s);
        }
        return situations;
    }

    const QHash<CCallsign, CAircraftSituation> CSamplesPerformance::situationsHash(const CCallsignSet &callsigns)
    {
        QHash<CCallsign, CAircraftSituation> situations;
        for (const CCallsign &cs : callsigns)
        {
            const CAircraftSituation s(cs);
            situations.insert(cs, s);
        }
        return situations;
    }

    const CAtcStationList &CSamplesPerformance::stations10k()
    {
        static const CAtcStationList s = CTesting::createAtcStations(10000, false);
        return s;
    }

    void CSamplesPerformance::accessStationsData(const CAtcStationList &stations, bool byPropertyIndex)
    {
        for (const CAtcStation &station : stations)
        {
            const QString s = CSamplesPerformance::accessStationData(station, byPropertyIndex);
            Q_UNUSED(s);
        }
    }

    QString CSamplesPerformance::accessStationData(const CAtcStation &station, bool byPropertyIndex)
    {
        QString r;
        if (byPropertyIndex)
        {
            r.append(station.propertyByIndex(CPropertyIndex { CAtcStation::IndexCallsign, CCallsign::IndexString}).toString());
            r.append(station.propertyByIndex(CPropertyIndex { CAtcStation::IndexController, CUser::IndexRealName}).toString());
            r.append(station.propertyByIndex(CPropertyIndex { CAtcStation::IndexPosition, CCoordinateGeodetic::IndexLatitudeAsString}).toString());
            r.append(station.propertyByIndex(CPropertyIndex { CAtcStation::IndexPosition, CCoordinateGeodetic::IndexLongitudeAsString}).toString());
            r.append(station.propertyByIndex(CPropertyIndex { CAtcStation::IndexRelativeDistance, CLength::IndexValueRounded2DigitsWithUnit}).toString());
            r.append(station.propertyByIndex(CPropertyIndex { CAtcStation::IndexBookedFrom}).toDateTime().toString("yyyy-MM-dd hh:mm"));
            r.append(station.propertyByIndex(CPropertyIndex { CAtcStation::IndexBookedUntil}).toDateTime().toString("yyyy-MM-dd hh:mm"));
        }
        else
        {
            r.append(station.getCallsignAsString());
            r.append(station.getController().getRealName());
            r.append(station.getPosition().latitudeAsString());
            r.append(station.getPosition().longitudeAsString());
            r.append(station.getRelativeDistance().toQString(true));
            r.append(station.getBookedFromUtc().toString("yyyy-MM-dd hh:mm"));
            r.append(station.getBookedUntilUtc().toString("yyyy-MM-dd hh:mm"));
        }
        return r;
    }
} // namespace
