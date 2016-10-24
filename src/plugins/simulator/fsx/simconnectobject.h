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
            CSimConnectObject(const BlackMisc::Simulation::CSimulatedAircraft &aircraft,int requestId);

            //! Destructor
            ~CSimConnectObject() {}

            //! Get Callsign
            BlackMisc::Aviation::CCallsign getCallsign() const { return m_aircraft.getCallsign(); }

            //! Simulated aircraft (as added)
            BlackMisc::Simulation::CSimulatedAircraft getAircraft() const { return m_aircraft; }

            //! Set Simconnect request id
            void setRequestId(int id) { m_requestId = id; }

            //! Get Simconnect request id
            int getRequestId() const { return m_requestId; }

            //! Set Simconnect object id
            void setObjectId(int id) { m_objectId = id; }

            //! Set Simconnect object id
            int getObjectId() const { return m_objectId; }

            //! VTOL?
            bool isVtol() const { return m_aircraft.isVtol(); }

            //! Was the object really added to SIM
            bool hasValidRequestAndObjectId() const;

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_aircraft;
            int m_requestId = -1;
            int m_objectId  = -1;
        };
    } // namespace
} // namespace

#endif // guard
