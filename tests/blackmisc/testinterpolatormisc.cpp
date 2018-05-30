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

#include "testinterpolatormisc.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"

#include <QDebug>
#include <QTest>
#include <QtDebug>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMiscTest
{
    void CTestInterpolatorMisc::setupTests()
    {
        const CInterpolationAndRenderingSetupGlobal gs1;
        CInterpolationAndRenderingSetupGlobal gs2(gs1);

        QVERIFY2(gs1 == gs2, "Expect equal setups");
        const CLength l(100, CLengthUnit::m());
        gs2.setMaxRenderedDistance(l);
        QVERIFY2(gs1 != gs2, "Expect unequal setups");

        const CCallsign cs("DAMBZ");
        const CInterpolationAndRenderingSetupPerCallsign setup1(cs, gs1);
        CInterpolationAndRenderingSetupPerCallsign setup2(setup1);
        QVERIFY2(setup1 == setup2, "Expect equal setups (per callsign)");
        setup2.setEnabledAircraftParts(!setup2.isAircraftPartsEnabled());
        QVERIFY2(setup1 != setup2, "Expect unequal setups (per callsign)");
    }
} // namespace

//! \endcond
