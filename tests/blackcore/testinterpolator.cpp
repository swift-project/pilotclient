/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testinterpolator.h"
#include "blackcore/interpolator_linear.h"
#include "blackmisc/simulation/simdirectaccessremoteaircraftdummy.h"
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

        const CCallsign cs("SWIFT");
        const qint64 ts = QDateTime::currentMSecsSinceEpoch();
        const qint64 deltaT = 5000; // ms
        for (int i = 0; i < IInterpolator::MaxSituationsPerCallsign; i++)
        {
            CAltitude a(i, CAltitude::MeanSeaLevel, CLengthUnit::m());
            CLatitude lat(i, CAngleUnit::deg());
            CLongitude lng(180.0 + i, CAngleUnit::deg());
            CLength height(0, CLengthUnit::m());
            CHeading heading(i * 10, CHeading::True, CAngleUnit::deg());
            CAngle bank(i, CAngleUnit::deg());
            CAngle pitch(i, CAngleUnit::deg());
            CSpeed gs(i * 10, CSpeedUnit::km_h());
            CCoordinateGeodetic c(lat, lng, height);
            CAircraftSituation s(c, a, heading, pitch, bank, gs);
            s.setMSecsSinceEpoch(ts - deltaT * i); // values in past
            s.setCallsign(cs);

            // check height above ground
            CLength hag = (a - height);
            QVERIFY2(s.getHeightAboveGround() == hag, "Wrong elevation");
            provider->insertNewSituation(s);
        }

        // make sure signals are processed
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

        // check if all  situations have been received
        QVERIFY2(interpolator.getSituationsForCallsign(cs).size() == IInterpolator::MaxSituationsPerCallsign, "Missing situations");

        // interpolation
        bool ok = false;
        double latOld = 360.0;
        double lngOld = 360.0;
        for (qint64 currentTime = ts - 2 * deltaT; currentTime < ts; currentTime += 250)
        {
            // This will use range
            // from:  ts - 2* deltaT - IInterpolator::TimeOffsetMs
            // to:    ts             - IInterpolator::TimeOffsetMs
            CAircraftSituation currentSituation(interpolator.getCurrentInterpolatedSituation
                                                (interpolator.getSituationsByCallsign(), cs, currentTime, &ok)
                                               );
            QVERIFY2(ok, "OK was false");
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
                CAircraftSituation currentSituation(interpolator.getCurrentInterpolatedSituation
                                                    (interpolator.getSituationsByCallsign(), cs, currentTime, &ok)
                                                   );
                QVERIFY2(ok, "OK was false");
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

        interpolator.syncRequestSituationsCalculationsForAllCallsigns(1, startTimeMsSinceEpoch);
        CAircraftSituationList interpolationsSync(interpolator.getRequest(1));
        QVERIFY(interpolationsSync.size() == 1);
        QVERIFY(interpolationsSync.containsCallsign(cs));

        interpolator.asyncRequestSituationsCalculationsForAllCallsigns(2, startTimeMsSinceEpoch);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        CAircraftSituationList interpolationsAsync(interpolator.getRequest(1));
        QVERIFY(interpolationsAsync.size() == 1);
        QVERIFY(interpolationsAsync.containsCallsign(cs));

        QVERIFY2(interpolationsAsync.front() == interpolationsSync.front(), "Calculated values should be equal");

    }

} // namespace
