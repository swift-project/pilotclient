/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_SIMCONNECT_OBJECT_H
#define BLACKSIMPLUGIN_SIMCONNECT_OBJECT_H

#include "blackmisc/avcallsign.h"
#include <QSharedPointer>

namespace BlackCore
{
    class IInterpolator;
}

namespace BlackSimPlugin
{
    namespace Fsx
    {
        //! \brief Class representing a Simconnect object
        class CSimConnectObject
        {
        public:

            //! \brief Constructor
            CSimConnectObject();

            //! \brief Destructor
            ~CSimConnectObject() {}

            /*!
             * \brief Set callsign
             * \param callsign
             */
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) { m_callsign = callsign; }

            /*!
             * \brief Get Callsign
             * \return
             */
            BlackMisc::Aviation::CCallsign getCallsign() const { return m_callsign; }

            /*!
             * \brief Set Simconnect request id
             * \param id
             */
            void setRequestId(int id) { m_requestId = id; }

            /*!
             * \brief Get Simconnect request id
             * \return
             */
            int getRequestId() const { return m_requestId; }

            /*!
             * \brief Set Simconnect object id
             * \param id
             */
            void setObjectId(int id) { m_objectId = id; }

            /*!
             * \brief Set Simconnect object id
             * \return
             */
            int getObjectId() const { return m_objectId; }

            /*!
             * \brief Get interpolator
             * \return
             */
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
