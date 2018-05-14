/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorp3d.h"
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
using namespace BlackMisc::Weather;
using namespace BlackCore;

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
            if (CBuildConfig::isCompiledWithP3DSupport() && CBuildConfig::isRunningOnWindowsNtPlatform() && CBuildConfig::buildWordSize() == 64)
            {
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
            case SIMCONNECT_RECV_ID_GROUND_INFO:
                {
                    // https://www.prepar3d.com/SDKv4/sdk/simconnect_api/references/structures_and_enumerations.html#SIMCONNECT_RECV_GROUND_INFO
                    const SIMCONNECT_RECV_GROUND_INFO *pObjData = (SIMCONNECT_RECV_GROUND_INFO *) pData;
                    const DWORD requestId = pObjData->dwRequestID;
                    if (!CSimulatorFsxCommon::isRequestForProbe(requestId)) { break; }
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
            const double latDeg = reference.latitude().value(CAngleUnit::deg());
            const double lngDeg = reference.longitude().value(CAngleUnit::deg());
            const double maxAltFt = reference.geodeticHeight().value(CLengthUnit::ft());
            const DWORD dwGridWidth = 1.0;
            const DWORD dwGridHeight = 1.0;

            const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForProbe();

            // returns SIMCONNECT_RECV_GROUND_INFO -> SIMCONNECT_DATA_GROUND_INFO
            const HRESULT hr = SimConnect_RequestGroundInfo(
                                   m_hSimConnect, requestId, latDeg, lngDeg, 0, latDeg, lngDeg, maxAltFt,
                                   dwGridWidth, dwGridHeight,
                                   SIMCONNECT_GROUND_INFO_LATLON_FORMAT_DEGREES,
                                   SIMCONNECT_GROUND_INFO_ALT_FORMAT_FEET,
                                   SIMCONNECT_GROUND_INFO_SOURCE_FLAG_PLATFORMS);
            bool ok = false;
            if (hr == S_OK)
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
#else
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
