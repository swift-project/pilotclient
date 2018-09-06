/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorp3d.h"
#include "../fsxcommon/simconnectfunctions.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackSimPlugin::FsxCommon;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackSimPlugin::FsxCommon;

namespace BlackSimPlugin
{
    namespace P3D
    {
        CSimulatorP3D::CSimulatorP3D(const CSimulatorPluginInfo &info,
                                     IOwnAircraftProvider *ownAircraftProvider,
                                     IRemoteAircraftProvider *remoteAircraftProvider,
                                     IWeatherGridProvider *weatherGridProvider,
                                     IClientProvider *clientProvider,
                                     QObject *parent) :
            CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            // set build/sim specific SimConnectProc, which is the FSX SimConnectProc on WIN32 systems
            if (CBuildConfig::isCompiledWithP3DSupport() && CBuildConfig::isRunningOnWindowsNtPlatform() && CBuildConfig::buildWordSize() == 64)
            {
                // modern x64 P3D
                this->setUsingFsxTerrainProbe(false);
                m_dispatchProc = &CSimulatorP3D::SimConnectProc;
            }
            this->setDefaultModel(CAircraftModel("LOCKHEED L049_2", CAircraftModel::TypeModelMatchingDefaultModel,
                                                 "Constellation in TWA livery", CAircraftIcaoCode("CONI", "L4P")));
        }

#ifdef Q_OS_WIN64
        void CSimulatorP3D::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
        {
            CSimulatorP3D *simulatorP3D = static_cast<CSimulatorP3D *>(pContext);
            Q_ASSERT_X(simulatorP3D, Q_FUNC_INFO, "Cannot convert context to CSimulatorP3D");

            switch (pData->dwID)
            {
            // case SIMCONNECT_RECV_ID_CAMERA_6DOF: break;
            // case SIMCONNECT_RECV_ID_CAMERA_FOV: break;
            // case SIMCONNECT_RECV_ID_CAMERA_SENSOR_MODE: break;
            // case SIMCONNECT_RECV_ID_CAMERA_WINDOW_POSITION: break;
            // case SIMCONNECT_RECV_ID_CAMERA_WINDOW_SIZE: break;

            case SIMCONNECT_RECV_ID_GROUND_INFO:
                {
                    // https://www.prepar3d.com/SDKv4/sdk/simconnect_api/references/structures_and_enumerations.html#SIMCONNECT_RECV_GROUND_INFO
                    const SIMCONNECT_RECV_GROUND_INFO *pObjData = static_cast<SIMCONNECT_RECV_GROUND_INFO *>(pData);
                    const DWORD requestId = pObjData->dwRequestID;
                    if (!CSimulatorFsxCommon::isRequestForSimObjTerrainProbe(requestId)) { break; }
                    // valid elevation request
                    // https://www.prepar3d.com/SDKv4/sdk/simconnect_api/references/structures_and_enumerations.html#SIMCONNECT_DATA_GROUND_INFO
                    if (pObjData->dwArraySize != 1) { break; }
                    const SIMCONNECT_DATA_GROUND_INFO gi = pObjData->rgData[0];
                    if (!gi.bIsValid) { break; }
                    const CLatitude lat(gi.fLat, CAngleUnit::deg());
                    const CLongitude lng(gi.fLon, CAngleUnit::deg());
                    const CAltitude alt(gi.fAlt, CAltitude::MeanSeaLevel, CAltitude::TrueAltitude, CLengthUnit::ft());
                    const CCoordinateGeodetic coordinate(lat, lng, alt);
                    const CElevationPlane ep(coordinate, CElevationPlane::singlePointRadius());

                    const CCallsign cs(simulatorP3D->getCallsignForPendingProbeRequests(requestId, true));
                    simulatorP3D->callbackReceivedRequestedElevation(ep, cs);
                }
                break;
            default:
                CSimulatorFsxCommon::SimConnectProc(pData, cbData, pContext);
                break;
            }
        }

