//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/simulation/interpolation/interpolatorlinearpbh.h"
#include "blackmisc/math/mathutils.h"
#include "test.h"

#include <QTest>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;
using namespace BlackMisc::Simulation;

namespace BlackMiscTest
{
    //! InterpolatorPBH tests
    //! As the PBH interpolator works with time-fractions, the situations are time-independent
    class CTestInterpolatorLinearPbh : public QObject
    {
        Q_OBJECT

    private slots:
        //! Test interpolate heading from 0 to 120 degrees
        static void pbhInterpolatorTestHeading0To120();

        //! Test interpolate heading from -90 to 30 degrees (over 0)
        static void pbhInterpolatorTestHeadingM90To30();

        //! Test interpolate heading from -90 to 170 degrees (over 180)
        static void pbhInterpolatorTestHeadingM90To170();

        //! Test interpolate bank from 270 to 30 degrees (over 0)
        static void pbhInterpolatorTestBank270To30();

        //! Test interpolate bank from 170 to 190 degrees (over 180)
        static void pbhInterpolatorTestBank170To190();

        //! Test interpolate pitch from 30 to -30 degrees (over 0)
        static void pbhInterpolatorTestPitch30ToM30();

        //! Test interpolate pitch from -30 to 30 degrees (over 0)
        static void pbhInterpolatorTestPitchM30To30();

    private:
        static constexpr const int m_steps = 10; //!< Time steps to check
        static constexpr const double m_tfStep = 1.0 / m_steps; //!< Time fraction between steps
    };

