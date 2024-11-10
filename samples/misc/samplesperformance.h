// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#ifndef SWIFT_SAMPLE_SAMPLESPERFORMANCE_H
#define SWIFT_SAMPLE_SAMPLESPERFORMANCE_H

#include "misc/simulation/aircraftmodellist.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include <QTextStream>
#include <QtGlobal>
#include <QMap>
#include <QHash>

class QTextStream;

namespace swift::sample
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
        static int samplesJsonModelAndLivery(QTextStream &out);

        //! String manipulation (inserter)
        static int samplesStringUtilsVsRegEx(QTextStream &out);

        //! String manipulation (concatenation)
        static int samplesStringConcat(QTextStream &out);

        //! const QString vs. QStringLiteral
        static int samplesStringLiteralVsConstQString(QTextStream &out);

        //! Callsign based hash/map comparison
        static int sampleQMapVsQHashByCallsign(QTextStream &out);

    private:
        static const qint64 DeltaTime = 10;

        //! Situation values for testing
        static swift::misc::aviation::CAircraftSituationList createSituations(qint64 baseTimeEpoch, int numberOfCallsigns, int numberOfTimes);

        //! Model values for testing
        static swift::misc::simulation::CAircraftModelList createModels(int numberOfModels, int numberOfMemoParts);

        //! Calculate n times distance (greater circle distance)
        static void calculateDistance(int n);

        //! Copy 10k stations n times
        static void copy10kStations(int times);

        //! Const 10000 stations
        static const swift::misc::aviation::CAtcStationList &stations10k();

        //! Access properties of given stations
        static void accessStationsData(const swift::misc::aviation::CAtcStationList &stations, bool byPropertyIndex = false);

        //! Read properties of a station and concatenate them
        static QString accessStationData(const swift::misc::aviation::CAtcStation &station, bool byPropertyIndex = false);

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

        //! Get n callsigns
        static swift::misc::aviation::CCallsignSet callsigns(int number);

        //! Situations map
        static QMap<swift::misc::aviation::CCallsign, swift::misc::aviation::CAircraftSituation> situationsMap(const swift::misc::aviation::CCallsignSet &callsigns);

        //! Situations hash
        static QHash<swift::misc::aviation::CCallsign, swift::misc::aviation::CAircraftSituation> situationsHash(const swift::misc::aviation::CCallsignSet &callsigns);
    };
} // namespace

#endif
