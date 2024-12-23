// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QScopedPointer>
#include <QTest>
#include <QTime>
#include <QtDebug>

#include "test.h"

#include "misc/aviation/aircraftengine.h"
#include "misc/aviation/aircraftenginelist.h"
#include "misc/aviation/aircraftlights.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/heading.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/mixin/mixincompare.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/speed.h"
#include "misc/pq/units.h"
#include "misc/simulation/interpolation/interpolator.h"
#include "misc/simulation/interpolation/interpolatorlinear.h"
#include "misc/simulation/remoteaircraftproviderdummy.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace MiscTest
{
    //! Interpolator classes basic tests
    class CTestInterpolatorLinear : public QObject
    {
        Q_OBJECT

    private slots:
        //! Basic unit tests for interpolator
        void basicInterpolatorTests();

    private:
        //! Test situation for testing
        static swift::misc::aviation::CAircraftSituation
        getTestSituation(const swift::misc::aviation::CCallsign &callsign, int number, qint64 ts, qint64 deltaT,
                         qint64 offset);

        //! Test parts
        static swift::misc::aviation::CAircraftParts getTestParts(int number, qint64 ts, qint64 deltaT);
    };

    void CTestInterpolatorLinear::basicInterpolatorTests()
    {
        const CCallsign cs("SWIFT");
        CRemoteAircraftProviderDummy provider;
        CInterpolatorLinear interpolator(cs, nullptr, nullptr, &provider);
        interpolator.markAsUnitTest();

        // fixed time so everything can be debugged
        const qint64 ts = 1425000000000; // QDateTime::currentMSecsSinceEpoch();
        const qint64 deltaT = 5000; // ms
        const qint64 offset = 5000; // ms
        for (int i = IRemoteAircraftProvider::MaxSituationsPerCallsign - 1; i >= 0; i--)
        {
            const CAircraftSituation s(getTestSituation(cs, i, ts, deltaT, offset));

            // check height above ground
            CLength hag = (s.getAltitude() - s.getGroundElevation());
            QVERIFY2(s.getHeightAboveGround() == hag, "Wrong elevation");
            provider.insertNewSituation(s);
        }

        constexpr int partsCount = 10;
        for (int i = partsCount - 1; i >= 0; i--)
        {
            const CAircraftParts p(getTestParts(i, ts, deltaT));
            provider.insertNewAircraftParts(cs, p, false);
        }

        // make sure signals are processed, if the interpolator depends on those signals
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

        // interpolation functional check
        const CInterpolationAndRenderingSetupPerCallsign setup;
        double latOld = 360.0;
        double lngOld = 360.0;

        qint64 from = ts - 2 * deltaT + offset;
        qint64 to = ts; // ts + offset is last value, but we have to consider offset
        qint64 step = deltaT / 20;
        for (qint64 currentTime = from; currentTime < to; currentTime += step)
        {
            // This will use time range
            // from:  ts - 2 * deltaT + offset
            // to:    ts              + offset

            const CInterpolationResult result = interpolator.getInterpolation(currentTime, setup, 0);
            const CAircraftSituation currentSituation(result);
            QVERIFY2(result.getInterpolationStatus().isInterpolated(), "Value was not interpolated");
            const double latDeg = currentSituation.getPosition().latitude().valueRounded(CAngleUnit::deg(), 5);
            const double lngDeg = currentSituation.getPosition().longitude().valueRounded(CAngleUnit::deg(), 5);
            QVERIFY2(latDeg < latOld && lngDeg < lngOld, QStringLiteral("Values shall decrease: %1/%2 %3/%4")
                                                             .arg(latDeg)
                                                             .arg(latOld)
                                                             .arg(lngDeg)
                                                             .arg(lngOld)
                                                             .toLatin1());
            QVERIFY2(latDeg >= 0 && latDeg <= IRemoteAircraftProvider::MaxSituationsPerCallsign,
                     "Values shall be in range");
            latOld = latDeg;
            lngOld = lngDeg;
        }

        QElapsedTimer timer;
        timer.start();
        int interpolationNo = 0;
        const qint64 startTimeMsSinceEpoch = ts - 2 * deltaT;

        // Pseudo performance test:
        // Those make not completely sense, as the performance depends on the implementation of
        // the dummy provider, which is different from the real provider
        // With one callsign in the lists (of dummy provider) it is somehow expected to be roughly the same performance

        interpolator.resetLastInterpolation();
        interpolator.markAsUnitTest();
        for (int loops = 0; loops < 20; loops++)
        {
            from = startTimeMsSinceEpoch + offset;
            to = ts; // ts + offset is last value, but we have to consider offset
            step = deltaT / 20;

            for (qint64 currentTime = from; currentTime < to; currentTime += step)
            {
                // This will use range
                // from:  ts - 2* deltaT + offset
                // to:    ts             + offset
                const CInterpolationResult result = interpolator.getInterpolation(currentTime, setup, 0);
                const CAircraftSituation currentSituation(result);
                QVERIFY2(result.getInterpolationStatus().isInterpolated(), "Not interpolated");
                QVERIFY2(!currentSituation.getCallsign().isEmpty(), "Empty callsign");
                QVERIFY2(currentSituation.getCallsign() == cs, "Wrong callsign");
                const double latDeg = currentSituation.getPosition().latitude().valueRounded(CAngleUnit::deg(), 5);
                const double lngDeg = currentSituation.getPosition().longitude().valueRounded(CAngleUnit::deg(), 5);
                Q_UNUSED(latDeg);
                Q_UNUSED(lngDeg);
                interpolationNo++;
            }
        }

        // check on time just to learn if interpolation suddenly gets very slow
        // this is a risky test as in some situations the values can be exceeded
        int timeMs = timer.elapsed();
        QVERIFY2(timeMs < interpolationNo * 1.5, "Interpolation > 1.5ms");
        qDebug() << timeMs << "ms"
                 << "for" << interpolationNo << "interpolations";

        int fetchedParts = 0;
        timer.start();
        for (qint64 currentTime = ts - 2 * deltaT; currentTime < ts; currentTime += 250)
        {
            const CInterpolationResult result = interpolator.getInterpolation(currentTime, setup, 0);
            fetchedParts++;
            QVERIFY2(result.getPartsStatus().isSupportingParts(), "Parts not supported");
        }
        timeMs = timer.elapsed();
        qDebug() << timeMs << "ms"
                 << "for" << fetchedParts << "fetched parts";
    }

    CAircraftSituation CTestInterpolatorLinear::getTestSituation(const CCallsign &callsign, int number, qint64 ts,
                                                                 qint64 deltaT, qint64 offset)
    {
        const CAltitude alt(number, CAltitude::MeanSeaLevel, CLengthUnit::m());
        const CLatitude lat(number, CAngleUnit::deg());
        const CLongitude lng(180.0 + number, CAngleUnit::deg());
        const CHeading heading(number * 10, CHeading::True, CAngleUnit::deg());
        const CAngle bank(number, CAngleUnit::deg());
        const CAngle pitch(number, CAngleUnit::deg());
        const CSpeed gs(number * 10, CSpeedUnit::km_h());
        const CAltitude gndElev({ 0, CLengthUnit::m() }, CAltitude::MeanSeaLevel);
        const CCoordinateGeodetic c(lat, lng, alt);
        CAircraftSituation s(callsign, c, heading, pitch, bank, gs);
        s.setGroundElevation(gndElev, CAircraftSituation::Test);
        s.setMSecsSinceEpoch(ts - deltaT * number); // values in past
        s.setTimeOffsetMs(offset);
        return s;
    }

    CAircraftParts CTestInterpolatorLinear::getTestParts(int number, qint64 ts, qint64 deltaT)
    {
        CAircraftLights l(true, false, true, false, true, false);
        CAircraftEngineList e({ CAircraftEngine(1, true), CAircraftEngine(2, false), CAircraftEngine(3, true) });
        CAircraftParts p(l, true, 20, true, e, false);
        p.setMSecsSinceEpoch(ts - deltaT * number); // values in past
        return p;
    }
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestInterpolatorLinear);

#include "testinterpolatorlinear.moc"

//! \endcond
