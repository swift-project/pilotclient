// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIMPLUGIN_FS9_DIRECTPLAY_PEER_H
#define SWIFT_SIMPLUGIN_FS9_DIRECTPLAY_PEER_H

#include <dplay8.h>

#include <atomic>
#include <functional>

#include <QList>
#include <QObject>
#include <QScopedPointer>

#include "callbackwrapper.h"
#include "fs9.h"
#include "hostnode.h"

#include "misc/aviation/callsign.h"
#include "misc/logcategorylist.h"

namespace swift::simplugin::fs9
{
    HRESULT inline s_success_pending() { return DPNSUCCESS_PENDING; }
    bool inline isPending(HRESULT hr) { return hr == s_success_pending(); }

    //! DirectPlay peer implementation
    //! More information can be found in the DirectX9 SDK documentation
    //! http://doc.51windows.net/Directx9_SDK/?url=/Directx9_SDK/play/dplay.htm
    class CDirectPlayPeer : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CDirectPlayPeer(const swift::misc::aviation::CCallsign &callsign, QObject *parent = nullptr);

        //! Destructor
        virtual ~CDirectPlayPeer() override;

        //! Returns users DirectPlay ID
        DPNID getPlayerUserId() const { return m_playerUser; }

        //! Sets users DirectPlay ID
        void setPlayerUserId(DPNID id) { m_playerUser = id; }

        //! Log categories
        static const QStringList &getLogCategories();

    public slots:
        //! Send a custom DirectPlay message
        HRESULT sendMessage(const QByteArray &data);

        //! Reset peer
        void reset();

    signals:
        //! Received custom FS9 packet
        void customPacketReceived(const QByteArray &data);

        //! Async operatione complete
        void connectionComplete();

    protected:
        //! DirectPlay message handler
        HRESULT directPlayMessageHandler(DWORD messageId, void *msgBuffer);

        //! Initialize DirectPlay
        HRESULT initDirectPlay();

        //! Returns true of the service provider is a valid on this machine
        bool isServiceProviderValid(const GUID *pGuidSP);

        //! Creates a new DirectPlay device address
        HRESULT createDeviceAddress();

        //! Creates a new DirectPlay device address
        HRESULT createHostAddress();

        const swift::misc::aviation::CCallsign m_callsign; //!< Peer callsign

        IDirectPlay8Peer *m_directPlayPeer = nullptr; //!< DirectPlay peer address
        IDirectPlay8Address *m_deviceAddress = nullptr; //!< DirectPlay device address

        QList<CHostNode> m_hostNodeList; //!< List of enumerated hosts
        quint32 m_packetIndex = 0; //!< Multiplayer packet index

        // DirectPlay Player Id's
        std::atomic<DPNID> m_playerLocal = { 0 }; //!< Local player Id
        // We need the Id of the users player, because we are sending packets only to him
        std::atomic<DPNID> m_playerUser = { 0 }; //!< User player Id

        using TCallbackWrapper =
            CallbackWrapper<CDirectPlayPeer, HRESULT, DWORD, void *>; //!< DirectPlay peer message handler wrapper
        TCallbackWrapper m_callbackWrapper; //!< Callback wrapper

    private:
        bool m_coInitializeSucceeded = false;
    };
} // namespace swift::simplugin::fs9

#endif // SWIFT_SIMPLUGIN_FS9_DIRECTPLAY_PEER_H
