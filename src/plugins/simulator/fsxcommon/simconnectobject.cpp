/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simconnectobject.h"
#include "simconnectobject.h"
#include "blackmisc/simulation/interpolatormulti.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        CSimConnectObject::CSimConnectObject()
        { }

        CSimConnectObject::CSimConnectObject(const CSimulatedAircraft &aircraft,
                                             DWORD requestId,
                                             CInterpolationLogger *logger) :
            m_aircraft(aircraft), m_requestId(requestId), m_validRequestId(true),
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(aircraft.getCallsign()))
        {
            m_interpolator->attachLogger(logger);

            // if available set situation and parts
            if (aircraft.isPartsSynchronized()) { this->addAircraftParts(aircraft.getParts()); }
            if (aircraft.getSituation().hasValidTimestamp()) { this->addAircraftSituation(aircraft.getSituation()); }
        }

        CSimConnectObject::CSimConnectObject(const CAircraftSituation &situation) :
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(situation.getCallsign()))
        {
            if (situation.hasValidTimestamp()) { this->addAircraftSituation(situation); }
        }

        CSimConnectObject::CSimConnectObject(const CAircraftParts &parts, const CCallsign &callsign) :
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(callsign))
        {
            if (parts.hasValidTimestamp()) { this->addAircraftParts(parts); }
        }

        void CSimConnectObject::addAircraftParts(const CAircraftParts &parts)
        {
            Q_ASSERT(m_interpolator);
            Q_ASSERT(parts.hasValidTimestamp());
            m_interpolator->addAircraftParts(parts);
            m_aircraft.setParts(parts);
        }

        void CSimConnectObject::addAircraftSituation(const CAircraftSituation &situation)
        {
            Q_ASSERT(m_interpolator);
            Q_ASSERT(situation.hasValidTimestamp());
            m_interpolator->addAircraftSituation(situation);
            m_aircraft.setSituation(situation); // update with last situation
        }

        void CSimConnectObject::invalidatePartsAsSent()
        {
            DataDefinitionRemoteAircraftPartsWithoutLights dd;
            dd.resetToInvalid();
            m_partsAsSent = dd;
        }

        bool CSimConnectObject::isSameAsSent(const SIMCONNECT_DATA_INITPOSITION &position) const
        {
            return std::tie(m_positionAsSent.Airspeed, m_positionAsSent.Altitude, m_positionAsSent.Bank, m_positionAsSent.Heading, m_positionAsSent.Latitude, m_positionAsSent.Longitude, m_positionAsSent.Pitch, m_positionAsSent.OnGround) ==
                   std::tie(position.Airspeed, position.Altitude, position.Bank, position.Heading, position.Latitude, position.Longitude, position.Pitch, position.OnGround);
        }

        void CSimConnectObject::invalidatePositionAsSent()
        {
            m_positionAsSent.Airspeed = 0;
            m_positionAsSent.Altitude = -1;
            m_positionAsSent.Bank = -1;
            m_positionAsSent.Heading = -1;
            m_positionAsSent.Latitude = -1;
            m_positionAsSent.Longitude = -1;
            m_positionAsSent.OnGround = 0;
            m_positionAsSent.Pitch = -1;
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

        void CSimConnectObject::resetState()
        {
            m_pendingRemoved = false;
            m_confirmedAdded = false;
            m_currentLightsInSim = CAircraftLights();
            m_lightsAsSent = CAircraftLights();
            m_requestId = -1;
            m_objectId = -1;
            m_lightsRequestedAt = -1;
            m_validRequestId = false;
            m_validObjectId = false;
            this->invalidatePartsAsSent();
            this->invalidatePositionAsSent();
        }

        bool CSimConnectObject::hasValidRequestAndObjectId() const
        {
            return this->hasValidRequestId() && this->hasValidObjectId();
        }

        void CSimConnectObject::toggleInterpolatorMode()
        {
            Q_ASSERT(m_interpolator);
            m_interpolator->toggleMode();
        }

        bool CSimConnectObject::setInterpolatorMode(CInterpolatorMulti::Mode mode)
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->setMode(mode);
        }

        QString CSimConnectObject::getInterpolatorInfo() const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatorInfo();
        }

        void CSimConnectObject::attachInterpolatorLogger(CInterpolationLogger *logger)
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->attachLogger(logger);
        }

        CAircraftSituation CSimConnectObject::getInterpolatedSituation(
            qint64 currentTimeSinceEpoc,
            const CInterpolationAndRenderingSetupPerCallsign &setup,
            CInterpolationStatus &status) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatedSituation(currentTimeSinceEpoc, setup, status);
        }

        CAircraftParts CSimConnectObject::getInterpolatedParts(
            qint64 currentTimeSinceEpoc,
            const CInterpolationAndRenderingSetupPerCallsign &setup,
            CPartsStatus &partsStatus, bool log) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
        }

        bool CSimConnectObjects::setSimConnectObjectIdForRequestId(DWORD requestId, DWORD objectId, bool resetSentParts)
        {
            // First check, if this request id belongs to us
            auto it = std::find_if(this->begin(), this->end(), [requestId](const CSimConnectObject & obj) { return obj.getRequestId() == requestId; });
            if (it == this->end()) { return false; }

            // belongs to us
            it->setObjectId(objectId);
            if (resetSentParts) { it->invalidatePartsAsSent(); }
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
            const CSimConnectObject simObject(getSimObjectForObjectId(objectId));
            return simObject.hasValidRequestAndObjectId() && objectId == simObject.getObjectId();
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

        void CSimConnectObjects::toggleInterpolatorModes()
        {
            for (const CCallsign &cs : this->keys())
            {
                (*this)[cs].toggleInterpolatorMode();
            }
        }

        void CSimConnectObjects::toggleInterpolatorMode(const CCallsign &callsign)
        {
            if (!this->contains(callsign)) { return; }
            (*this)[callsign].toggleInterpolatorMode();
        }

        int CSimConnectObjects::setInterpolatorModes(CInterpolatorMulti::Mode mode)
        {
            int c = 0;
            for (const CCallsign &cs : this->keys())
            {
                if ((*this)[cs].setInterpolatorMode(mode)) c++;
            }
            return c;
        }
    } // namespace
} // namespace