        // P3D version with new P3D simconnect functions
        bool CSimulatorP3D::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
        {
            if (reference.isNull()) { return false; }
            if (this->isShuttingDown()) { return false; }
            if (!this->isConnected()) { return false; }

            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this),  Q_FUNC_INFO, "thread");
            const bool hasHeight = reference.hasMSLGeodeticHeight();
            const double latDeg = reference.latitude().value(CAngleUnit::deg());
            const double lngDeg = reference.longitude().value(CAngleUnit::deg());
            const double maxAltFt = hasHeight ? reference.geodeticHeight().value(CLengthUnit::ft()) : 50000;
            const DWORD dwGridWidth = 1.0;
            const DWORD dwGridHeight = 1.0;

            const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimObjTerrainProbe(); // P3D we use new request id each time (no simobject)

            // returns SIMCONNECT_RECV_GROUND_INFO -> SIMCONNECT_DATA_GROUND_INFO
            const HRESULT hr = SimConnect_RequestGroundInfo(
                                   m_hSimConnect, requestId, latDeg, lngDeg, 0, latDeg, lngDeg, maxAltFt,
                                   dwGridWidth, dwGridHeight,
                                   SIMCONNECT_GROUND_INFO_LATLON_FORMAT_DEGREES,
                                   SIMCONNECT_GROUND_INFO_ALT_FORMAT_FEET,
                                   SIMCONNECT_GROUND_INFO_SOURCE_FLAG_PLATFORMS);
            bool ok = false;
            if (isOk(hr))
            {
                ok = true;
                emit this->requestedElevation(callsign);
                m_pendingProbeRequests.insert(requestId, callsign);
            }
            else
            {
                const CStatusMessage msg = CStatusMessage(this).error("SimConnect, can not request ground info: '%1' '%2'") << requestId << callsign.asString();
                CLogMessage::preformatted(msg);
            }

            return ok;
        }

        bool CSimulatorP3D::followAircraft(const CCallsign &callsign)
        {
            if (this->isShuttingDownOrDisconnected()) { return false; }
            if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return false; }

            // Experimental code, suffering from bugs and also requiring
            // P3D v4.2 (bugs in V4.1
            CSimConnectObject &simObject = m_simConnectObjects[callsign];

            const CAircraftModel model = simObject.getAircraft().getModel();
            const QString viewName = "Commercial Jet-" + callsign.asString();
            const char *view = viewName.toLatin1().constData();
            CLogMessage(this).warning("Modelview %1") << viewName;
            Q_UNUSED(model);

            HRESULT hr = SimConnect_ChangeView(m_hSimConnect, view);
            return isOk(hr);

            /**
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (simObject.getCallsignByteArray().isEmpty()) { return false; }
            const char *cs = simObject.getCallsignByteArray().constData();

            HRESULT hr = S_FALSE;
            if (!simObject.hasCamera())
            {
                GUID guid;
                CoCreateGuid(&guid);
                const SIMCONNECT_CAMERA_TYPE cameraType = SIMCONNECT_CAMERA_TYPE_OBJECT_CENTER;
                hr = SimConnect_CreateCameraDefinition(m_hSimConnect, guid, cameraType, cs, simObject.cameraPosition(), simObject.cameraRotation());
                if (hr == S_OK)
                {
                    const SIMCONNECT_OBJECT_ID objectId = static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId());
                    const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimData();
                    hr = SimConnect_CreateCameraInstance(m_hSimConnect, guid, cs, objectId, requestId);
                    if (hr == S_OK)
                    {
                        simObject.setCameraGUID(guid);
                    }
                }
            }

            if (!simObject.hasCamera()) { return false; }
            hr = SimConnect_OpenView(m_hSimConnect, cs);
            return hr == S_OK;
            **/
        }

        HRESULT CSimulatorP3D::initEventsP3D()
        {
            HRESULT hr = S_OK;
            if (hr != S_OK)
            {
                CLogMessage(this).error("P3D plugin error: %1") << "initEventsP3D failed";
                return hr;
            }
            return hr;
        }
#else
        bool CSimulatorP3D::followAircraft(const CCallsign &callsign)
        {
            Q_UNUSED(callsign);
            return false;
        }

        HRESULT CSimulatorP3D::initEventsP3D()
        {
            return S_OK;
        }

        void CSimulatorP3D::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
        {
            CSimulatorFsxCommon::SimConnectProc(pData, cbData, pContext);
        }

        bool CSimulatorP3D::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
        {
            return CSimulatorFsxCommon::requestElevation(reference, callsign);
        }
#endif
    } // namespace
} // namespace
