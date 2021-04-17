/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackconfig/buildconfig.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/cputime.h"
// #include "blackmisc/math/mathutils.h"
// #include "blackmisc/stringutils.h"
#include "test.h"

#include <QTest>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;

namespace BlackMiscTest
{
    //! CAircraftSituation and CAircraftSituationChange tests
    class CTestAircraftSituation : public QObject
    {
        Q_OBJECT

    private slots:
        //! All GND flags
        void allGndFlagsAndTakeOff() const;

        //! All not GND flags
        void allNotGndFlagsAndTouchdown() const;

        //! Ascending aircraft
        void ascending();

        //! Descending aircraft
        void descending();

        //! Rotating up aircraft
        void rotateUp();

        //! Test sort order
        void sortOrder() const;

        //! Altitude correction
        void altitudeCorrection();

        //! Using sort hint
        void sortHint();

    private:
        //! Test situations (ascending)
        static BlackMisc::Aviation::CAircraftSituationList testSituations();

        //! Set descending altitudes
        static BlackMisc::Aviation::CAircraftSituationList testSetDescendingAltitudes(const BlackMisc::Aviation::CAircraftSituationList &situations);

        //! Set descending altitudes
        static BlackMisc::Aviation::CAircraftSituationList testSetRotateUpPitch(const BlackMisc::Aviation::CAircraftSituationList &situations);

        //! CG
        static const BlackMisc::PhysicalQuantities::CLength &cg();
    };

    void CTestAircraftSituation::allGndFlagsAndTakeOff() const
    {
        CAircraftSituationList situations = testSituations();
        situations.setOnGround(CAircraftSituation::OnGround);
        const CAircraftSituationChange change(situations, cg(), false);
        QVERIFY2(change.isConstOnGround(), "Expect const on ground");
        QVERIFY(!change.isConstNotOnGround());
        QVERIFY(!change.isJustTakingOff());
        QVERIFY(!change.isJustTouchingDown());
        QVERIFY(change.wasConstOnGround());
        QVERIFY(!change.wasConstNotOnGround());
        QVERIFY(situations.isSortedAdjustedLatestFirstWithoutNullPositions());

        CAircraftSituation f = situations.front();
        f.setOnGround(false);
        situations.pop_front();
        situations.push_front(f);
        const CAircraftSituationChange change2(situations, cg(), false);
        QVERIFY2(change2.isJustTakingOff(), "Expect just take off");
        QVERIFY(!change2.isJustTouchingDown());
        QVERIFY(change.wasConstOnGround());
        QVERIFY(!change.wasConstNotOnGround());
    }

    void CTestAircraftSituation::allNotGndFlagsAndTouchdown() const
    {
        CAircraftSituationList situations = testSetDescendingAltitudes(testSituations());
        situations.setOnGround(CAircraftSituation::NotOnGround);
        const CAircraftSituationChange change(situations, cg(), false);
        QVERIFY2(change.isConstNotOnGround(), "Expect const not on ground");
        QVERIFY(!change.isConstOnGround());
        QVERIFY(!change.isJustTakingOff());
        QVERIFY(!change.isJustTouchingDown());
        QVERIFY(!change.wasConstOnGround());
        QVERIFY(change.wasConstNotOnGround());
        QVERIFY(situations.isSortedAdjustedLatestFirstWithoutNullPositions());

        CAircraftSituation f = situations.front();
        f.setOnGround(true);
        situations.pop_front();
        situations.push_front(f);
        const CAircraftSituationChange change2(situations, cg(), false);
        QVERIFY2(change2.isJustTouchingDown(), "Expect just touchdown");
        QVERIFY(!change2.isJustTakingOff());
        QVERIFY(!change.wasConstOnGround());
        QVERIFY(change.wasConstNotOnGround());
    }

    void CTestAircraftSituation::ascending()
    {
        const CAircraftSituationList situations = testSituations();
        QVERIFY2(situations.isConstAscending(), "Expect ascending");
    }

    void CTestAircraftSituation::descending()
    {
        const CAircraftSituationList situations = testSetDescendingAltitudes(testSituations());
        QVERIFY2(situations.isConstDescending(), "Expect descending");
    }

    void CTestAircraftSituation::rotateUp()
    {
        CAircraftSituationList situations = testSetRotateUpPitch(testSituations());
        const CAircraftSituationChange change(situations, cg(), false);
        QVERIFY2(!change.isRotatingUp(), "Do not expect rotate up");

        CAircraftSituation f = situations.front();
        situations.pop_front();
        f.setPitch(CAngle(7.3, CAngleUnit::deg()));
        situations.push_front(f);

        const CAircraftSituationChange change2(situations, cg(), false);
        QVERIFY2(change2.isRotatingUp(), "Expect rotate up");
    }

