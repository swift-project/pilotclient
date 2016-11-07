/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simconnectobject.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackSimPlugin
{
    namespace Fsx
    {
        CSimConnectObject::CSimConnectObject() { }

        CSimConnectObject::CSimConnectObject(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, int requestId) :
            m_aircraft(aircraft), m_requestId(requestId)
        { }

        bool CSimConnectObject::hasValidRequestId() const
        {
            return this->m_requestId >= 0;
        }

        bool CSimConnectObject::hasValidobjectId() const
        {
            return this->m_objectId >= 0;
        }

        bool CSimConnectObject::isPendingAdded() const
        {
            return !this->hasValidRequestAndObjectId() || !this->m_confirmedAdded;
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
            return this->hasValidRequestId() && this->hasValidobjectId();
        }

        bool CSimConnectObjects::setSimConnectObjectId(int requestID, int objectId)
        {
            // First check, if this request id belongs to us
            const int requestIntId = static_cast<int>(requestID);
            auto it = std::find_if(this->begin(), this->end(), [requestIntId](const CSimConnectObject & obj) { return obj.getRequestId() == requestIntId; });
            if (it == this->end()) { return false; }

            // belongs to us
            it->setObjectId(objectId);
            return true;
        }

        CCallsign CSimConnectObjects::getCallsignForObjectId(int objectId) const
        {
            return getSimObjectForObjectId(objectId).getCallsign();
        }

        CSimConnectObject CSimConnectObjects::getSimObjectForObjectId(int objectId) const
        {
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.getObjectId() == objectId) { return simObject; }
            }
            return CSimConnectObject();
        }

        bool CSimConnectObjects::isKnownSimObjectId(int objectId) const
        {
            const CSimConnectObject simObject(getSimObjectForObjectId(objectId));
            return simObject.hasValidRequestAndObjectId() && objectId == simObject.getObjectId();
        }

        bool CSimConnectObjects::containsPendingAdd() const
        {
            for (const CSimConnectObject &simObject : this->values())
            {
                if (simObject.isPendingAdded()) { return true; }
            }
            return false;
        }
    } // namespace
} // namespace
