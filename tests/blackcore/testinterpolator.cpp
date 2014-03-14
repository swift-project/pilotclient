/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testinterpolator.h"
#include "blackcore/interpolator_linear.h"

using namespace BlackCore;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCoreTest
{

/*
 * Interpolator tests
 */
void CTestInterpolator::interpolatorBasics()
{
    IInterpolator *interpolator = new CInterpolatorLinear();

    QVERIFY2(!interpolator->hasEnoughAircraftSituations(), "Interpolator cannot have enough situations. They should be 0!");

    CAircraftSituation situation;
    interpolator->addAircraftSituation(situation);
    QTest::qSleep(6100);
    situation.setHeight(CLength(500, CLengthUnit::ft()));
    interpolator->addAircraftSituation(situation);

    QVERIFY2(interpolator->hasEnoughAircraftSituations(), "Interpolator should have enough situations!");
}

} // namespace
