// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testsimplugin
 */

#include "testsimpluginfsxp3d.h"

#include <QTest>

#include "plugins/simulator/fsxcommon/simconnectdatadefinition.h"
#include "plugins/simulator/fsxcommon/simconnectsymbols.h"
#include "plugins/simulator/fsxcommon/simulatorfsxcommon.h"

using namespace swift::simplugin::fsxcommon;

namespace swift::simplugin::fsxp3d
{
    void CSimPluginFsxP3d::resolveSymbols()
    {
#ifdef Q_OS_WIN64
        QVERIFY2(loadAndResolveP3DSimConnect(P3DSimConnectv42), "Could not load and resolve SimConnect library!");
#else
        QVERIFY2(loadAndResolveFsxSimConnect(false), "Could not load and resolve SimConnect library!");
#endif
        HANDLE hSimConnect;
        SimConnect_Open(&hSimConnect, "Test", nullptr, 0, nullptr, 0);
        SimConnect_Close(hSimConnect);
    }

    void CSimPluginFsxP3d::requestIds()
    {
        DWORD objectId = 666;
        DWORD requestId = CSimulatorFsxCommon::unitTestRequestId(CSimConnectObject::AircraftNonAtc);
        CSimConnectObject simObject(CSimConnectObject::AircraftNonAtc);
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
} // namespace swift::simplugin::fsxp3d

//! main
SWIFTTEST_MAIN(SwiftSimPluginFsxP3D::CSimPluginFsxP3d);

//! \endcond