    void CTestInterpolatorLinearPbh::pbhInterpolatorTestHeading0To120()
    {
        CAircraftSituation s1({}, CHeading(0, CHeading::True, CAngleUnit::deg()));
        CAircraftSituation s2({}, CHeading(120, CHeading::True, CAngleUnit::deg()));
        CInterpolatorLinearPbh pbh(s1, s2);

        double lastDeg = 0;

        for (int i = 0; i <= m_steps; i++)
        {
            pbh.setTimeFraction(m_tfStep * i);
            const double h = pbh.getHeading().value(CAngleUnit::deg());

            if (i == 0)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(h, 0.0), "Expect initial heading");
            }
            else if (i == m_steps)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(h, 120.0), "Expect final heading");
            }
            else
            {
                QVERIFY2(h > lastDeg, "Expect increasing heading");
            }
            lastDeg = h;
        }
    }

    void CTestInterpolatorLinearPbh::pbhInterpolatorTestHeadingM90To30()
    {
        CAircraftSituation s1({}, CHeading(270, CHeading::True, CAngleUnit::deg())); // -90
        CAircraftSituation s2({}, CHeading(30, CHeading::True, CAngleUnit::deg()));
        CInterpolatorLinearPbh pbh(s1, s2);

        double lastDeg = 0;

        for (int i = 0; i <= m_steps; i++)
        {
            pbh.setTimeFraction(m_tfStep * i);
            const double h = pbh.getHeading().value(CAngleUnit::deg());

            if (i == 0)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(h, -90.0), "Expect  initial heading");
            }
            else if (i == m_steps)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(h, 30.0), "Expect final heading");
            }
            else
            {
                QVERIFY2(h > lastDeg, "Expect increasing heading");
            }
            lastDeg = h;
        }
    }

    void CTestInterpolatorLinearPbh::pbhInterpolatorTestHeadingM90To170()
    {
        CAircraftSituation s1({}, CHeading(270, CHeading::True, CAngleUnit::deg())); // -90
        CAircraftSituation s2({}, CHeading(170, CHeading::True, CAngleUnit::deg()));
        CInterpolatorLinearPbh pbh(s1, s2);

        double lastDeg = 0;

        for (int i = 0; i <= m_steps; i++)
        {
            pbh.setTimeFraction(m_tfStep * i);
            const double h = CAngle::normalizeDegrees360(pbh.getHeading().value(CAngleUnit::deg()));

            if (i == 0)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(h, 270.0), "Expect initial heading");
            }
            else if (i == m_steps)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(h, 170.0), "Expect final heading");
            }
            else
            {
                QVERIFY2(h < lastDeg, "Expect decreasing heading");
            }
            lastDeg = h;
        }
    }

    void CTestInterpolatorLinearPbh::pbhInterpolatorTestBank270To30()
    {
        CAircraftSituation s1({}, CHeading {}, {}, CAngle(270, CAngleUnit::deg()));
        CAircraftSituation s2({}, CHeading {}, {}, CAngle(30, CAngleUnit::deg()));
        CInterpolatorLinearPbh pbh(s1, s2);

        double lastDeg = 0;

        for (int i = 0; i <= m_steps; i++)
        {
            pbh.setTimeFraction(m_tfStep * i);
            const double b = pbh.getBank().value(CAngleUnit::deg());

            if (i == 0)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(b, -90.0), "Expect initial bank"); // -90 as not normalized
            }
            else if (i == m_steps)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(b, 30.0), "Expect final bank");
            }
            else
            {
                QVERIFY2(b > lastDeg, "Expect increasing bank");
            }
            lastDeg = b;
        }
    }

    void CTestInterpolatorLinearPbh::pbhInterpolatorTestBank170To190()
    {
        CAircraftSituation s1({}, CHeading {}, {}, CAngle(170, CAngleUnit::deg()));
        CAircraftSituation s2({}, CHeading {}, {}, CAngle(190, CAngleUnit::deg()));
        CInterpolatorLinearPbh pbh(s1, s2);

        double lastDeg = 0;

        for (int i = 0; i <= m_steps; i++)
        {
            pbh.setTimeFraction(m_tfStep * i);
            const double b = CAngle::normalizeDegrees360(pbh.getBank().value(CAngleUnit::deg()));

            if (i == 0)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(b, 170.0), "Expect initial bank");
            }
            else if (i == m_steps)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(b, 190.0), "Expect final bank");
            }
            else
            {
                QVERIFY2(b > lastDeg, "Expect increasing bank");
            }
            lastDeg = b;
        }
    }

    void CTestInterpolatorLinearPbh::pbhInterpolatorTestPitch30ToM30()
    {
        CAircraftSituation s1({}, {}, CAngle(30, CAngleUnit::deg()));
        CAircraftSituation s2({}, {}, CAngle(-30, CAngleUnit::deg()));
        CInterpolatorLinearPbh pbh(s1, s2);

        double lastDeg = 0;

        for (int i = 0; i <= m_steps; i++)
        {
            pbh.setTimeFraction(m_tfStep * i);
            const double p = pbh.getPitch().value(CAngleUnit::deg());

            if (i == 0)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(p, 30.0), "Expect initial pitch");
            }
            else if (i == m_steps)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(p, -30.0), "Expect final pitch");
            }
            else
            {
                QVERIFY2(p < lastDeg, "Expect decreasing pitch");
            }
            lastDeg = p;
        }
    }

    void CTestInterpolatorLinearPbh::pbhInterpolatorTestPitchM30To30()
    {
        CAircraftSituation s1({}, {}, CAngle(-30, CAngleUnit::deg()));
        CAircraftSituation s2({}, {}, CAngle(30, CAngleUnit::deg()));
        CInterpolatorLinearPbh pbh(s1, s2);

        double lastDeg = 0;

        for (int i = 0; i <= m_steps; i++)
        {
            pbh.setTimeFraction(m_tfStep * i);
            const double p = pbh.getPitch().value(CAngleUnit::deg());

            if (i == 0)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(p, -30.0), "Expect initial pitch");
            }
            else if (i == m_steps)
            {
                QVERIFY2(CMathUtils::epsilonEqualLimits(p, 30.0), "Expect final pitch");
            }
            else
            {
                QVERIFY2(p > lastDeg, "Expect increasing pitch");
            }
            lastDeg = p;
        }
    }
}

//! main
BLACKTEST_MAIN(BlackMiscTest::CTestInterpolatorLinearPbh);

#include "testinterpolatorlinearpbh.moc"

//! \endcond
