/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMCONNECT_OBJECT_H
#define BLACKSIMPLUGIN_SIMCONNECT_OBJECT_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "simconnect/SimConnect.h"
#include <QSharedPointer>

namespace BlackMisc { class IInterpolator; }

namespace BlackSimPlugin
{
    namespace Fsx
    {
        //! Class representing a SimConnect object
        class CSimConnectObject
        {
        public:
            //! Constructor
            CSimConnectObject();

            //! Constructor
            CSimConnectObject(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, DWORD requestId);

            //! Destructor
            ~CSimConnectObject() {}

            //! Get Callsign
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

            //! Simulated aircraft (as added)
            const BlackMisc::Simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

            //! Simulated aircraft model string
            const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

            //! Set Simconnect request id
            void setRequestId(DWORD id) { m_requestId = id; m_validRequestId = true; }

            //! Get Simconnect request id
            DWORD getRequestId() const { return m_requestId; }

            //! Set Simconnect object id
            void setObjectId(DWORD id) { m_objectId = id; m_validObjectId = true; }

            //! Set Simconnect object id
            DWORD getObjectId() const { return m_objectId; }

            //! Valid request id?
            bool hasValidRequestId() const { return this->m_validRequestId; }

            //! Valid object id?
            bool hasValidObjectId() const { return this->m_validObjectId; }

            //! Object is requested, not yet added
            bool isPendingAdded() const;

            //! Adding is confirmed
            bool isConfirmedAdded() const;

            //! Marked as confirmed
            void setConfirmedAdded(bool confirm);

            //! Removing is pending
            bool isPendingRemoved() const { return m_pendingRemoved; }

            //! Marked as confirmed
            void setPendingRemoved(bool pending);

            //! VTOL?
            bool isVtol() const { return m_aircraft.isVtol(); }

            //! Was the object really added to SIM
            bool hasValidRequestAndObjectId() const;

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_aircraft;
            DWORD m_requestId = 0;
            DWORD m_objectId  = 0;
            bool m_validRequestId = false;
            bool m_validObjectId = false;
            bool m_confirmedAdded = false;
            bool m_pendingRemoved = false;
        };

        //! Simulator objects (aka AI aircraft)
        class CSimConnectObjects : public QHash<BlackMisc::Aviation::CCallsign, CSimConnectObject>
        {
        public:
            //! Set ID of a SimConnect object, so far we only have an request id in the object
            bool setSimConnectObjectIdForRequestId(DWORD requestId, DWORD objectId);

            //! Find which callsign belongs to the object id
            BlackMisc::Aviation::CCallsign getCallsignForObjectId(DWORD objectId) const;

            //! Find which callsign belongs to the object id
            CSimConnectObject getSimObjectForObjectId(DWORD objectId) const;

            //! Is the object id one of our AI objects?
            bool isKnownSimObjectId(DWORD objectId) const;

            //! Pending add condition
            bool containsPendingAdd() const;
        };
    } // namespace
} // namespace

#endif // guard
