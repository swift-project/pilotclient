// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/json.h"
#include "test.h"
#include <QTest>
#include <QJsonObject>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Json;

namespace BlackMiscTest
{
    //! Aircraft parts tests
    class CTestAircraftParts : public QObject
    {
        Q_OBJECT

    private slots:
        //! Test ground flag
        void groundFlag();

    private:
        //! Test parts
        BlackMisc::Aviation::CAircraftParts testParts1() const;
    };

    void CTestAircraftParts::groundFlag()
    {
        const CAircraftParts ap1 = this->testParts1();
        CAircraftParts ap2(ap1);
        const QJsonObject ap1Json = ap1.toJson();
        QJsonObject ap2Json = ap2.toJson();
        QJsonObject deltaJson12 = getIncrementalObject(ap1Json, ap2Json);
        QJsonObject deltaJson21 = getIncrementalObject(ap2Json, ap1Json);
        QVERIFY2(deltaJson12.isEmpty(), "Values shall be the same");
        QVERIFY2(deltaJson21.isEmpty(), "Values shall be the same");

        ap2.setOnGround(false);
        ap2Json = ap2.toJson();
        deltaJson12 = getIncrementalObject(ap1Json, ap2Json);
        deltaJson21 = getIncrementalObject(ap2Json, ap1Json);

        QVERIFY2(deltaJson12.keys().size() == 1, "Values shall be 1");
        QVERIFY2(deltaJson21.keys().size() == 1, "Values shall be 1");
        bool deltaGnd = deltaJson12.value("on_ground").toBool(true);
        QVERIFY2(deltaGnd == false, "Values shall be false");
        deltaGnd = deltaJson21.value("on_ground").toBool(false);
        QVERIFY2(deltaGnd == true, "Values shall be false");

        ap2 = ap1;
        ap2.lights().setLandingOn(false);
        deltaJson12 = getIncrementalObject(ap1.getLights().toJson(), ap2.getLights().toJson());
        QVERIFY2(deltaJson12.size() == 1, "Expect 1 value (landing) changed");

        // same on parts object
        deltaJson12 = getIncrementalObject(ap1.toJson(), ap2.toJson());
        QVERIFY2(deltaJson12.size() == 1, "Expect 1 value (lights) changed");

        ap2.engines().setEngineOn(2, false);
        ap2Json = ap2.toJson();
        deltaJson12 = getIncrementalObject(ap1Json, ap2Json);
        QVERIFY2(deltaJson12.size() == 2, "Expect 2 value (lights, engines) changed");

        // const QString json1 = stringFromJsonObject(deltaJson12);
        // const QString json2 = stringFromJsonObject(deltaJson21);
    }

    CAircraftParts CTestAircraftParts::testParts1() const
    {
        const CAircraftLights lights = CAircraftLights::allLightsOn();
        CAircraftEngineList engines;
        engines.initEngines(4, true);
        const bool onGround = true;
        CAircraftParts ap(lights, true, 0, false, engines, onGround);
        return ap;
    }
} // ns

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestAircraftParts);

#include "testaircraftparts.moc"

//! \endcond
