/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testnetmediators.h"
#include "blackmisc/context.h"
#include <QObject>

namespace BlackCoreTest
{

    using namespace BlackCore;
    using namespace BlackMisc;
    using namespace BlackMisc::PhysicalQuantities;
    using namespace BlackMisc::Geo;

    void CTestNetMediators::initTestCase()
    {
        BlackMisc::IContext::getInstance().setSingleton<INetwork>(&m_networkDummy);
    }

    void CTestNetMediators::cleanupTestCase()
    {
        BlackMisc::IContext::getInstance().releaseSingleton<INetwork>();
    }

    void CTestNetMediators::atcListManagerTest()
    {
        CAtcListManager mgr;
        AtcListConsumer cons;

        QObject::connect(&mgr, &IAtcListManager::listChanged, &cons, &AtcListConsumer::listChanged);

        QVERIFY(cons.m_list.constMap().size() == 0);

        emit m_networkDummy.atcPositionUpdate("EGLL_TWR", CFrequency(118.7, CFrequencyUnit::MHz()), CCoordinateGeodetic(51.4775, 0.46139, 0), CLength(50, CLengthUnit::m()));
        QCoreApplication::processEvents();
        QVERIFY(cons.m_list.constMap().size() == 1);

        emit m_networkDummy.atcPositionUpdate("EGLL_GND", CFrequency(121.9, CFrequencyUnit::MHz()), CCoordinateGeodetic(51.4775, 0.46139, 0), CLength(20, CLengthUnit::m()));
        QCoreApplication::processEvents();
        QVERIFY(cons.m_list.constMap().size() == 2);

        emit m_networkDummy.atcPositionUpdate("EGLL_TWR", CFrequency(118.5, CFrequencyUnit::MHz()), CCoordinateGeodetic(51.4775, 0.46139, 0), CLength(50, CLengthUnit::m()));
        QCoreApplication::processEvents();
        QVERIFY(cons.m_list.constMap().size() == 2);

        emit m_networkDummy.atcDisconnected("EGLL_TWR");
        QCoreApplication::processEvents();
        QVERIFY(cons.m_list.constMap().size() == 1);

        emit m_networkDummy.connectionStatusDisconnected();
        QCoreApplication::processEvents();
        QVERIFY(cons.m_list.constMap().size() == 0);

        emit m_networkDummy.atcPositionUpdate("EGLL_TWR", CFrequency(118.5, CFrequencyUnit::MHz()), CCoordinateGeodetic(51.4775, 0.46139, 0), CLength(50, CLengthUnit::m()));
        QCoreApplication::processEvents();
        QVERIFY(cons.m_list.constMap().contains("EGLL_TWR"));
        QVERIFY(cons.m_list.constMap()["EGLL_TWR"].getCallsign() == "EGLL_TWR");
        QVERIFY(cons.m_list.constMap()["EGLL_TWR"].getFrequency() == CFrequency(118.5, CFrequencyUnit::MHz()));
        QVERIFY(cons.m_list.constMap()["EGLL_TWR"].getPosition() == CCoordinateGeodetic(51.4775, 0.46139, 0));
        QVERIFY(cons.m_list.constMap()["EGLL_TWR"].getAtcRange() == CLength(50, CLengthUnit::m()));
    }

} //namespace BlackCoreTest