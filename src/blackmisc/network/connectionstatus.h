/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_CONNECTIONSTATUS_H
#define BLACKMISC_NETWORK_CONNECTIONSTATUS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CConnectionStatus)

namespace BlackMisc::Network
{
    //! Value object encapsulating information about a connection status
    class BLACKMISC_EXPORT CConnectionStatus : public CValueObject<CConnectionStatus>
    {
    public:
        //! Connection Status
        enum ConnectionStatus
        {
            Connected,
            Connecting,
            Disconnecting,
            Disconnected
        };

        //! Default constructor.
        CConnectionStatus() = default;

        //! Constructor
        CConnectionStatus(ConnectionStatus status) : m_connectionStatus(status) {}

        //! Query status
        //! @{
        bool isConnected() const { return m_connectionStatus == Connected; }
        bool isConnecting() const { return m_connectionStatus == Connecting; }
        bool isDisconnecting() const { return m_connectionStatus == Disconnecting; }
        bool isDisconnected() const { return m_connectionStatus == Disconnected; }
        //! @}

        //! Get status
        ConnectionStatus getConnectionStatus() const { return m_connectionStatus; }

        //! Set status
        void setConnectionStatus(ConnectionStatus status) { m_connectionStatus = status; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        ConnectionStatus m_connectionStatus = Disconnected;

        BLACK_METACLASS(
            CConnectionStatus,
            BLACK_METAMEMBER(connectionStatus)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CConnectionStatus)

#endif // guard
