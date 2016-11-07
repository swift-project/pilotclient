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
            CSimConnectObject(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, int requestId);

            //! Destructor
            ~CSimConnectObject() {}

            //! Get Callsign
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

            //! Simulated aircraft (as added)
            const BlackMisc::Simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

            //! Simulated aircraft model string
            const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

            //! Set Simconnect request id
            void setRequestId(int id) { m_requestId = id; }

            //! Get Simconnect request id
            int getRequestId() const { return m_requestId; }

            //! Set Simconnect object id
            void setObjectId(int id) { m_objectId = id; }

            //! Set Simconnect object id
            int getObjectId() const { return m_objectId; }

            //! Valid request id?
            bool hasValidRequestId() const;

            //! Valid object id?
            bool hasValidobjectId() const;

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
            int m_requestId = -1;
            int m_objectId  = -1;
            bool m_confirmedAdded = false;
            bool m_pendingRemoved = false;
        };

        //! Simulator objects (aka AI aircraft
        class CSimConnectObjects : public QHash<BlackMisc::Aviation::CCallsign, CSimConnectObject>
        {
        public:
            //! Set ID of a SimConnect object, so far we only have an request id in the object
            bool setSimConnectObjectId(int requestID, int objectId);

            //! Find which callsign belongs to the object id
            BlackMisc::Aviation::CCallsign getCallsignForObjectId(int objectId) const;

            //! Find which callsign belongs to the object id
            CSimConnectObject getSimObjectForObjectId(int objectId) const;

            //! Is the object id one of our AI objects?
            bool isKnownSimObjectId(int objectId) const;

            //! Pending add condition
            bool containsPendingAdd() const;
        };

    } // namespace
} // namespace

#endif // guard
