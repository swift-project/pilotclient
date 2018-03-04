/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "testinterpolatorparts.h"
#include "blackmisc/simulation/interpolatorspline.h"
#include <QTest>
#include <QtDebug>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMiscTest
{
    void CTestInterpolatorParts::groundFlagInterpolation()
    {
        CCallsign cs("SWIFT");
        CInterpolatorSpline interpolator(cs);

        // fixed time so everything can be debugged
        const qint64 ts =  1425000000000; // QDateTime::currentMSecsSinceEpoch()
        const qint64 deltaT = 5000; // ms
        const int number = interpolator.maxParts();
        // const int numberHalf = number / 2;
        const qint64 farFuture = ts + 3 * number * deltaT;
        const qint64 farPast = ts - 4 * number * deltaT;

        CAircraftPartsList parts;
        for (int i = 0; i < number; i++)
        {
            const CAircraftParts p = getTestParts(i, ts, deltaT, true);
            parts.push_back(p);
        }

        // interpolation functional check
        CPartsStatus status;
        const CInterpolationAndRenderingSetup setup;
        qint64 oldestTs = parts.oldestTimestampMsecsSinceEpoch();

        // Testing for a time >> last time
        // all on ground flags true
        interpolator.addAircraftParts(parts, false); // we work with 0 offsets here
        CAircraftParts p = interpolator.getInterpolatedParts(farFuture, setup, status);
        qint64 pTs = p.getAdjustedMSecsSinceEpoch();
        QVERIFY2(status.isSupportingParts(), "Should support parts");
        QVERIFY2(pTs == ts, "Expect latest ts");
        QCOMPARE(p.isOnGroundInterpolated(), 1.0);
        p = interpolator.getInterpolatedParts(farPast, setup, status);
        pTs = p.getAdjustedMSecsSinceEpoch();
        QVERIFY2(status.isSupportingParts(), "Should support parts");
        QVERIFY2(pTs == oldestTs, "Expect oldest ts");
        QCOMPARE(p.isOnGroundInterpolated(), 1.0);

        // Testing for a time >> last time
        // all on ground flags true
        interpolator.clear();
        parts.setOnGround(false);
        interpolator.addAircraftParts(parts, false); // we work with 0 offsets here
        p = interpolator.getInterpolatedParts(farFuture, setup, status);
        pTs = p.getAdjustedMSecsSinceEpoch();
        QVERIFY2(status.isSupportingParts(), "Should support parts");
        QVERIFY2(p.getAdjustedMSecsSinceEpoch() == pTs, "Expect latest ts");
        QCOMPARE(p.isOnGroundInterpolated(), 0.0);
    }

    CAircraftParts CTestInterpolatorParts::getTestParts(int number, qint64 ts, qint64 deltaT, bool onGround)
    {
        CAircraftLights l(true, false, true, false, true, false);
        CAircraftEngineList e({ CAircraftEngine(1, true), CAircraftEngine(2, false), CAircraftEngine(3, true) });
        CAircraftParts p(l, true, 20, true, e, false);
        p.setMSecsSinceEpoch(ts - deltaT * number); // values in past
        p.setTimeOffsetMs(0);
        p.setOnGround(onGround);
        return p;
    }
} // namespace

//! \endcond
