/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_SIMCONNECT_OBJECT_H
#define BLACKSIMPLUGIN_SIMCONNECT_OBJECT_H

#include "blackmisc/avcallsign.h"
#include <QSharedPointer>

namespace BlackCore { class IInterpolator; }
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

            //! Get interpolator
            QSharedPointer<BlackCore::IInterpolator> getInterpolator() const { return m_interpolator; }

        private:

            BlackMisc::Aviation::CCallsign m_callsign;
            QSharedPointer<BlackCore::IInterpolator> m_interpolator;
            int m_requestId;
            int m_objectId;
        };
    }
}

#endif // guard