    void CTestAircraftSituation::sortOrder() const
    {
        CAircraftSituationList situations = testSituations();
        QVERIFY2(situations.isSortedAdjustedLatestFirstWithoutNullPositions(), "Expect latest first");
        QVERIFY(!situations.isSortedLatestLast());

        situations.reverse();
        QVERIFY2(situations.isSortedAdjustedLatestLast(), "Expect latest first");
        QVERIFY(!situations.isSortedAdjustedLatestFirst());
    }

    void CTestAircraftSituation::altitudeCorrection()
    {
        CAircraftSituation::AltitudeCorrection correction = CAircraftSituation::NoCorrection;
        const CAircraftSituationList situations = testSituations();
        CAircraftSituation situation = situations.front();
        CAltitude alt(100, CAltitude::MeanSeaLevel, CLengthUnit::ft());

        situation.setAltitude(alt);
        CAltitude corAlt = situation.getCorrectedAltitude(true, &correction);

        // no elevation, expect same values
        QVERIFY2(corAlt == alt, "Expect same altitude");

        CElevationPlane ep(situation, CElevationPlane::singlePointRadius());
        situation.setGroundElevation(ep, CAircraftSituation::Test);

        // now we have same alt and elevation values
        // no elevation, expect same values
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(corAlt == alt, "Still expect same altitude");

        // now we use CG underflow detection
        const CLength cg(3, CLengthUnit::m());
        situation.setCG(cg);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::Underflow, "Expect underflow correction");
        QVERIFY2(corAlt > alt, "Expect corrected altitude");
        QVERIFY2((corAlt - cg) == alt, "Expect correction by CG");

