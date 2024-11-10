// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorp3d.h"
#include "../fsxcommon/simconnectsymbols.h"
#include "../fscommon/simulatorfscommonfunctions.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/logmessage.h"
#include "config/buildconfig.h"

using namespace swift::config;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackSimPlugin::FsxCommon;
using namespace BlackCore;
using namespace BlackSimPlugin::FsCommon;

namespace BlackSimPlugin::P3D
{
    CSimulatorP3D::CSimulatorP3D(const CSimulatorPluginInfo &info,
                                 IOwnAircraftProvider *ownAircraftProvider,
                                 IRemoteAircraftProvider *remoteAircraftProvider,
                                 IClientProvider *clientProvider,
                                 QObject *parent) : CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, parent)
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

    bool CSimulatorP3D::connectTo()
    {
#ifdef Q_OS_WIN64
        if (!loadAndResolveP3DSimConnectByString(m_p3dVersion.get()))
        {
            return false;
        }
        return CSimulatorFsxCommon::connectTo();
#else
        if (!loadAndResolveFsxSimConnect(true))
        {
            return false;
        }
        return CSimulatorFsxCommon::connectTo();
#endif
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
            simulatorP3D->callbackReceivedRequestedElevation(ep, cs, false);
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

        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "thread");
        const bool hasHeight = reference.hasMSLGeodeticHeight();
        const double latDeg = reference.latitude().value(CAngleUnit::deg());
        const double lngDeg = reference.longitude().value(CAngleUnit::deg());
        const double maxAltFt = hasHeight ? reference.geodeticHeight().value(CLengthUnit::ft()) : 50000;
        const DWORD dwGridWidth = 1.0;
        const DWORD dwGridHeight = 1.0;

        const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimObjTerrainProbe(); // P3D we use new request id each time (no simobject)

        // returns SIMCONNECT_RECV_GROUND_INFO -> SIMCONNECT_DATA_GROUND_INFO
        const HRESULT hr = this->logAndTraceSendId(
            SimConnect_RequestGroundInfo(
                m_hSimConnect, requestId, latDeg, lngDeg, 0, latDeg, lngDeg, maxAltFt,
                dwGridWidth, dwGridHeight,
                SIMCONNECT_GROUND_INFO_LATLON_FORMAT_DEGREES,
                SIMCONNECT_GROUND_INFO_ALT_FORMAT_FEET,
                SIMCONNECT_GROUND_INFO_SOURCE_FLAG_PLATFORMS),
            Q_FUNC_INFO, "SimConnect_RequestGroundInfo");

        bool ok = false;
        if (isOk(hr))
        {
            ok = true;
            emit this->requestedElevation(callsign);
            m_pendingProbeRequests.insert(requestId, callsign);
        }
        else
        {
            const CStatusMessage msg = CStatusMessage(this).error(u"SimConnect, can not request ground info: '%1' '%2'") << requestId << callsign.asString();
            CLogMessage::preformatted(msg);
        }

        return ok;
    }

    bool CSimulatorP3D::followAircraft(const CCallsign &callsign)
    {
        if (this->isShuttingDownOrDisconnected()) { return false; }

        CSimConnectObject &simObject = m_simConnectObjects[callsign];
        if (!simObject.hasValidRequestAndObjectId()) { return false; }
        if (simObject.getCallsignByteArray().isEmpty()) { return false; }
        HRESULT hr = s_false();

        //
        // Experimental code
        //

        /** version a
        const char *cameraName = simObject.getCallsignByteArray().constData();
        if (!simObject.hasCamera())
        {
            SIMCONNECT_DATA_XYZ position; position.x = 25; position.y = 25; position.z = 0;
            SIMCONNECT_DATA_PBH rotation; rotation.Pitch = 0; rotation.Bank = 0; rotation.Heading = -90;
            simObject.setCameraPositionAndRotation(position, rotation);

            GUID guid;
            CoCreateGuid(&guid);
            // SIMCONNECT_CAMERA_TYPE_OBJECT_AI_VIRTUAL needs a P3D configuration
            // SIMCONNECT_CAMERA_TYPE_LATLONALT_ORTHOGONAL is a top down view
            // SIMCONNECT_CAMERA_TYPE_FIXED position at one place
            const SIMCONNECT_CAMERA_TYPE cameraType = SIMCONNECT_CAMERA_TYPE_OBJECT_AI_CENTER;
            hr = SimConnect_CreateCameraDefinition(m_hSimConnect, guid, cameraType, cameraName, simObject.cameraPosition(), simObject.cameraRotation());
            if (isOk(hr))
            {
                const SIMCONNECT_OBJECT_ID objectId = static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId());
                const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimObjAircraft();
                hr = SimConnect_CreateCameraInstance(m_hSimConnect, guid, cameraName, objectId, requestId);
                if (isOk(hr))
                {
                    simObject.setCameraGUID(guid);
                }
            }

        }
        version a **/

        /** version b
        const CAircraftModel m = simObject.getAircraftModel();
        const QString viewName = "Regional Jet-" + callsign.asString();
        const QByteArray viewNameBA = viewName.toLatin1();
        hr = SimConnect_ChangeView(m_hSimConnect, viewNameBA.constData());
        **/

        // Observer is P3D only, not FSX
        const CAircraftSituation situation = m_lastSentSituations[callsign];
        if (situation.isNull()) { return false; }
        SIMCONNECT_DATA_OBSERVER obs;
        SIMCONNECT_DATA_PBH pbh;
        pbh.Pitch = pbh.Bank = pbh.Heading = 0;
        obs.Rotation = pbh;
        obs.Position = coordinateToFsxLatLonAlt(situation);
        obs.Regime = SIMCONNECT_OBSERVER_REGIME_GHOST;
        obs.RotateOnTarget = TRUE;
        obs.FocusFixed = TRUE;
        obs.FieldOfViewH = 30; // deg.
        obs.FieldOfViewV = 30; // deg.
        obs.LinearStep = 20; // meters
        obs.AngularStep = 10; // deg.

        const char *observerName = simObject.getCallsignByteArray().constData();
        hr = SimConnect_CreateObserver(m_hSimConnect, observerName, obs);
        if (isOk(hr))
        {
            SIMCONNECT_DATA_XYZ offset;
            offset.x = offset.y = 30;
            offset.z = 0;
            hr = SimConnect_ObserverAttachToEntityOn(m_hSimConnect, observerName, simObject.getObjectId(), offset);
            if (isOk(hr))
            {
                SimConnect_SetObserverLookAt(m_hSimConnect, observerName, obs.Position);

                // const QByteArray viewName = QStringLiteral("Observer %1").arg(callsign.asString()).toLatin1();
                hr = SimConnect_OpenView(m_hSimConnect, observerName);

                simObject.setObserverName(callsign.asString());
            }
        }

        return isOk(hr);
    }

    HRESULT CSimulatorP3D::initEventsP3D()
    {
        HRESULT hr = s_ok();
        if (isFailure(hr))
        {
            CLogMessage(this).error(u"P3D plugin error: %1") << "initEventsP3D failed";
            return hr;
        }
        return hr;
    }

    void CSimulatorP3D::removeCamera(FsxCommon::CSimConnectObject &simObject)
    {
        if (!simObject.hasCamera()) { return; }
        simObject.removeCamera();
        // const char *cameraName = simObject.getCallsignByteArray().constData();
        // SimConnect_DeleteCameraInstance(m_hSimConnect, simObject.getCameraGUID(), 0);
        // SimConnect_CloseView(m_hSimConnect, cameraName);
    }

    void CSimulatorP3D::removeObserver(CSimConnectObject &simObject)
    {
        if (simObject.getObserverName().isEmpty()) { return; }

        QByteArray viewName = simObject.getObserverName().toLatin1();
        SimConnect_CloseView(m_hSimConnect, viewName.constData());
    }

    bool CSimulatorP3D::releaseAIControl(const CSimConnectObject &simObject, SIMCONNECT_DATA_REQUEST_ID requestId)
    {
        // completely remove AI control
        const SIMCONNECT_OBJECT_ID objectId = simObject.getObjectId();
        const HRESULT hr1 = this->logAndTraceSendId(
            SimConnect_AIReleaseControlEx(m_hSimConnect, objectId, requestId, TRUE),
            simObject, "Release control", Q_FUNC_INFO, "SimConnect_AIReleaseControlEx");
        const HRESULT hr2 = this->logAndTraceSendId(
            SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeLatLng, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
            simObject, "EventFreezeLatLng", Q_FUNC_INFO, "SimConnect_TransmitClientEvent");
        const HRESULT hr3 = this->logAndTraceSendId(
            SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAlt, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
            simObject, "EventFreezeAlt", Q_FUNC_INFO, "SimConnect_TransmitClientEvent");
        const HRESULT hr4 = this->logAndTraceSendId(
            SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAtt, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
            simObject, "EventFreezeAtt", Q_FUNC_INFO, "SimConnect_TransmitClientEvent");

        return isOk(hr1, hr2, hr3, hr4);
    }
#else
    HRESULT CSimulatorP3D::initEventsP3D()
    {
        return s_ok();
    }

    void CSimulatorP3D::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
    {
        CSimulatorFsxCommon::SimConnectProc(pData, cbData, pContext);
    }
#endif

    void CSimulatorP3DListener::startImpl()
    {
#ifdef Q_OS_WIN64
        if (!loadAndResolveP3DSimConnectByString(m_p3dVersion.get()))
        {
            return;
        }
        CSimulatorFsxCommonListener::startImpl();
#else
        if (!loadAndResolveFsxSimConnect(true))
        {
            return;
        }
        CSimulatorFsxCommonListener::startImpl();
#endif
    }

} // namespace
