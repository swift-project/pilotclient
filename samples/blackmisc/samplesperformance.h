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

#ifndef BLACKSAMPLE_SAMPLESPERFORMANCE_H
#define BLACKSAMPLE_SAMPLESPERFORMANCE_H

#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include <QTextStream>
#include <QtGlobal>

class QTextStream;

namespace BlackSample
{
    //! Samples for our containers
    class CSamplesPerformance
    {
    public:
        //! Copy, create, RegEx
        static int samplesMisc(QTextStream &out);

        //! Interpolator scenario
        static int interpolatorScenario(QTextStream &out, int numberOfCallsigns, int numberOfTimes);

        //! JSON (de)serialization
        static int samplesJson(QTextStream &out);

        //! JSON loading (database vs. own format
        static int samplesJsonModel(QTextStream &out);

        //! String manipulation (inserter)
        static int samplesStringUtilsVsRegEx(QTextStream &out);

        //! String manipulation (concatenation)
        static int samplesStringConcat(QTextStream &out);

        //! const QString vs. QStringLiteral
        static int samplesStringLiteralVsConstQString(QTextStream &out);

    private:
        static const qint64 DeltaTime = 10;

        //! Situation values for testing
        static BlackMisc::Aviation::CAircraftSituationList createSituations(qint64 baseTimeEpoch, int numberOfCallsigns, int numberOfTimes);

        //! Model values for testing
        static BlackMisc::Simulation::CAircraftModelList createModels(int numberOfModels, int numberOfMemoParts);

        //! Calculate n times distance (greater circle distance)
        static void calculateDistance(int n);

        //! Copy 10k stations n times
        static void copy10kStations(int times);

        //! Const 10000 stations
        static const BlackMisc::Aviation::CAtcStationList &stations10k();

        //! Access properties of given stations
        static void accessStationsData(const BlackMisc::Aviation::CAtcStationList &stations, bool byPropertyIndex = false);

        //! Read properties of a station and concatenate them
        static QString accessStationData(const BlackMisc::Aviation::CAtcStation &station, bool byPropertyIndex = false);

        //! parse coordinates from WGS
        static void parseWgs(int times);

        //! String as reference
        static const QString &fooString();

        //! String as reference
        static QString fooStringLiteral();

        //! String list generated
        static QStringList generateList();

        //! String list generated
        static QStringList replacedList();
    };
} // namespace

#endif
