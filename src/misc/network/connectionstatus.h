// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_CONNECTIONSTATUS_H
#define SWIFT_MISC_NETWORK_CONNECTIONSTATUS_H

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CConnectionStatus)

namespace swift::misc::network
{
    //! Value object encapsulating information about a connection status
    class SWIFT_MISC_EXPORT CConnectionStatus : public CValueObject<CConnectionStatus>
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

        //! @{
        //! Query status
        bool isConnected() const { return m_connectionStatus == Connected; }
        bool isConnecting() const { return m_connectionStatus == Connecting; }
        bool isDisconnecting() const { return m_connectionStatus == Disconnecting; }
        bool isDisconnected() const { return m_connectionStatus == Disconnected; }
        //! @}

        //! Get status
        ConnectionStatus getConnectionStatus() const { return m_connectionStatus; }

        //! Set status
        void setConnectionStatus(ConnectionStatus status) { m_connectionStatus = status; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        ConnectionStatus m_connectionStatus = Disconnected;

        SWIFT_METACLASS(
            CConnectionStatus,
            SWIFT_METAMEMBER(connectionStatus));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CConnectionStatus)

#endif // SWIFT_MISC_NETWORK_CONNECTIONSTATUS_H