        // Unit of CG must be irrelevant
        CLength cg2(cg);
        cg2.switchUnit(CLengthUnit::m());
        situation.setCG(cg2);
        CAltitude corAlt2 = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::Underflow, "Expect underflow correction");
        QVERIFY2(corAlt2 == corAlt, "Expect same value for corrected altitude");

        // now we test a negative altitude
        alt = CAltitude(-1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
        ep.setGeodeticHeight(alt);
        situation.setAltitude(alt);
        situation.setGroundElevation(ep, CAircraftSituation::Test);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::Underflow, "Expect underflow correction");
        QVERIFY2(corAlt > alt, "Expect corrected altitude");
        QVERIFY2((corAlt - cg) == alt, "Expect correction by CG");

        // overflow detection
        alt = CAltitude(1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
        situation.setAltitude(alt);
        ep.setGeodeticHeight(alt);
        ep.addAltitudeOffset(CLength(-100, CLengthUnit::ft()));
        situation.setGroundElevation(ep, CAircraftSituation::Test);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(corAlt == alt, "Expect same altitude, no overflow since not on gnd.");

        situation.setOnGround(CAircraftSituation::OnGround, CAircraftSituation::InFromNetwork);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::DraggedToGround, "Expect dragged to gnd.");
        QVERIFY2(corAlt < alt, "Expect corrected altitude dragged to gnd.");
        QVERIFY2(corAlt == (ep.getAltitude() + cg), "Expect correction by CG");
    }

    void CTestAircraftSituation::sortHint()
    {
        constexpr int Lists = 50000;
        constexpr int Loops = 20;
        CAircraftSituationList situations = testSituations();
        situations.sortAdjustedLatestFirst();
        CSequence<CAircraftSituationList> listOfLists1;
        CSequence<CAircraftSituationList> listOfLists2;
        for (int i = 0; i < Lists; ++i)
        {
            listOfLists1.push_back(situations);
            listOfLists2.push_back(situations);
            listOfLists1.back().detach();
            listOfLists2.back().detach();
            listOfLists2.back().setAdjustedSortHint(CAircraftSituationList::AdjustedTimestampLatestFirst);
        }

        QElapsedTimer time;
        int cpuTime = getThreadCpuTimeMs();
        time.start();
        for (int i = 0; i < Loops; ++i)
        {
            for (const CAircraftSituationList &s : std::as_const(listOfLists1))
            {
                const CAircraftSituation s1 = s.oldestAdjustedObject();
                const CAircraftSituation s2 = s.latestAdjustedObject();
                QVERIFY(s1.getAdjustedMSecsSinceEpoch() < s2.getAdjustedMSecsSinceEpoch());
            }
        }
        const auto noHint = std::make_pair(time.elapsed(), getThreadCpuTimeMs() - cpuTime);

        cpuTime = getThreadCpuTimeMs();
        time.start();
        for (int i = 0; i < Loops; ++i)
        {
            for (const CAircraftSituationList &s : std::as_const(listOfLists2))
            {
                const CAircraftSituation s1 = s.oldestAdjustedObject();
                const CAircraftSituation s2 = s.latestAdjustedObject();
                QVERIFY(s1.getAdjustedMSecsSinceEpoch() < s2.getAdjustedMSecsSinceEpoch());
            }
        }
        const auto hint = std::make_pair(time.elapsed(), getThreadCpuTimeMs() - cpuTime);
        const double ratio = static_cast<double>(hint.second) / static_cast<double>(noHint.second); // expected <1.0

        //qDebug() << "MacOS:" << boolToYesNo(CBuildConfig::isRunningOnMacOSPlatform());
        qDebug() << "Access without hint" << noHint.first << "ms (CPU time" << noHint.second << "ms)";
        qDebug() << "Access with hint" << hint.first << "ms (CPU time" << hint.second << "ms)";
        qDebug() << "Access ratio" << ratio;

#if 0
        // remark On Win/Linux access with hint is faster
        // on MacOS the times are the same, maybe with hint it is even slightly slower
        if (noHint >= hint)
        {
            // on MacOS we accept up to 10% overhead and SKIP the test
            if (CBuildConfig::isRunningOnMacOSPlatform())
            {
                if (ratio <= 1.1) { return; } // on MacOS 10% overhead accepted

                // more than 10% overhead!
                //! \todo KB 2018-08 we have no idea why sort hint on MacOs does not work
                QSKIP("Skipped sort hint on MacOS");
                return;
            }
        }
#endif

        //! \FIXME KB 2019-03, as discussed on Discord: temp workaround to NOT break the Jenkins build (until we know why this sometimes fails)
        const bool ok = hint <= noHint;
        if (ok || CBuildConfig::isLocalDeveloperDebugBuild())
        {
            QVERIFY2(ok, "Expected hinted sort being faster");
        }
    }

    CAircraftSituationList CTestAircraftSituation::testSituations()
    {
        // "Kugaaruk Airport","Pelly Bay","Canada","YBB","CYBB",68.534401,-89.808098,56,-7,"A","America/Edmonton","airport","OurAirports"
        // "Baie Comeau Airport","Baie Comeau","Canada","YBC","CYBC",49.13249969482422,-68.20439910888672,71,-5,"A","America/Toronto","airport","OurAirports"
        // "CFB Bagotville","Bagotville","Canada","YBG","CYBG",48.33060073852539,-70.99639892578125,522,-5,"A","America/Toronto","airport","OurAirports"

        CAircraftSituationList situations;
        const qint64 ts = QDateTime::currentSecsSinceEpoch();
        const qint64 os = CFsdSetup::c_positionTimeOffsetMsec;
        CAltitude alt(10000, CAltitude::MeanSeaLevel, CLengthUnit::m());
        static const CCoordinateGeodetic dummyPos(48.33060073852539, -70.99639892578125, 522);

        for (int i = 0; i < 10; i++)
        {
            CAircraftSituation s(dummyPos);
            const qint64 cTs = ts - i * os;
            s.setMSecsSinceEpoch(cTs);
            s.setTimeOffsetMs(os);
            CAltitude altitude(alt);
            altitude.addValueSameUnit(-100 * i); // 10000, 9900, 9800 .... (newer->older)
            s.setAltitude(altitude);
            situations.push_back(s);
        }
        return situations;
    }

    CAircraftSituationList CTestAircraftSituation::testSetDescendingAltitudes(const CAircraftSituationList &situations)
    {
        CAircraftSituationList newSituations;
        CAltitude alt(0, CAltitude::MeanSeaLevel, CLengthUnit::m());

        for (const CAircraftSituation &situation : situations)
        {
            CAircraftSituation s(situation);
            s.setAltitude(alt);
            newSituations.push_back(s);
            alt.addValueSameUnit(100); // 0, 100, 200 ... (newer->older)
        }
        return newSituations;
    }

    CAircraftSituationList CTestAircraftSituation::testSetRotateUpPitch(const CAircraftSituationList &situations)
    {
        CAircraftSituationList newSituations;
        double average = 0;
        for (const CAircraftSituation &situation : situations)
        {
            CAircraftSituation s(situation);
            const double pitch = CMathUtils::randomDouble(1.5);
            average += pitch;
            s.setPitch(CAngle(pitch, CAngleUnit::deg()));
            newSituations.push_back(s);
        }
        average = average / newSituations.size();
        CAircraftSituation avg = newSituations.front();
        avg.setPitch(CAngle(average, CAngleUnit::deg()));
        avg.addMsecs(1000); // make this the latest situation
        newSituations.push_front(avg); // first value is average pitch, so it will NOT be detected
        return newSituations;
    }

    const CLength &CTestAircraftSituation::cg()
    {
        static const CLength cg(2.0, CLengthUnit::m());
        return cg;
    }
} // namespace

//! main
BLACKTEST_MAIN(BlackMiscTest::CTestAircraftSituation);

#include "testaircraftsituation.moc"

//! \endcond
