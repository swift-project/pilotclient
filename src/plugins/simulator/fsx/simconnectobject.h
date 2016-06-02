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

#include "blackmisc/aviation/callsign.h"
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
            CSimConnectObject(const BlackMisc::Aviation::CCallsign &callsign, int requestId, int objectId, bool vtol);

            //! Destructor
            ~CSimConnectObject() {}

            //! Set callsign
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) { m_callsign = callsign; }

            //! Get Callsign
            BlackMisc::Aviation::CCallsign getCallsign() const { return m_callsign; }

            //! Set Simconnect request id
            void setRequestId(int id) { m_requestId = id; }

            //! Get Simconnect request id
            int getRequestId() const { return m_requestId; }

            //! Set Simconnect object id
            void setObjectId(int id) { m_objectId = id; }

            //! Set Simconnect object id
            int getObjectId() const { return m_objectId; }

            //! VTOL?
            bool isVtol() const { return m_vtol; }

            //! VTOL?
            void setVtol(bool vtol) { m_vtol =  vtol; }

        private:
            BlackMisc::Aviation::CCallsign m_callsign;
            int m_requestId = -1;
            int m_objectId  = -1;
            bool m_vtol = false;
        };
    } // namespace
} // namespace

#endif // guard
