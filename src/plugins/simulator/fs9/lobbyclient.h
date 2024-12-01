// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIMPLUGIN_FS9_LOBBYCLIENT_H
#define SWIFT_SIMPLUGIN_FS9_LOBBYCLIENT_H

#include <dplay8.h>
#include <dplobby8.h>

#include <QDebug>

#include "callbackwrapper.h"

namespace swift::simplugin::fs9
{
    //! Lobby client launching and connecting FS9
    class CLobbyClient : public QObject
    {
        Q_OBJECT

    signals:
        //! Emitted when FS9 is closed
        void disconnected();

    public:
        //! Constructor
        CLobbyClient(QObject *parent = nullptr);

        //! Destructor
        virtual ~CLobbyClient() override;

        //! Initialize DirectPlay
        HRESULT initDirectPlay();

        //! Can FS9 be lobby connected?
        bool canLobbyConnect();

        //! Connect FS9 simulator to our host
        HRESULT connectFs9ToHost(const QString &address);

    private:
        //! Alloc and fill up a DPL_CONNECTION_SETTINGS. Call FreeConnectSettings later to free it.
        HRESULT allocAndInitConnectSettings(const QString &address, GUID *pAppGuid,
                                            DPL_CONNECTION_SETTINGS **ppdplConnectSettings);

        void freeConnectSettings(DPL_CONNECTION_SETTINGS *pSettings);

        //! DirectPlay message handler
        HRESULT directPlayMessageHandler(DWORD messageId, void *msgBuffer);

        //! DirectPlay message handler
        HRESULT directPlayLobbyMessageHandler(DWORD messageId, void *msgBuffer);

        IDirectPlay8Peer *m_directPlayPeer = nullptr; //!< DirectPlay peer address
        IDirectPlay8Address *m_deviceAddress = nullptr; //!< DirectPlay device address
        IDirectPlay8Address *m_hostAddress = nullptr; //!< DirectPlay device address
        IDirectPlay8LobbyClient *m_dpLobbyClient = nullptr;

        QString m_hostname = "localhost";

        DWORD m_dwPort = 0;

        DPNHANDLE m_applicationHandle = 0;

        using TCallbackWrapper =
            CallbackWrapper<CLobbyClient, HRESULT, DWORD, void *>; //!< DirectPlay message handler wrapper
        TCallbackWrapper m_callbackWrapper; //!< Callback wrapper
        TCallbackWrapper m_lobbyCallbackWrapper; //!< Callback wrapper

        static const size_t m_maxSizePlayerName = 14;
    };
} // namespace swift::simplugin::fs9

#endif // SWIFT_SIMPLUGIN_FS9_LOBBYCLIENT_H
