/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testsimcommon.h"
#include "blacksim/simulatorinfo.h"
#include <QList>

using namespace BlackSim;

namespace BlackSimTest
{
    /*
     * Tests
     */
    void CTestFsCommon::misc()
    {
        CSimulatorInfo si1 = CSimulatorInfo::FSX();
        CSimulatorInfo si2 = CSimulatorInfo::FS9();
        CSimulatorInfo si1Copy(si1);
        QVERIFY2(si1.getValueHash() != si2.getValueHash(), "Info objects expect unequal hashs");
        QVERIFY2(si1.getValueHash() == si1Copy.getValueHash(), "Info objects expect equal hashs");
    }

} // namespace
