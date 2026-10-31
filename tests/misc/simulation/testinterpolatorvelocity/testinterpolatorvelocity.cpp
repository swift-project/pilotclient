// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <cmath>
#include <limits>

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QTest>

#include "test.h"

#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/aircraftvelocity.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/heading.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/speed.h"
#include "misc/pq/time.h"
#include "misc/simulation/interpolation/interpolatorvelocity.h"
#include "misc/simulation/remoteaircraftproviderdummy.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace MiscTest
{
    //! Velocity extrapolating interpolator tests, situations streamed in like live network updates
    class CTestInterpolatorVelocity : public QObject
    {
        Q_OBJECT

    private slots:
        //! Extrapolation of a steady flight track
        void steadyFlightExtrapolation();

        //! A displaced fix is converged smoothly, no position snap
        void displacedFixIsConvergedWithoutSnap();

        //! Without velocity data the interpolator falls back to linear interpolation
        void fallbackWithoutVelocity();

    private:
        //! Situation of an aircraft flying north with constant velocity
        static CAircraftSituation northFlightSituation(const CCallsign &cs, int fixNo, qint64 tsBase, qint64 offsetMs,
                                                       qint64 fixIntervalMs, double lonDisplacementDeg);

        //! Ideal dead reckoned latitude [deg] at real time
        static double idealLatitude(double lat0Deg, double realTimeMs, double tsBaseMs);

        static constexpr double c_northVelocityM_s = 100.0;
        static constexpr qint64 c_fixIntervalMs = 200;
        static constexpr double c_mPerDegLat = 6335439.3; // WGS84 meridian radius at equator
        static constexpr double c_degToRad = 0.017453292519943295;
        static constexpr double c_radToDeg = 57.295779513082323;
    };

    CAircraftSituation CTestInterpolatorVelocity::northFlightSituation(const CCallsign &cs, int fixNo, qint64 tsBase,
                                                                       qint64 offsetMs, qint64 fixIntervalMs,
                                                                       double lonDisplacementDeg)
    {
        const double dLatDegPerFix = c_northVelocityM_s * (fixIntervalMs / 1000.0) / c_mPerDegLat * c_radToDeg;
        const CAircraftVelocity velocity(0, 0, c_northVelocityM_s, CSpeedUnit::m_s(), 0, 0, 0, CAngleUnit::rad(),
                                         CTimeUnit::s());
        const CAltitude altitude(10000.0, CAltitude::MeanSeaLevel, CLengthUnit::m());
        const CAltitude gndElevation({ 0.0, CLengthUnit::m() }, CAltitude::MeanSeaLevel);
        const CCoordinateGeodetic pos(CLatitude(dLatDegPerFix * fixNo, CAngleUnit::deg()),
                                      CLongitude(lonDisplacementDeg, CAngleUnit::deg()), altitude);
        CAircraftSituation s(cs, pos, CHeading(0, CHeading::True, CAngleUnit::deg()), CAngle(0, CAngleUnit::deg()),
                             CAngle(0, CAngleUnit::deg()), CSpeed(c_northVelocityM_s, CSpeedUnit::m_s()));
        s.setGroundElevation(gndElevation, CAircraftSituation::Test);
        s.setVelocity(velocity);
        s.setMSecsSinceEpoch(tsBase + fixIntervalMs * fixNo);
        s.setTimeOffsetMs(offsetMs);
        return s;
    }

    double CTestInterpolatorVelocity::idealLatitude(double lat0Deg, double realTimeMs, double tsBaseMs)
    {
        const double traveledM = c_northVelocityM_s * (realTimeMs - tsBaseMs) / 1000.0;
        return lat0Deg + traveledM / c_mPerDegLat * c_radToDeg;
    }

    void CTestInterpolatorVelocity::steadyFlightExtrapolation()
    {
        const CCallsign cs("VELTEST");
        CRemoteAircraftProviderDummy provider;
        CInterpolatorVelocity interpolator(cs, nullptr, nullptr, &provider);
        interpolator.markAsUnitTest();

        CInterpolationAndRenderingSetupPerCallsign setup;
        setup.setInterpolatorMode(CInterpolationAndRenderingSetupBase::Velocity);

        const qint64 tsBase = 1425000000000;
        const qint64 offsetMs = 500;
        const int fixCount = 40;
        const double lat0Deg = 0.0;

        // stream the fixes in like live network updates, render in between
        double lastLatDeg = -1.0;
        double lastLonDeg = -1.0;
        qint64 lastTMs = -1;
        for (int i = 0; i < fixCount; i++)
        {
            provider.insertNewSituation(northFlightSituation(cs, i, tsBase, offsetMs, c_fixIntervalMs, 0.0));
            QTest::qWait(2); // advance wall clock so the provider's last-modified stamp moves forward

            const qint64 tFix = tsBase + i * c_fixIntervalMs + offsetMs; // adjusted time of this fix
            const qint64 tInter = tFix + c_fixIntervalMs / 4; // in between this and the next fix
            for (qint64 t : { tFix, tInter })
            {
                const CInterpolationResult result = interpolator.getInterpolation(t, setup, 0);
                QVERIFY2(result.getInterpolationStatus().isInterpolated(), "Not interpolated");
                const CAircraftSituation s(result);
                const double latDeg = s.latitude().value(CAngleUnit::deg());
                const double lonDeg = s.longitude().value(CAngleUnit::deg());

                // no jumps, movement shall match the velocity over the render interval (+ margin)
                if (lastLatDeg >= 0.0)
                {
                    const double stepM = std::fabs(latDeg - lastLatDeg) * c_mPerDegLat / c_radToDeg;
                    const double maxStepM = c_northVelocityM_s * (t - lastTMs) / 1000.0 + 2.0;
                    QVERIFY2(stepM < maxStepM,
                             QStringLiteral("Position jump %1m, max %2m").arg(stepM).arg(maxStepM).toLatin1());
                    QVERIFY2(latDeg > lastLatDeg, QStringLiteral("Shall move north, lat %1 last %2 (t %3)")
                                                      .arg(latDeg, 0, 'f', 8)
                                                      .arg(lastLatDeg, 0, 'f', 8)
                                                      .arg(t)
                                                      .toLatin1());
                    QVERIFY2(std::fabs(lonDeg - lastLonDeg) < 1e-9, "Shall not drift east/west");
                }

                // dead reckoning accuracy
                const double ideal = idealLatitude(lat0Deg, t - offsetMs, tsBase);
                QVERIFY2(std::fabs(latDeg - ideal) < 1e-4,
                         QStringLiteral("DR error %1deg").arg(latDeg - ideal).toLatin1());

                lastLatDeg = latDeg;
                lastLonDeg = lonDeg;
                lastTMs = t;
            }
        }

        // auto mode: with spline configured, velocity data available => extrapolation shall be used
        {
            CInterpolationAndRenderingSetupPerCallsign splineSetup;
            splineSetup.setInterpolatorMode(CInterpolationAndRenderingSetupBase::Spline);
            QVERIFY2(interpolator.hasVelocityData(), "Velocity data shall be detected");
            const qint64 t = tsBase + (fixCount - 1) * c_fixIntervalMs + offsetMs + 100;
            const CInterpolationResult result = interpolator.getInterpolation(t, splineSetup, 0);
            QVERIFY2(result.getInterpolationStatus().isInterpolated(), "Not interpolated");
            const CAircraftSituation s(result);
            const double ideal = idealLatitude(lat0Deg, t - offsetMs, tsBase);
            QVERIFY2(std::fabs(s.latitude().value(CAngleUnit::deg()) - ideal) < 1e-4,
                     "Spline setup shall auto-select velocity extrapolation");
        }
    }

    void CTestInterpolatorVelocity::displacedFixIsConvergedWithoutSnap()
    {
        const CCallsign cs("VELSNAP");
        CRemoteAircraftProviderDummy provider;
        CInterpolatorVelocity interpolator(cs, nullptr, nullptr, &provider);
        interpolator.markAsUnitTest();

        CInterpolationAndRenderingSetupPerCallsign setup;
        setup.setInterpolatorMode(CInterpolationAndRenderingSetupBase::Velocity);

        const qint64 tsBase = 1425000000000;
        const qint64 offsetMs = 500;
        const int fixCount = 60;
        const int displacedFix = 30;
        const double displacementDeg = 0.001; // ~111m east

        // stream the fixes in, fix #displacedFix displaced eastbound
        double lastLatDeg = -1.0;
        double lastLonDeg = -999.0;
        const double mPerDegLon = c_mPerDegLat * c_degToRad; // at equator ~111.3km
        for (int i = 0; i < fixCount; i++)
        {
            const double lon = (i == displacedFix) ? displacementDeg : 0.0;
            provider.insertNewSituation(northFlightSituation(cs, i, tsBase, offsetMs, c_fixIntervalMs, lon));
            QTest::qWait(2); // advance wall clock so the provider's last-modified stamp moves forward

            const qint64 tFix = tsBase + i * c_fixIntervalMs + offsetMs;
            const qint64 tInter = tFix + c_fixIntervalMs / 2;
            for (qint64 t : { tFix, tInter })
            {
                const CInterpolationResult result = interpolator.getInterpolation(t, setup, 0);
                QVERIFY2(result.getInterpolationStatus().isInterpolated(), "Not interpolated");
                const CAircraftSituation s(result);
                const double latDeg = s.latitude().value(CAngleUnit::deg());
                const double lonDeg = s.longitude().value(CAngleUnit::deg());

                if (lastLatDeg >= 0.0)
                {
                    // 10m per 100ms from velocity, corrections glide with bounded rate
                    const double stepM = std::fabs(latDeg - lastLatDeg) * c_mPerDegLat / c_radToDeg;
                    QVERIFY2(stepM < 40.0, QStringLiteral("Position jump %1m").arg(stepM).toLatin1());
                }
                if (lastLonDeg > -900.0)
                {
                    // the displacement is eastbound, a snap would show up here
                    const double stepEastM = std::fabs(lonDeg - lastLonDeg) * mPerDegLon;
                    QVERIFY2(stepEastM < 20.0, QStringLiteral("Eastbound jump %1m").arg(stepEastM).toLatin1());
                }

                // right after the displaced fix: position shall NOT snap to it
                if (i == displacedFix && t == tFix)
                {
                    QVERIFY2(std::fabs(lonDeg - displacementDeg) > displacementDeg / 2.0,
                             QStringLiteral("Snapped to displaced fix: %1").arg(lonDeg).toLatin1());
                }

                lastLatDeg = latDeg;
                lastLonDeg = lonDeg;
            }
        }

        // a few seconds after the displaced fix the track is back on the ideal path
        const qint64 tBack = tsBase + (displacedFix + 15) * c_fixIntervalMs + offsetMs;
        const CInterpolationResult result = interpolator.getInterpolation(tBack, setup, 0);
        QVERIFY2(result.getInterpolationStatus().isInterpolated(), "Not interpolated");
        const CAircraftSituation s(result);
        QVERIFY2(
            std::fabs(s.longitude().value(CAngleUnit::deg())) < 1e-4,
            QStringLiteral("Not converged back, lon err %1").arg(s.longitude().value(CAngleUnit::deg())).toLatin1());
        const double ideal = idealLatitude(0.0, tBack - offsetMs, tsBase);
        QVERIFY2(std::fabs(s.latitude().value(CAngleUnit::deg()) - ideal) < 1e-4, "Not back on ideal track");
    }

    void CTestInterpolatorVelocity::fallbackWithoutVelocity()
    {
        const CCallsign cs("VELFALLBACK");
        CRemoteAircraftProviderDummy provider;
        CInterpolatorVelocity interpolator(cs, nullptr, nullptr, &provider);
        interpolator.markAsUnitTest();

        CInterpolationAndRenderingSetupPerCallsign setup;
        setup.setInterpolatorMode(CInterpolationAndRenderingSetupBase::Velocity);

        // slow track without any velocity information (like plain FSD position updates)
        const qint64 tsBase = 1425000000000;
        const qint64 offsetMs = 500;
        const int fixCount = 20;
        const qint64 fixIntervalMs = 5000;
        const double dLatDegPerFix = 20.0 / c_mPerDegLat * c_radToDeg;
        const CAltitude altitude(10000.0, CAltitude::MeanSeaLevel, CLengthUnit::m());
        const CAltitude gndElevation({ 0.0, CLengthUnit::m() }, CAltitude::MeanSeaLevel);
        for (int i = 0; i < fixCount; i++)
        {
            const CCoordinateGeodetic pos(CLatitude(dLatDegPerFix * i, CAngleUnit::deg()),
                                          CLongitude(0.0, CAngleUnit::deg()), altitude);
            CAircraftSituation s(cs, pos, CHeading(0, CHeading::True, CAngleUnit::deg()), CAngle(0, CAngleUnit::deg()),
                                 CAngle(0, CAngleUnit::deg()), CSpeed(20.0, CSpeedUnit::m_s()));
            s.setGroundElevation(gndElevation, CAircraftSituation::Test);
            s.setMSecsSinceEpoch(tsBase + fixIntervalMs * i);
            s.setTimeOffsetMs(offsetMs);
            provider.insertNewSituation(s);
        }
        QTest::qWait(2); // advance wall clock so the provider's last-modified stamp moves forward

        // interpolate between two fixes, shall yield in-between latitude (linear fallback)
        const qint64 t0 = tsBase + 5 * fixIntervalMs + offsetMs;
        const qint64 t1 = tsBase + 6 * fixIntervalMs + offsetMs;
        const auto latAt = [&](qint64 t) -> double {
            const CInterpolationResult r = interpolator.getInterpolation(t, setup, 0);
            if (!r.getInterpolationStatus().isInterpolated()) { return std::numeric_limits<double>::quiet_NaN(); }
            return static_cast<const CAircraftSituation &>(r).latitude().value(CAngleUnit::deg());
        };
        const double latAtT0 = latAt(t0);
        const double latAtT1 = latAt(t1);
        const double latMid = latAt(t0 + fixIntervalMs / 2);
        QVERIFY2(!std::isnan(latAtT0) && !std::isnan(latAtT1) && !std::isnan(latMid), "Fallback not interpolated");
        const double expectedMid = latAtT0 + (latAtT1 - latAtT0) / 2.0;
        QVERIFY2(
            std::fabs(latMid - expectedMid) < 1e-9,
            QStringLiteral("Fallback shall interpolate linearly, %1 vs %2").arg(latMid).arg(expectedMid).toLatin1());
    }
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestInterpolatorVelocity);

#include "testinterpolatorvelocity.moc"

//! \endcond
