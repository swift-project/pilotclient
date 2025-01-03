// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QDebug>
#include <QTest>
#include <QtDebug>

#include "test.h"

#include "misc/aviation/aircraftsituation.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"

using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace MiscTest
{
    //! Interpolator related basic tests
    class CTestInterpolatorMisc : public QObject
    {
        Q_OBJECT

    private slots:
        //! Basic unit tests for interpolation setup
        void setupTests();

        //! Equal situations
        void equalSituationTests();
    };

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

    void CTestInterpolatorMisc::equalSituationTests()
    {
        const CCoordinateGeodetic geoPos0 =
            CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
        const CCoordinateGeodetic geoPos1 =
            CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1488, CLengthUnit::ft() });
        const CCoordinateGeodetic geoPos2 =
            CCoordinateGeodetic::fromWgs84("48° 21′ 14″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });

        CAngle pitch(3.33, CAngleUnit::deg());
        CAngle bank(-1.0, CAngleUnit::deg());
        CHeading heading(270.0, CAngleUnit::deg());

        // initial tests
        const CAircraftSituation s0(geoPos0, heading, pitch, bank);
        CAircraftSituation s1(s0);
        CAircraftSituation s2(s0);
        CAircraftSituation sg1(s0);
        CAircraftSituation sg2(s0);

        QVERIFY2(s1.equalPbhAndVector(s2), "Expect same PHB/Vector");
        QVERIFY2(s1.equalPbhVectorAltitude(s2), "Expect same PHB/Vector/Altitude");
        QVERIFY2(sg1 == sg2, "Expect the same situation");

        sg1 = geoPos1;
        sg2 = geoPos2;

        QVERIFY2(!sg1.equalPbhVectorAltitude(s0), "Expect different positions");
        QVERIFY2(!sg2.equalPbhVectorAltitude(s0), "Expect different positions");

        // test all pitch
        for (int i = 0; i < 361; i++)
        {
            pitch = CAngle(i, CAngleUnit::deg());
            s1.setPitch(pitch);
            s2.setPitch(pitch);
            QVERIFY2(s1.equalPbhVectorAltitude(s2), "Pitch test, expect same PHB/Vector/Altitude");
            pitch = CAngle(i + 1, CAngleUnit::deg());
            s2.setPitch(pitch);
            QVERIFY2(!s1.equalPbhVectorAltitude(s2), "Pitch test, expect same PHB/Vector/Altitude");
        }

        // test all bank
        s1 = s0;
        s2 = s0;
        for (int i = 0; i < 361; i++)
        {
            bank = CAngle(i, CAngleUnit::deg());
            s1.setBank(bank);
            s2.setBank(bank);
            QVERIFY2(s1.equalPbhVectorAltitude(s2), "Bank test, expect same PHB/Vector/Altitude");
            bank = CAngle(i + 1, CAngleUnit::deg());
            s2.setBank(bank);
            QVERIFY2(!s1.equalPbhVectorAltitude(s2), "Bank test, expect same PHB/Vector/Altitude");
        }

        // test all heading
        s1 = s0;
        s2 = s0;
        for (int i = 0; i < 361; i++)
        {
            heading = CHeading(i, CAngleUnit::deg());
            s1.setHeading(heading);
            s2.setHeading(heading);
            QVERIFY2(s1.equalPbhVectorAltitude(s2), "Heading test, expect same PHB/Vector/Altitude");
            heading = CHeading(i + 1, CAngleUnit::deg());
            s2.setHeading(heading);
            QVERIFY2(!s1.equalPbhVectorAltitude(s2), "Heading test, expect same PHB/Vector/Altitude");
        }
    }
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestInterpolatorMisc);

#include "testinterpolatormisc.moc"

//! \endcond
