// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#define _CRT_SECURE_NO_WARNINGS

#include "blackcore/application.h"
#include "blackmisc/logmessage.h"
#include "directplayerror.h"
#include "directplayutils.h"
#include "fs9host.h"
#include "multiplayerpacketparser.h"
#include "multiplayerpackets.h"
#include <QScopedArrayPointer>
#include <QVector>

using namespace BlackMisc;

namespace BlackSimPlugin::Fs9
{
    CFs9Host::CFs9Host(QObject *parent) : CDirectPlayPeer(sApp->swiftVersionString(), parent)
    {
        initDirectPlay();
        createHostAddress();
        startHosting(sApp->swiftVersionString(), m_callsign.toQString());
    }

    CFs9Host::~CFs9Host()
    {
        stopHosting();
    }

    QString CFs9Host::getHostAddress()
    {
        QString address;
        if (m_hostStatus == Hosting)
        {
            DWORD dwNumAddresses = 0;

            HRESULT hr;
            QVector<LPDIRECTPLAY8ADDRESS> addresses(static_cast<int>(dwNumAddresses));
            m_directPlayPeer->GetLocalHostAddresses(addresses.data(), &dwNumAddresses, 0);
            addresses.resize(static_cast<int>(dwNumAddresses));
            ZeroMemory(addresses.data(), dwNumAddresses * sizeof(LPDIRECTPLAY8ADDRESS));
            if (FAILED(hr = m_directPlayPeer->GetLocalHostAddresses(addresses.data(), &dwNumAddresses, 0)))
            {
                logDirectPlayError(hr);
                return address;
            }

            if (dwNumAddresses < 1) { return {}; }
            char url[250];

            /*
            DWORD size = 250;
            addresses[0]->GetURLA(url, &size);
            address = QString(url);
            */

            // try to find URL address in any address
            for (uint ii = 0; ii < dwNumAddresses; ++ii)
            {
                DWORD size = 250;
                addresses[static_cast<int>(ii)]->GetURLA(url, &size);
                address = QString(url);
                if (!address.isEmpty()) { break; }
            }

            for (uint ii = 0; ii < dwNumAddresses; ++ii)
            {
                LPDIRECTPLAY8ADDRESS pAddress = addresses[static_cast<int>(ii)];
                SafeRelease(pAddress);
            }
        }
        return address;
    }

    void CFs9Host::sendTextMessage(const QString &textMessage)
    {
        MPChatText mpChatText;
        mpChatText.chat_data = textMessage;
        QByteArray message;
        MultiPlayerPacketParser::writeType(message, CFs9Sdk::MPCHAT_PACKET_ID_CHAT_TEXT_SEND);
        MultiPlayerPacketParser::writeSize(message, mpChatText.chat_data.size() + 1);
        message = MultiPlayerPacketParser::writeMessage(message, mpChatText);
        qDebug() << "Message:" << textMessage;
        sendMessage(message);
    }

    HRESULT CFs9Host::startHosting(const QString &session, const QString &callsign)
    {
        HRESULT hr = S_OK;

        if (m_hostStatus == Hosting) { return hr; }
        if (!m_directPlayPeer) { return S_FALSE; }

        DPN_APPLICATION_DESC dpAppDesc;

        QScopedArrayPointer<wchar_t> wszSession(new wchar_t[static_cast<unsigned>(session.size()) + 1]);
        QScopedArrayPointer<wchar_t> wszPlayername(new wchar_t[static_cast<unsigned>(callsign.size()) + 1]);

        session.toWCharArray(wszSession.data());
        wszSession[session.size()] = 0;
        callsign.toWCharArray(wszPlayername.data());
        wszPlayername[callsign.size()] = 0;

        PLAYER_INFO_STRUCT playerInfo;
        ZeroMemory(&playerInfo, sizeof(PLAYER_INFO_STRUCT));
        strcpy(playerInfo.szAircraft, "Boeing 737-400");

        playerInfo.dwFlags = PLAYER_INFO_STRUCT::PARAMS_RECV | PLAYER_INFO_STRUCT::PARAMS_SEND;

        // Prepare and set the player information structure.
        DPN_PLAYER_INFO player;
        ZeroMemory(&player, sizeof(DPN_PLAYER_INFO));
        player.dwSize = sizeof(DPN_PLAYER_INFO);
        player.pvData = &playerInfo;
        player.dwDataSize = sizeof(PLAYER_INFO_STRUCT);
        player.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
        player.pwszName = wszPlayername.data();
        if (FAILED(hr = m_directPlayPeer->SetPeerInfo(&player, nullptr, nullptr, DPNSETPEERINFO_SYNC)))
        {
            logDirectPlayError(hr);
            return hr;
        }

        // Now set up the Application Description
        ZeroMemory(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
        dpAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
        dpAppDesc.guidApplication = CFs9Sdk::guid();
        dpAppDesc.pwszSessionName = wszSession.data();

        // We are now ready to host the app
        if (FAILED(hr = m_directPlayPeer->Host(&dpAppDesc, // AppDesc
                                               &m_deviceAddress, 1, // Device Address
                                               nullptr,
                                               nullptr, // Reserved
                                               nullptr, // Player Context
                                               0))) // dwFlags
        {
            logDirectPlayError(hr);
            return hr;
        }
        else
        {
            CLogMessage(this).info(u"Hosting successfully started");
            m_hostStatus = Hosting;
        }

        // Enumerate the number of stalled DirectPlay peers
        DWORD dwNumPlayers = 0;
        hr = m_directPlayPeer->EnumPlayersAndGroups(nullptr, &dwNumPlayers, DPNENUM_PLAYERS);

        if (hr == DPNERR_BUFFERTOOSMALL)
        {
            QScopedArrayPointer<DPNID> stalledPeers(new DPNID[dwNumPlayers]);
            hr = m_directPlayPeer->EnumPlayersAndGroups(stalledPeers.data(), &dwNumPlayers, DPNENUM_PLAYERS);

            // Destroy all stalled peers
            for (DWORD i = 0; i < dwNumPlayers; ++i)
            {
                m_directPlayPeer->DestroyPeer(stalledPeers[static_cast<int>(i)], nullptr, 0, 0);
            }
        }

        emit statusChanged(m_hostStatus);
        return hr;
    }

    HRESULT CFs9Host::stopHosting()
    {
        HRESULT hr = S_OK;

        if (m_hostStatus == Terminated) return hr;

        CLogMessage(this).info(u"Hosting terminated!");
        if (FAILED(hr = m_directPlayPeer->TerminateSession(nullptr, 0, 0)))
        {
            return logDirectPlayError(hr);
        }

        if (FAILED(hr = m_directPlayPeer->Close(0)))
        {
            return logDirectPlayError(hr);
        }

        m_hostStatus = Terminated;

        // Enumerate the number of stalled DirectPlay peers
        DWORD dwNumPlayers = 0;
        hr = m_directPlayPeer->EnumPlayersAndGroups(nullptr, &dwNumPlayers, DPNENUM_PLAYERS);

        if (hr == DPNERR_BUFFERTOOSMALL)
        {
            QScopedArrayPointer<DPNID> stalledPeers(new DPNID[dwNumPlayers]);
            hr = m_directPlayPeer->EnumPlayersAndGroups(stalledPeers.data(), &dwNumPlayers, DPNENUM_PLAYERS);

            // Destroy all stalled peers
            for (DWORD i = 0; i < dwNumPlayers; ++i)
            {
                m_directPlayPeer->DestroyPeer(stalledPeers[static_cast<int>(i)], nullptr, 0, 0);
            }
        }

        emit statusChanged(m_hostStatus);
        return hr;
    }
}
