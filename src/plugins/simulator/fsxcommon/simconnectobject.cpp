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

        void CSimConnectObject::addAircraftParts(const CAircraftParts &parts)
        {
            Q_ASSERT(m_interpolator);
            m_interpolator->addAircraftParts(parts);
        }

        void CSimConnectObject::addAircraftSituation(const CAircraftSituation &situation)
        {
            Q_ASSERT(m_interpolator);
            m_interpolator->addAircraftSituation(situation);
        }

        void CSimConnectObject::invalidatePartsAsSent()
        {
            DataDefinitionRemoteAircraftPartsWithoutLights dd;
            dd.resetToInvalid();
            m_partsAsSent = dd;
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

        CAircraftSituation CSimConnectObject::getInterpolatedSituation(
            qint64 currentTimeSinceEpoc,
            const CInterpolationAndRenderingSetup &setup,
            const CInterpolationHints &hints, CInterpolationStatus &status) const
        {
            Q_ASSERT(m_interpolator);
            const CAircraftSituation s = m_interpolator->getInterpolatedSituation(currentTimeSinceEpoc, setup, hints, status);

            // return original position if interpolation fails for some reason
            const bool valid = status.allTrue() && !s.isPositionNull() && !s.isGeodeticHeightNull();
            return valid ? m_aircraft.getSituation() : s;
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
            return getSimObjectForObjectId(objectId).getCallsign();
        }

        CCallsignSet CSimConnectObjects::getAllCallsigns() const
        {
            return CCallsignSet(this->keys());
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
