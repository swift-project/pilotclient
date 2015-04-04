/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testinterpolator.h"
#include "blackcore/interpolator_linear.h"
#include "blackmisc/simulation/remoteaircraftproviderdummy.h"
#include <QScopedPointer>

using namespace BlackCore;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackCoreTest
{

    void CTestInterpolator::linearInterpolator()
    {
        QScopedPointer<CRemoteAircraftProviderDummy> provider(new CRemoteAircraftProviderDummy());
        CInterpolatorLinear interpolator(provider.data());
        interpolator.forceSortingOfAddedValues(true);

        // fixed time so everything can be debugged
        const qint64 ts =  1425000000000; // QDateTime::currentMSecsSinceEpoch();
        const qint64 deltaT = 5000; // ms
        CCallsign cs("SWIFT");
        for (int i = 0; i < IInterpolator::MaxSituationsPerCallsign; i++)
        {
            CAircraftSituation s(getTestSituation(cs, i, ts, deltaT));

            // check height above ground
            CLength hag = (s.getAltitude() - s.geodeticHeight());
            QVERIFY2(s.getHeightAboveGround() == hag, "Wrong elevation");
            provider->insertNewSituation(s);
        }

        for (int i = 0; i < IInterpolator::MaxPartsPerCallsign; i++)
        {
            CAircraftParts p(getTestParts(cs, i, ts, deltaT));
            provider->insertNewAircraftParts(p);
        }

        // make sure signals are processed
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

        // check if all situations / parts have been received
        QVERIFY2(interpolator.getSituationsForCallsign(cs).size() == IInterpolator::MaxSituationsPerCallsign, "Missing situations");
        QVERIFY2(interpolator.getPartsForCallsign(cs).size() == IInterpolator::MaxPartsPerCallsign, "Missing parts");

        // interpolation
        IInterpolator::InterpolationStatus status;
        double latOld = 360.0;
        double lngOld = 360.0;
        for (qint64 currentTime = ts - 2 * deltaT; currentTime < ts; currentTime += 250)
        {
            // This will use time range
            // from:  ts - 2* deltaT - IInterpolator::TimeOffsetMs
            // to:    ts             - IInterpolator::TimeOffsetMs
            CAircraftSituation currentSituation(interpolator.getInterpolatedSituation
                                                (cs, currentTime, status)
                                               );
            QVERIFY2(status.interpolationSucceeded, "Interpolation was not succesful");
            QVERIFY2(status.changedPosition, "Interpolation did not changed");
            double latDeg = currentSituation.getPosition().latitude().valueRounded(CAngleUnit::deg(), 5);
            double lngDeg = currentSituation.getPosition().longitude().valueRounded(CAngleUnit::deg(), 5);
            QVERIFY2(latDeg < latOld && lngDeg < lngOld, "Values shall decrease");
            latOld = latDeg;
            lngOld = lngDeg;
        }

        QTime timer;
        timer.start();
        int interpolationNo = 0;
        qint64 startTimeMsSinceEpoch = ts - 2 * deltaT;

        for (int callsigns = 0; callsigns < 20; callsigns++)
        {
            for (qint64 currentTime = startTimeMsSinceEpoch; currentTime < ts; currentTime += 250)
            {
                // This will use range
                // from:  ts - 2* deltaT - IInterpolator::TimeOffsetMs
                // to:    ts             - IInterpolator::TimeOffsetMs
                CAircraftSituation currentSituation(interpolator.getInterpolatedSituation
                                                    (cs, currentTime, status)
                                                   );
                QVERIFY2(status.allTrue(), "Failed interpolation");
                QVERIFY2(currentSituation.getCallsign() == cs, "Wrong callsign");
                double latDeg = currentSituation.getPosition().latitude().valueRounded(CAngleUnit::deg(), 5);
                double lngDeg = currentSituation.getPosition().longitude().valueRounded(CAngleUnit::deg(), 5);
                Q_UNUSED(latDeg);
                Q_UNUSED(lngDeg);
                interpolationNo++;
            }
        }
        int timeMs = timer.elapsed();
        QVERIFY2(timeMs < interpolationNo, "Interpolation > 1ms");
        qDebug() << timeMs << "ms" << "for" << interpolationNo;

        CAircraftSituationList interpolations(interpolator.getInterpolatedSituations(startTimeMsSinceEpoch));
        QVERIFY(interpolations.size() == 1);
        QVERIFY(interpolations.containsCallsign(cs));

        //
        // Single interpolation vs. all interpolations at once
        //

        // Create data in provider
        provider->clear();
        interpolator.clear();
        QVERIFY(interpolator.getSituationsByCallsign().size() == 0);
        const int callsignsInProvider = 20;
        for (int callsignNo = 0; callsignNo < callsignsInProvider; callsignNo++)
        {
            cs = CCallsign("SWIFT" + QString::number(callsignNo));
            int i = 0;
            for (int t = 0; t < IInterpolator::MaxSituationsPerCallsign; t++)
            {
                qint64 currentTime = ts - t * deltaT;
                CAircraftSituation s(getTestSituation(cs, i, currentTime, 0));
                provider->insertNewSituation(s);

                CAircraftParts p(getTestParts(cs, i, currentTime, 0));
                provider->insertNewAircraftParts(p);
                i++;
            }
        }

        QList<CCallsign> csKeys = interpolator.getSituationsByCallsign().keys();
        CCallsignSet callsigns(csKeys);
        QVERIFY(callsigns.size() == callsignsInProvider);
        QVERIFY(interpolator.getSituationsForCallsign("SWIFT0").size() == IInterpolator::MaxSituationsPerCallsign);
        QVERIFY(interpolator.getPartsForCallsign("SWIFT0").size() == IInterpolator::MaxPartsPerCallsign);

        // interpolation for time, then for each callsign
        int doneInterpolations = 0;
        timer.start();
        for (qint64 currentTime = ts - 2 * deltaT; currentTime < ts; currentTime += 250)
        {
            // This will use range
            // from:  ts - 2* deltaT - IInterpolator::TimeOffsetMs
            // to:    ts             - IInterpolator::TimeOffsetMs

            for (const CCallsign &cs : callsigns)
            {
                CAircraftSituation currentSituation(interpolator.getInterpolatedSituation
                                                    (cs, currentTime, status)
                                                   );
                QVERIFY2(status.interpolationSucceeded, "Interpolation was not succesful");
                QVERIFY2(status.changedPosition, "Interpolation did not changed");
                double latDeg = currentSituation.getPosition().latitude().valueRounded(CAngleUnit::deg(), 5);
                double lngDeg = currentSituation.getPosition().longitude().valueRounded(CAngleUnit::deg(), 5);
                Q_UNUSED(latDeg);
                Q_UNUSED(lngDeg);
                doneInterpolations++;
            }
        }
        timeMs = timer.elapsed();
        qDebug() << "Per callsign" << doneInterpolations << "interpolations in" << timeMs << "ms";

        doneInterpolations = 0;
        timer.start();
        for (qint64 currentTime = ts - 2 * deltaT; currentTime < ts; currentTime += 250)
        {
            // This will use range
            // from:  ts - 2* deltaT - IInterpolator::TimeOffsetMs
            // to:    ts             - IInterpolator::TimeOffsetMs

            CAircraftSituationList currentSituations(interpolator.getInterpolatedSituations(currentTime));
            QVERIFY2(currentSituations.size() == callsignsInProvider, "Interpolation was not succesful");
            for (const CAircraftSituation &currentSituation : currentSituations)
            {
                double latDeg = currentSituation.getPosition().latitude().valueRounded(CAngleUnit::deg(), 5);
                double lngDeg = currentSituation.getPosition().longitude().valueRounded(CAngleUnit::deg(), 5);
                doneInterpolations++;
                Q_UNUSED(latDeg);
                Q_UNUSED(lngDeg);
            }
        }
        timeMs = timer.elapsed();
        qDebug() << "All callsigns" << doneInterpolations << "interpolations in" << timeMs << "ms";

        int fetchedParts = 0;
        timer.start();
        for (qint64 currentTime = ts - 2 * deltaT; currentTime < ts; currentTime += 250)
        {
            for (const CCallsign &callsign : callsigns)
            {
                IInterpolator::PartsStatus status;
                CAircraftPartsList pl = interpolator.getAndRemovePartsBeforeTime(callsign, ts, status);
                fetchedParts++;
                Q_UNUSED(pl);
            }
        }
        timeMs = timer.elapsed();
        qDebug() << "Per callsign" << fetchedParts << "fetched parts in" << timeMs << "ms";
    }

    CAircraftSituation CTestInterpolator::getTestSituation(const CCallsign &callsign, int number, qint64 ts, qint64 deltaT)
    {
        CAltitude a(number, CAltitude::MeanSeaLevel, CLengthUnit::m());
        CLatitude lat(number, CAngleUnit::deg());
        CLongitude lng(180.0 + number, CAngleUnit::deg());
        CLength height(0, CLengthUnit::m());
        CHeading heading(number * 10, CHeading::True, CAngleUnit::deg());
        CAngle bank(number, CAngleUnit::deg());
        CAngle pitch(number, CAngleUnit::deg());
        CSpeed gs(number * 10, CSpeedUnit::km_h());
        CCoordinateGeodetic c(lat, lng, height);
        CAircraftSituation s(callsign, c, a, heading, pitch, bank, gs);
        s.setMSecsSinceEpoch(ts - deltaT * number); // values in past
        return s;
    }

    CAircraftParts CTestInterpolator::getTestParts(const CCallsign &callsign, int number, qint64 ts, qint64 deltaT)
    {
        CAircraftLights l(true, false, true, false, true, false);
        CAircraftEngineList e({ CAircraftEngine(1, true), CAircraftEngine(2, false), CAircraftEngine(3, true) });
        CAircraftParts p(callsign, l, true, 20, true, e, false);
        p.setMSecsSinceEpoch(ts - deltaT * number); // values in past
        return p;
    }

} // namespace
