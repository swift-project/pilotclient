/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simconnectobject.h"
#include "blackmisc/stringutils.h"
#include "simulatorfsxcommon.h"
#include "blackcore/simulator.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        CSimConnectObject::CSimConnectObject()
        {
            this->resetCameraPositions();
        }

        CSimConnectObject::CSimConnectObject(CSimConnectObject::SimObjectType type) : m_type(type)
        {
            this->resetCameraPositions();
        }

        CSimConnectObject::CSimConnectObject(const CSimulatedAircraft &aircraft,
                                             DWORD requestId,
                                             ISimulationEnvironmentProvider *simEnvProvider, IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *remoteAircraftProvider,
                                             CInterpolationLogger *logger) :
            m_aircraft(aircraft), m_requestId(requestId), m_validRequestId(true),
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(aircraft.getCallsign(), simEnvProvider, setupProvider, remoteAircraftProvider, logger))
        {
            this->resetCameraPositions();
            m_interpolator->initCorrespondingModel(aircraft.getModel());
            m_callsignByteArray = aircraft.getCallsignAsString().toLatin1();
        }

        void CSimConnectObject::setAircraft(const CSimulatedAircraft &aircraft)
        {
            m_aircraft = aircraft;
            m_callsignByteArray = aircraft.getCallsignAsString().toLatin1();
        }

        void CSimConnectObject::setRequestId(DWORD id)
        {
            m_requestId = id;
            m_validRequestId = true;
            const SimObjectType type = requestIdToType(id);
            this->setType(type);
        }

        DWORD CSimConnectObject::getRequestId(CSimConnectDefinitions::SimObjectRequest offset) const
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                const SimObjectType type = requestIdToType(m_requestId);
                Q_ASSERT_X(type == this->getType(), Q_FUNC_INFO, "Type mismatch");
            }

            DWORD os = 0;
            switch (this->getType())
            {
            case TerrainProbe:
                os = static_cast<DWORD>(CSimulatorFsxCommon::offsetSimObjTerrainProbe(offset));
                break;
            case Aircraft:
            default:
                os = static_cast<DWORD>(CSimulatorFsxCommon::offsetSimObjAircraft(offset));
                break;
            }
            return os + m_requestId;
        }

        void CSimConnectObject::setObjectId(DWORD id)
        {
            m_objectId = id;
            m_validObjectId = true;
        }

        bool CSimConnectObject::isPendingAdded() const
        {
            return !this->hasValidRequestAndObjectId() || !m_confirmedAdded;
        }

        bool CSimConnectObject::isConfirmedAdded() const
        {
            Q_ASSERT_X(!m_confirmedAdded || this->hasValidRequestAndObjectId(), Q_FUNC_INFO, "confirmed but invalid ids");
            return m_confirmedAdded;
        }

        void CSimConnectObject::setConfirmedAdded(bool confirm)
        {
            m_confirmedAdded = confirm;
            m_aircraft.setRendered(true);
        }

        void CSimConnectObject::setPendingRemoved(bool pending)
        {
            m_pendingRemoved = pending;
            m_aircraft.setRendered(false);
        }

        void CSimConnectObject::resetCameraPositions()
        {
            m_cameraPosition.x = 0;
            m_cameraPosition.y = 0;
            m_cameraPosition.z = 0;
            m_cameraRotation.Pitch = 0;
            m_cameraRotation.Bank = 0;
            m_cameraRotation.Heading = 0;
        }

        void CSimConnectObject::resetState()
        {
            m_pendingRemoved = false;
            m_confirmedAdded = false;
            m_camera = false;
            m_currentLightsInSim = CAircraftLights();
            m_lightsAsSent = CAircraftLights();
            m_requestId = -1;
            m_objectId = -1;
            m_lightsRequestedAt = -1;
            m_validRequestId = false;
            m_validObjectId = false;
            this->resetCameraPositions();
        }

        bool CSimConnectObject::hasValidRequestAndObjectId() const
        {
            return this->hasValidRequestId() && this->hasValidObjectId();
        }

        QString CSimConnectObject::getInterpolatorInfo(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatorInfo(mode);
        }

        void CSimConnectObject::attachInterpolatorLogger(CInterpolationLogger *logger) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->attachLogger(logger);
        }

        CInterpolationResult CSimConnectObject::getInterpolation(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber) const
        {
            if (!m_interpolator) { CInterpolationResult result; result.reset(); return result; }
            return m_interpolator->getInterpolation(currentTimeSinceEpoc, setup, aircraftNumber);
        }

        const CAircraftSituation &CSimConnectObject::getLastInterpolatedSituation(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
        {
            if (!m_interpolator) { return CAircraftSituation::null(); }
            return m_interpolator->getLastInterpolatedSituation(mode);
        }

        QString CSimConnectObject::toQString() const
        {
            static const QString s("CS: '%1' obj: %2 req: %3 conf.added: %4 pend.rem.: %5");
            return s.arg(this->getCallsign().asString()).arg(m_objectId).arg(m_requestId).arg(boolToYesNo(m_confirmedAdded), boolToYesNo(m_pendingRemoved));
        }

        CSimConnectObject::SimObjectType CSimConnectObject::requestIdToType(DWORD requestId)
        {
            if (CSimulatorFsxCommon::isRequestForSimObjTerrainProbe(requestId)) { return TerrainProbe; }
            if (CSimulatorFsxCommon::isRequestForSimObjAircraft(requestId)) { return Aircraft; }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong range");
            return Aircraft;
        }

        const QString &CSimConnectObject::typeToString(CSimConnectObject::SimObjectType type)
        {
            static const QString a("aircraft");
            static const QString p("probe");
            static const QString u("unknown");
            switch (type)
            {
            case Aircraft: return a;
            case TerrainProbe: return p;
            default: break;
            }
            return u;
        }

        bool CSimConnectObjects::setSimConnectObjectIdForRequestId(DWORD requestId, DWORD objectId)
        {
            // First check, if this request id belongs to us
            auto it = std::find_if(this->begin(), this->end(), [requestId](const CSimConnectObject & obj) { return obj.getRequestId() == requestId; });
            if (it == this->end()) { return false; }

            // belongs to us
            it->setObjectId(objectId);
            return true;
        }

        CCallsign CSimConnectObjects::getCallsignForObjectId(DWORD objectId) const
        {
            return this->getSimObjectForObjectId(objectId).getCallsign();
        }

        CCallsignSet CSimConnectObjects::getAllCallsigns() const
        {
            return CCallsignSet(this->keys());
        }

        QStringList CSimConnectObjects::getAllCallsignStrings(bool sorted) const
        {
            return this->getAllCallsigns().getCallsignStrings(sorted);
        }

        QString CSimConnectObjects::getAllCallsignStringsAsString(bool sorted, const QString &separator) const
        {
            return this->getAllCallsignStrings(sorted).join(separator);
        }

        CSimConnectObject CSimConnectObjects::getSimObjectForObjectId(DWORD objectId) const
        {
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.getObjectId() == objectId) { return simObject; }
            }
            return CSimConnectObject();
        }

        CSimConnectObject CSimConnectObjects::markObjectAsAdded(DWORD objectId)
        {
            for (const CCallsign &cs : this->keys())
            {
                CSimConnectObject &simObject = (*this)[cs];
                if (simObject.getObjectId() != objectId) { continue; }
                simObject.setConfirmedAdded(true);
                return simObject;
            }
            return CSimConnectObject();
        }

        CSimConnectObject CSimConnectObjects::getSimObjectForRequestId(DWORD requestId) const
        {
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.getRequestId() == requestId) { return simObject; }
            }
            return CSimConnectObject();
        }

        bool CSimConnectObjects::isKnownSimObjectId(DWORD objectId) const
        {
            const CSimConnectObject simObject(this->getSimObjectForObjectId(objectId));
            return simObject.hasValidRequestAndObjectId() && objectId == simObject.getObjectId();
        }

        bool CSimConnectObjects::removeByObjectId(DWORD objectId)
        {
            const CSimConnectObject simObject(this->getSimObjectForObjectId(objectId));
            const int c = this->remove(simObject.getCallsign());
            return c > 0;
        }

        bool CSimConnectObjects::containsPendingAdded() const
        {
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isPendingAdded()) { return true; }
            }
            return false;
        }

        bool CSimConnectObjects::containsPendingRemoved() const
        {
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isPendingRemoved()) { return true; }
            }
            return false;
        }

        int CSimConnectObjects::countPendingAdded() const
        {
            int c = 0;
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isPendingAdded()) { c++; }
            }
            return c;
        }

        int CSimConnectObjects::countPendingRemoved() const
        {
            int c = 0;
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isPendingRemoved()) { c++; }
            }
            return c;
        }

        int CSimConnectObjects::countConfirmedAdded()
        {
            int c = 0;
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isConfirmedAdded()) { c++; }
            }
            return c;
        }

        CCallsignSet CSimConnectObjects::getPendingAddedCallsigns() const
        {
            CCallsignSet callsigns;
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isPendingAdded()) { callsigns.push_back(simObject.getCallsign()); }
            }
            return callsigns;
        }

        CCallsignSet CSimConnectObjects::getPendingRemovedCallsigns() const
        {
            CCallsignSet callsigns;
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isPendingRemoved()) { callsigns.push_back(simObject.getCallsign()); }
            }
            return callsigns;
        }

        QList<CSimConnectObject> CSimConnectObjects::getByType(CSimConnectObject::SimObjectType type) const
        {
            QList<CSimConnectObject> objs;
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.getType() == type) { objs.push_back(simObject); }
            }
            return objs;
        }

        bool CSimConnectObjects::containsType(CSimConnectObject::SimObjectType type) const
        {
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.getType() == type) { return true; }
            }
            return false;
        }
    } // namespace
} // namespace
