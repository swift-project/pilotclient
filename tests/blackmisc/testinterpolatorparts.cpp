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
#include "blackmisc/simulation/remoteaircraftproviderdummy.h"
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
        CRemoteAircraftProviderDummy provider;
        CInterpolatorSpline interpolator(cs, nullptr, nullptr, &provider);

        // fixed time so everything can be debugged
        const qint64 ts =  1425000000000; // QDateTime::currentMSecsSinceEpoch()
        const qint64 deltaT = 5000; // ms
        const int number = CRemoteAircraftProviderDummy::MaxPartsPerCallsign;
        // const int numberHalf = number / 2;
        const qint64 farFuture = ts + 3 * number * deltaT;
        const qint64 farPast = ts - 4 * number * deltaT;

        CAircraftPartsList parts;
        for (int i = 0; i < number; i++)
        {
            const CAircraftParts p = createTestParts(i, ts, deltaT, true);
            parts.push_back(p);
        }
        QVERIFY2(parts.size() == number, "Wrong parts size of list");

        // interpolation functional check
        CPartsStatus status;
        const CInterpolationAndRenderingSetupPerCallsign setup;
        const qint64 oldestTs = parts.oldestTimestampMsecsSinceEpoch();

        // Testing for a time >> last time
        // all on ground flags true
        provider.insertNewAircraftParts(cs, parts, false); // we work with 0 offsets here
        QVERIFY2(provider.remoteAircraftPartsCount(cs) == parts.size(), "Wrong parts size");
        CAircraftParts p = interpolator.getInterpolatedParts(farFuture, setup, status);
        qint64 pTs = p.getAdjustedMSecsSinceEpoch();
        QVERIFY2(status.isSupportingParts(), "Should support parts");
        QVERIFY2(pTs == ts, "Expect latest ts");
        p = interpolator.getInterpolatedParts(farPast, setup, status);
        pTs = p.getAdjustedMSecsSinceEpoch();
        QVERIFY2(status.isSupportingParts(), "Should support parts");
        QVERIFY2(pTs == oldestTs, "Expect oldest ts");

        // Testing for a time >> last time
        // all on ground flags true
        interpolator.clear();
        provider.clear();

        parts.setOnGround(false);
        provider.insertNewAircraftParts(cs, parts, false); // we work with 0 offsets here
        p = interpolator.getInterpolatedParts(farFuture, setup, status);
        pTs = p.getAdjustedMSecsSinceEpoch();
        QVERIFY2(status.isSupportingParts(), "Should support parts");
        QVERIFY2(p.getAdjustedMSecsSinceEpoch() == pTs, "Expect latest ts");
    }

    void CTestInterpolatorParts::partsToSituationGndFlag()
    {
        CCallsign cs("SWIFT");

        // fixed time so everything can be debugged
        const qint64 ts =  1425000000000; // QDateTime::currentMSecsSinceEpoch()
        const qint64 deltaT = 5000; // ms
        const int number = 10;

        CAircraftPartsList partsOnGround;
        for (int i = 0; i < number; i++)
        {
            const CAircraftParts p = createTestParts(i, ts, deltaT, true);
            partsOnGround.push_back(p);
        }

        CAircraftPartsList partsNotOnGround;
        for (int i = 0; i < number; i++)
        {
            const CAircraftParts p = createTestParts(i, ts, deltaT, false);
            partsNotOnGround.push_back(p);
        }

        CAircraftSituationList situations;
        for (int i = 0; i < number; i++)
        {
            CAircraftSituation s = createTestSituation(cs, i, ts, deltaT, 0);
            s.setOnGround(CAircraftSituation::OnGroundSituationUnknown, CAircraftSituation::NotSet);
            situations.push_back(s);
        }

        CAircraftSituation s0 = situations[0];
        s0.adjustGroundFlag(partsOnGround, true);
        QVERIFY2(s0.getOnGround(), "Supposed to be on ground");

        s0 = situations[0];
        s0.adjustGroundFlag(partsNotOnGround, true);
        QVERIFY2(!s0.getOnGround(), "Supposed to be not on ground");

        qint64 distanceMs = -1;
        const qint64 Offset = 33;
        partsOnGround.addMsecsToOffset(Offset);
        CAircraftSituation s1 = situations[1];
        s1.setOnGroundDetails(CAircraftSituation::NotSet);
        s1.adjustGroundFlag(partsOnGround, true, 0.1, &distanceMs);
        QVERIFY2(s1.getOnGround(), "Supposed to be on ground");
        QVERIFY2(distanceMs == deltaT - Offset, "Offset time wrong");
        QVERIFY2(s1.getOnGroundDetails() == CAircraftSituation::InFromParts, "Wrong details");
    }

    CAircraftParts CTestInterpolatorParts::createTestParts(int number, qint64 ts, qint64 deltaT, bool onGround)
    {
        CAircraftLights l(true, false, true, false, true, false);
        CAircraftEngineList e({ CAircraftEngine(1, true), CAircraftEngine(2, false), CAircraftEngine(3, true) });
        CAircraftParts p(l, true, 20, true, e, false);
        p.setMSecsSinceEpoch(ts - deltaT * number); // values in past
        p.setTimeOffsetMs(0);
        p.setOnGround(onGround);
        return p;
    }

    CAircraftSituation CTestInterpolatorParts::createTestSituation(const CCallsign &callsign, int number, qint64 ts, qint64 deltaT, qint64 offset)
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
        s.setGroundElevation(gndElev);
        s.setMSecsSinceEpoch(ts - deltaT * number); // values in past
        s.setTimeOffsetMs(offset);
        return s;
    }
} // namespace

//! \endcond
