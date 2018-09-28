/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblacksimplugin
 */

#include "testfsxp3dcommon.h"
#include "plugins/simulator/fsxcommon/simconnectdatadefinition.h"
#include "plugins/simulator/fsxcommon/simconnectsymbols.h"
#include "plugins/simulator/fsxcommon/simulatorfsxcommon.h"
#include <QTest>

using namespace BlackSimPlugin::FsxCommon;

namespace BlackSimPluginFsxP3D
{
    void CTestFsxP3DCommon::resolveSymbols()
    {
        QVERIFY2(loadAndResolveSimConnect(false), "Could not load and resolve SimConnect library!");
        HANDLE hSimConnect;
        SimConnect_Open(&hSimConnect, "Test", nullptr, 0, nullptr, 0);
        SimConnect_Close(hSimConnect);
    }

    void CTestFsxP3DCommon::requestIds()
    {
        DWORD objectId = 666;
        DWORD requestId = CSimulatorFsxCommon::unitTestRequestId(CSimConnectObject::Aircraft);
        CSimConnectObject simObject(CSimConnectObject::Aircraft);
        simObject.setRequestId(requestId);
        simObject.setObjectId(objectId);

        QVERIFY(CSimulatorFsxCommon::isRequestForSimObjAircraft(requestId));
        QVERIFY(!CSimulatorFsxCommon::isRequestForSimObjTerrainProbe(requestId));
        CSimConnectDefinitions::SimObjectRequest sor = CSimulatorFsxCommon::requestToSimObjectRequest(requestId);
        QVERIFY(sor == CSimConnectDefinitions::SimObjectBaseId);

        requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectAdd);
        sor = CSimulatorFsxCommon::requestToSimObjectRequest(requestId);
        QVERIFY(sor == CSimConnectDefinitions::SimObjectAdd);

        requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectLights);
        sor = CSimulatorFsxCommon::requestToSimObjectRequest(requestId);
        QVERIFY(sor == CSimConnectDefinitions::SimObjectLights);

        requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectRemove);
        sor = CSimulatorFsxCommon::requestToSimObjectRequest(requestId);
        QVERIFY(sor == CSimConnectDefinitions::SimObjectRemove);

        requestId = CSimulatorFsxCommon::unitTestRequestId(CSimConnectObject::TerrainProbe);
        simObject.setRequestId(requestId);

        requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectPositionData);
        sor = CSimulatorFsxCommon::requestToSimObjectRequest(requestId);
        QVERIFY(sor == CSimConnectDefinitions::SimObjectPositionData);

        requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectMisc);
        sor = CSimulatorFsxCommon::requestToSimObjectRequest(requestId);
        QVERIFY(sor == CSimConnectDefinitions::SimObjectMisc);
    }
} // ns

//! main
BLACKTEST_APPLESS_MAIN(BlackSimPluginFsxP3D::CTestFsxP3DCommon);

//! \endcond
