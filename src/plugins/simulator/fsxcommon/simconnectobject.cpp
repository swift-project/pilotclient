/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
            m_type = aircraft.isTerrainProbe() ? TerrainProbe : AircraftNonAtc;
            m_interpolator->initCorrespondingModel(aircraft.getModel());
            m_callsignByteArray = aircraft.getCallsignAsString().toLatin1();
        }

        void CSimConnectObject::setAircraft(const CSimulatedAircraft &aircraft)
        {
            m_aircraft = aircraft;
            m_callsignByteArray = aircraft.getCallsignAsString().toLatin1();
            m_type = aircraft.isTerrainProbe() ? TerrainProbe : AircraftNonAtc;
        }

        void CSimConnectObject::setAircraftModelString(const QString &modelString)
        {
            if (modelString.isEmpty()) { return; }
            m_aircraft.setModelString(modelString);
        }

        void CSimConnectObject::setAircraftCG(const PhysicalQuantities::CLength &cg)
        {
            if (cg.isNull()) { return; }
            m_aircraft.setCG(cg);
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
                const bool same = CSimConnectObject::isSameTypeGroup(type, this->getType());
                Q_ASSERT_X(same, Q_FUNC_INFO, "Type mismatch");
            }

            DWORD os = 0;
            switch (this->getType())
            {
            case TerrainProbe:
                os = static_cast<DWORD>(CSimulatorFsxCommon::offsetSimObjTerrainProbe(offset));
                break;
            case AircraftNonAtc:
            case AircraftSimulatedObject:
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

        bool CSimConnectObject::isOutdatedPendingAdded(qint64 thresholdMs, qint64 currentMsSinceEpoch) const
        {
            if (!this->isPendingAdded()) { return false; }
            if (currentMsSinceEpoch < 0) { currentMsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); }
            if (m_tsCreated < 0) { return true; } // no valid timestamp
            const qint64 delta = currentMsSinceEpoch - m_tsCreated;
            return delta > thresholdMs;
        }

        bool CSimConnectObject::isConfirmedAdded() const
        {
            Q_ASSERT_X(!m_confirmedAdded || this->hasValidRequestAndObjectId(), Q_FUNC_INFO, "confirmed but invalid ids");
            return m_confirmedAdded;
        }

        void CSimConnectObject::setConfirmedAdded(bool confirm)
        {
            m_confirmedAdded = confirm;
            m_removedWhileAdding = false;
            m_addedWhileRemoving = false;
            m_aircraft.setRendered(true);
        }

        void CSimConnectObject::setAddedWhileRemoving(bool addedWileRemoved)
        {
            m_addedWhileRemoving = addedWileRemoved;
        }

        void CSimConnectObject::setRemovedWhileAdding(bool removedWhileAdding)
        {
            m_removedWhileAdding = removedWhileAdding;
        }

        bool CSimConnectObject::isReadyToSend() const
        {
            return !this->isPending() && !m_addedWhileRemoving && !m_removedWhileAdding;
        }

        void CSimConnectObject::setPendingRemoved(bool pending)
        {
            m_pendingRemoved = pending;
            m_removedWhileAdding = false;
            m_addedWhileRemoving = false;
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
            m_removedWhileAdding = false;
            m_addedWhileRemoving = false;
            m_camera = false;
            m_currentLightsInSim = CAircraftLights();
            m_lightsAsSent = CAircraftLights();
            m_requestId = 0;
            m_objectId  = 0;
            m_addingExceptions = 0;
            m_validRequestId = false;
            m_validObjectId  = false;
            m_tsCreated = -1;
            this->resetCameraPositions();
        }

        void CSimConnectObject::resetToAddAgain()
        {
            const CSimConnectObject old(*this);
            this->resetState();
            this->copyAddingFailureCounters(old);
        }

        bool CSimConnectObject::hasValidRequestAndObjectId() const
        {
            return this->hasValidRequestId() && this->hasValidObjectId();
        }

        void CSimConnectObject::copyAddingFailureCounters(const CSimConnectObject &otherObject)
        {
            m_addingExceptions = otherObject.m_addingExceptions;
            m_addingDirectlyRemoved = otherObject.m_addingDirectlyRemoved;
        }

        QString CSimConnectObject::getInterpolatorInfo(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatorInfo(mode);
        }

        void CSimConnectObject::attachInterpolatorLogger(CInterpolationLogger *logger) const
        {
            Q_ASSERT(m_interpolator);
            m_interpolator->attachLogger(logger);
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

        const CStatusMessageList &CSimConnectObject::getInterpolationMessages(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
        {
            static const CStatusMessageList empty;
            if (!m_interpolator) { return empty; }
            return m_interpolator->getInterpolationMessages(mode);
        }

        QString CSimConnectObject::toQString() const
        {
            static const QString s("CS: '%1' obj: %2 req: %3 conf.added: %4 pend.rem.: %5 rwa: %6 awr: %7 aEx: %8 aRem: %9");
            return s.arg(this->getCallsign().asString()). arg(m_objectId).arg(m_requestId).arg(boolToYesNo(m_confirmedAdded), boolToYesNo(m_pendingRemoved), boolToYesNo(m_removedWhileAdding), boolToYesNo(m_addedWhileRemoving)).arg(m_addingExceptions).arg(m_addingDirectlyRemoved);
        }

        CSimConnectObject::SimObjectType CSimConnectObject::requestIdToType(DWORD requestId)
        {
            if (CSimulatorFsxCommon::isRequestForSimObjTerrainProbe(requestId)) { return TerrainProbe; }
            if (CSimulatorFsxCommon::isRequestForSimObjAircraft(requestId))     { return AircraftNonAtc; }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong range");
            return AircraftNonAtc;
        }

        const QString &CSimConnectObject::typeToString(CSimConnectObject::SimObjectType type)
        {
            static const QString a1("aircraft (non ATC)");
            static const QString a2("aircraft (sim.object)");
            static const QString p("probe");
            static const QString u("unknown");
            switch (type)
            {
            case AircraftNonAtc:    return a1;
            case AircraftSimulatedObject: return a2;
            case TerrainProbe:      return p;
            default: break;
            }
            return u;
        }

        bool CSimConnectObject::isSameTypeGroup(CSimConnectObject::SimObjectType t1, CSimConnectObject::SimObjectType t2)
        {
            if (t1 == t2) { return true; }
            return isAircraft(t1) && isAircraft(t2);
        }

        bool CSimConnectObject::isAircraft(CSimConnectObject::SimObjectType type)
        {
            return CSimConnectObject::AircraftNonAtc == type || CSimConnectObject::AircraftSimulatedObject;
        }

        bool CSimConnectObjects::insert(const CSimConnectObject &simObject, bool updateTimestamp)
        {
            if (!simObject.hasCallsign()) { return false; }
            if (updateTimestamp)
            {
                CSimConnectObject simObj(simObject);
                simObj.resetTimestampToNow();
                (*this)[simObj.getCallsign()] = simObj;
            }
            else
            {
                (*this)[simObject.getCallsign()] = simObject;
            }
            return true;
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

        CCallsignSet CSimConnectObjects::getAllCallsigns(bool withoutProbes) const
        {
            if (this->isEmpty()) { return CCallsignSet(); }
            if (!withoutProbes)  { return CCallsignSet(this->keys()); }
            CCallsignSet callsigns;
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.isAircraft()) { callsigns.insert(simObject.getCallsign().asString()); }
            }
            return callsigns;
        }

        QStringList CSimConnectObjects::getAllCallsignStrings(bool sorted, bool withoutProbes) const
        {
            return this->getAllCallsigns(withoutProbes).getCallsignStrings(sorted);
        }

        QString CSimConnectObjects::getAllCallsignStringsAsString(bool sorted, const QString &separator) const
        {
            return this->getAllCallsignStrings(sorted).join(separator);
        }

        CSimConnectObject CSimConnectObjects::getSimObjectForObjectId(DWORD objectId) const
        {
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.getObjectId() == objectId) { return simObject; }
            }
            return CSimConnectObject();
        }

        CSimConnectObject CSimConnectObjects::getOldestObject() const
        {
            if (this->isEmpty()) { return CSimConnectObject(); }
            CSimConnectObject oldestSimObj = *this->begin();
            for (const CSimConnectObject &simObj : *this)
            {
                if (!simObj.hasCreatedTimestamp()) { continue; }
                if (!oldestSimObj.hasCreatedTimestamp() || oldestSimObj.getCreatedTimestamp() > simObj.getCreatedTimestamp())
                {
                    oldestSimObj = simObj;
                }
            }
            return oldestSimObj;
        }

        CSimConnectObject CSimConnectObjects::getSimObjectForRequestId(DWORD requestId) const
        {
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.getRequestId() == requestId) { return simObject; }
            }
            return CSimConnectObject();
        }

        CSimConnectObject CSimConnectObjects::getSimObjectForOtherSimObject(const CSimConnectObject &otherSimObj) const
        {
            if (otherSimObj.hasValidObjectId())
            {
                CSimConnectObject obj = this->getSimObjectForObjectId(otherSimObj.getObjectId());
                if (!obj.isInvalid()) { return obj; }
            }
            if (!otherSimObj.hasValidRequestId()) { return CSimConnectObject(); }
            return this->getSimObjectForRequestId(otherSimObj.getRequestId());
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

        bool CSimConnectObjects::removeByOtherSimObject(const CSimConnectObject &otherSimObj)
        {
            const int c = this->remove(otherSimObj.getCallsign());
            return c > 0;
        }

        int CSimConnectObjects::removeAllProbes()
        {
            const QList<CSimConnectObject> probes = this->getProbes();
            int c = 0;
            for (const CSimConnectObject &probe : probes)
            {
                this->remove(probe.getCallsign());
                c++;
            }
            return c;
        }

        bool CSimConnectObjects::containsPendingAdded() const
        {
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.isPendingAdded()) { return true; }
            }
            return false;
        }

        bool CSimConnectObjects::containsPendingRemoved() const
        {
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.isPendingRemoved()) { return true; }
            }
            return false;
        }

        int CSimConnectObjects::countPendingAdded() const
        {
            int c = 0;
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.isPendingAdded()) { c++; }
            }
            return c;
        }

        int CSimConnectObjects::countPendingRemoved() const
        {
            int c = 0;
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.isPendingRemoved()) { c++; }
            }
            return c;
        }

        int CSimConnectObjects::countConfirmedAdded()
        {
            int c = 0;
            for (const CSimConnectObject &simObject : std::as_const(*this))
            {
                if (simObject.isConfirmedAdded()) { c++; }
            }
            return c;
        }

        CCallsignSet CSimConnectObjects::getPendingAddedCallsigns() const
        {
            CCallsignSet callsigns;
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.isPendingAdded()) { callsigns.push_back(simObject.getCallsign()); }
            }
            return callsigns;
        }

        CCallsignSet CSimConnectObjects::getPendingRemovedCallsigns() const
        {
            CCallsignSet callsigns;
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.isPendingRemoved()) { callsigns.push_back(simObject.getCallsign()); }
            }
            return callsigns;
        }

        QList<CSimConnectObject> CSimConnectObjects::getByType(CSimConnectObject::SimObjectType type) const
        {
            QList<CSimConnectObject> objs;
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.getType() == type) { objs.push_back(simObject); }
            }
            return objs;
        }

        QList<CSimConnectObject> CSimConnectObjects::getAircraft() const
        {
            QList<CSimConnectObject> l = this->getByType(CSimConnectObject::AircraftNonAtc);
            l.append(this->getByType(CSimConnectObject::AircraftSimulatedObject));
            return l;
        }

        CSimConnectObject CSimConnectObjects::getNotPendingProbe() const
        {
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.getType() == CSimConnectObject::TerrainProbe && !simObject.isPending()) { return simObject; }
            }
            return CSimConnectObject();
        }

        CSimConnectObject CSimConnectObjects::getOldestNotPendingProbe() const
        {
            CSimConnectObject oldestProbe;
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.getType() == CSimConnectObject::TerrainProbe && !simObject.isPending())
                {
                    if (!oldestProbe.hasCreatedTimestamp() || oldestProbe.getCreatedTimestamp() > simObject.getCreatedTimestamp())
                    {
                        oldestProbe = simObject;
                    }
                }
            }
            return oldestProbe;
        }

        bool CSimConnectObjects::containsType(CSimConnectObject::SimObjectType type) const
        {
            for (const CSimConnectObject &simObject : *this)
            {
                if (simObject.getType() == type) { return true; }
            }
            return false;
        }

        bool CSimConnectObjects::containsAircraft() const
        {
            return this->containsType(CSimConnectObject::AircraftNonAtc) || this->containsType(CSimConnectObject::AircraftSimulatedObject);
        }

        int CSimConnectObjects::removeCallsigns(const CCallsignSet &callsigns)
        {
            int c = 0;
            for (const CCallsign &cs : callsigns)
            {
                c += this->remove(cs);
            }
            return c;
        }

        CSimConnectObjects CSimConnectObjects::removeOutdatedPendingAdded(CSimConnectObject::SimObjectType type)
        {
            CCallsignSet removeCallsigns;
            CSimConnectObjects removedObjects;

            const qint64 ts = QDateTime::currentMSecsSinceEpoch();
            for (const CSimConnectObject &simObject : std::as_const(*this))
            {
                // verification takes at least a second, so we need some time before outdating
                if (type != CSimConnectObject::AllTypes && simObject.getType() != type)  { continue; }
                if (!simObject.isOutdatedPendingAdded(5000, ts)) { continue; }
                removedObjects.insert(simObject);
                removeCallsigns.insert(simObject.getCallsign());
            }
            if (!removeCallsigns.isEmpty())
            {
                this->removeCallsigns(removeCallsigns);
            }
            return removedObjects;
        }
    } // namespace
} // namespace
