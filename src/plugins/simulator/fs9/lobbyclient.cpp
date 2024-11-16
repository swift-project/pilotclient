// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#define _CRT_SECURE_NO_WARNINGS

#include "lobbyclient.h"

#include <QDebug>
#include <QFile>
#include <QMutexLocker>
#include <QScopedPointer>
#include <QStringList>
#include <QTimer>

#include "directplayerror.h"
#include "directplayutils.h"
#include "fs9.h"

#include "core/actionbind.h"
#include "core/application.h"
#include "misc/logmessage.h"

using namespace swift::misc;

namespace swift::simplugin::fs9
{
    CLobbyClient::CLobbyClient(QObject *parent)
        : QObject(parent),
          m_callbackWrapper(this, &CLobbyClient::directPlayMessageHandler),
          m_lobbyCallbackWrapper(this, &CLobbyClient::directPlayLobbyMessageHandler)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing global object");
        initDirectPlay();
    }

    CLobbyClient::~CLobbyClient()
    {
        if (m_directPlayPeer)
        {
            m_directPlayPeer->Close(DPNCLOSE_IMMEDIATE);
            m_directPlayPeer->Release();
        }

        SafeRelease(m_deviceAddress);
        SafeRelease(m_hostAddress);

        if (m_dpLobbyClient)
        {
            m_dpLobbyClient->ReleaseApplication(DPLHANDLE_ALLCONNECTIONS, 0);
            m_dpLobbyClient->Close(0);
        }

        CoUninitialize();
    }

    HRESULT CLobbyClient::initDirectPlay()
    {
        HRESULT hr;

        // Create and init IDirectPlay8Peer
        if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay8Peer, nullptr,
                                         CLSCTX_INPROC_SERVER,
                                         IID_IDirectPlay8Peer,
                                         (LPVOID *)&m_directPlayPeer)))
            return logDirectPlayError(hr);

        // Turn off parameter validation in release builds
        const DWORD dwInitFlags = 0;
        // const DWORD dwInitFlags = DPNINITIALIZE_DISABLEPARAMVAL;

        if (FAILED(hr = m_directPlayPeer->Initialize(&m_callbackWrapper, m_callbackWrapper.messageHandler, dwInitFlags)))
            return logDirectPlayError(hr);

        // Create and init IDirectPlay8LobbyClient
        if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay8LobbyClient, nullptr,
                                         CLSCTX_INPROC_SERVER,
                                         IID_IDirectPlay8LobbyClient,
                                         (LPVOID *)&m_dpLobbyClient)))
            return logDirectPlayError(hr);

        if (FAILED(hr = m_dpLobbyClient->Initialize(&m_lobbyCallbackWrapper, m_lobbyCallbackWrapper.messageHandler, dwInitFlags)))
            return logDirectPlayError(hr);

        return S_OK;
    }

    bool CLobbyClient::canLobbyConnect()
    {
        if (!m_dpLobbyClient) { return false; }
        GUID appGuid = CFs9Sdk::guid();
        DWORD dwSize = 0;
        DWORD dwItems = 0;
        const HRESULT hr = m_dpLobbyClient->EnumLocalPrograms(&appGuid, nullptr, &dwSize, &dwItems, 0);
        if (hr == DPNERR_BUFFERTOOSMALL)
        {
            QScopedArrayPointer<BYTE> memPtr(new BYTE[dwSize]);
            DPL_APPLICATION_INFO *appInfo = reinterpret_cast<DPL_APPLICATION_INFO *>(memPtr.data());

            m_dpLobbyClient->EnumLocalPrograms(&appGuid, memPtr.data(), &dwSize, &dwItems, 0);

            if (dwItems > 0)
            {
                CLogMessage(this).debug() << "Found lobby application:" << QString::fromWCharArray(appInfo->pwszApplicationName);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    HRESULT CLobbyClient::connectFs9ToHost(const QString &address)
    {
        HRESULT hr = S_OK;

        GUID pAppGuid = CFs9Sdk::guid();

        // Setup the DPL_CONNECT_INFO struct
        DPL_CONNECT_INFO dnConnectInfo;
        ZeroMemory(&dnConnectInfo, sizeof(DPL_CONNECT_INFO));
        dnConnectInfo.dwSize = sizeof(DPL_CONNECT_INFO);
        dnConnectInfo.pvLobbyConnectData = nullptr;
        dnConnectInfo.dwLobbyConnectDataSize = 0;
        dnConnectInfo.dwFlags = 0;
        dnConnectInfo.guidApplication = pAppGuid;

        if (FAILED(hr = allocAndInitConnectSettings(address, &pAppGuid, &dnConnectInfo.pdplConnectionSettings)))
        {
            return S_FALSE;
        }

        hr = m_dpLobbyClient->ConnectApplication(&dnConnectInfo,
                                                 nullptr,
                                                 &m_applicationHandle,
                                                 INFINITE,
                                                 0);
        if (FAILED(hr)) { return hr; }

        CLogMessage(this).info(u"Lobby client '%1' connected!") << address;
        freeConnectSettings(dnConnectInfo.pdplConnectionSettings);
        return S_OK;
    }

    HRESULT CLobbyClient::allocAndInitConnectSettings(const QString &address, GUID *pAppGuid, DPL_CONNECTION_SETTINGS **ppdplConnectSettings)
    {
        HRESULT hr;

        IDirectPlay8Address *pHostAddress = nullptr;
        IDirectPlay8Address *pDeviceAddress = nullptr;

        QScopedPointer<GUID> pSPGuid(new GUID);
        memcpy(pSPGuid.data(), &CLSID_DP8SP_TCPIP, sizeof(GUID));

        // Create a host address
        if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay8Address, nullptr, CLSCTX_INPROC_SERVER,
                                         IID_IDirectPlay8Address, reinterpret_cast<void **>(&pHostAddress))))
        {
            return logDirectPlayError(hr);
        }

        // Set the SP to pHostAddress
        if (FAILED(hr = pHostAddress->SetSP(pSPGuid.data())))
        {
            return logDirectPlayError(hr);
        }

        // Create a device address to specify which device we are using
        if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
                                         IID_IDirectPlay8Address, reinterpret_cast<void **>(&pDeviceAddress))))
        {
            return logDirectPlayError(hr);
        }

        // Set the SP to pDeviceAddress
        if (FAILED(hr = pDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP)))
        {
            return logDirectPlayError(hr);
        }

        if (FAILED(hr = pHostAddress->BuildFromURLA(address.toLocal8Bit().data())))
        {
            return logDirectPlayError(hr);
        }

        // Setup the DPL_CONNECTION_SETTINGS
        DPL_CONNECTION_SETTINGS *pSettings = new DPL_CONNECTION_SETTINGS;

        // Allocate space for device address pointers
        // We cannot use QScopedArrayPointer, because memory needs to be valid
        // for the lifetime of DPL_CONNECTION_SETTINGS.
        IDirectPlay8Address **apDevAddress = new IDirectPlay8Address *[1];

        // Set the device addresses
        apDevAddress[0] = pDeviceAddress;

        QString session = sApp->swiftVersionString();
        QScopedArrayPointer<wchar_t> wstrSessionName(new wchar_t[session.size() + 1]);
        session.toWCharArray(wstrSessionName.data());
        wstrSessionName[session.size()] = 0;

        // Fill in the connection settings
        ZeroMemory(pSettings, sizeof(DPL_CONNECTION_SETTINGS));
        pSettings->dwSize = sizeof(DPL_CONNECTION_SETTINGS);
        pSettings->dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
        pSettings->dwFlags = 0;
        pSettings->dpnAppDesc.guidApplication = *pAppGuid;
        pSettings->dpnAppDesc.guidInstance = GUID_NULL;
        pSettings->dpnAppDesc.dwFlags = DPNSESSION_NODPNSVR;
        pSettings->pdp8HostAddress = pHostAddress;
        pSettings->ppdp8DeviceAddresses = apDevAddress;
        pSettings->cNumDeviceAddresses = 1;
        pSettings->dpnAppDesc.pwszSessionName = new WCHAR[wcslen(wstrSessionName.data()) + 1];
        wcscpy(pSettings->dpnAppDesc.pwszSessionName, wstrSessionName.data());

        // FIXME: Use players callsign
        QString playerName("Player");
        WCHAR wstrPlayerName[m_maxSizePlayerName];
        playerName.toWCharArray(wstrPlayerName);
        wstrPlayerName[playerName.size()] = 0;
        pSettings->pwszPlayerName = new WCHAR[wcslen(wstrPlayerName) + 1];
        wcscpy(pSettings->pwszPlayerName, wstrPlayerName);

        *ppdplConnectSettings = pSettings;

        return S_OK;
    }

    void CLobbyClient::freeConnectSettings(DPL_CONNECTION_SETTINGS *pSettings)
    {
        if (!pSettings) return;

        SafeDeleteArray(pSettings->pwszPlayerName);
        SafeDeleteArray(pSettings->dpnAppDesc.pwszSessionName);
        SafeDeleteArray(pSettings->dpnAppDesc.pwszPassword);

        // The following two lines came from DX9 SDK samples, but they don't make sense.
        // Deleteing a void* pointer is considered unsafe. If you really had to pass
        // anything non-void here, make sure the original object is cleaned up properly.
        // SafeDeleteArray(pSettings->dpnAppDesc.pvReservedData);
        // SafeDeleteArray(pSettings->dpnAppDesc.pvApplicationReservedData);
        SafeRelease(pSettings->pdp8HostAddress);
        SafeRelease(pSettings->ppdp8DeviceAddresses[0]);
        SafeDeleteArray(pSettings->ppdp8DeviceAddresses);
        SafeDelete(pSettings);
    }

    HRESULT CLobbyClient::directPlayMessageHandler(DWORD /* messageId */, void * /* msgBuffer */)
    {
        return S_OK;
    }

    HRESULT CLobbyClient::directPlayLobbyMessageHandler(DWORD messageId, void *msgBuffer)
    {
        switch (messageId)
        {
        case DPL_MSGID_DISCONNECT:
        {
            PDPL_MESSAGE_DISCONNECT pDisconnectMsg;
            pDisconnectMsg = (PDPL_MESSAGE_DISCONNECT)msgBuffer;
            Q_UNUSED(pDisconnectMsg)

            emit disconnected();

            // We should free any data associated with the
            // app here, but there is none.
            break;
        }

        case DPL_MSGID_RECEIVE:
        {
            PDPL_MESSAGE_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPL_MESSAGE_RECEIVE)msgBuffer;
            Q_UNUSED(pReceiveMsg)

            // The lobby app sent us data.  This sample doesn't
            // expected data from the app, but it is useful
            // for more complex clients.
            break;
        }

        case DPL_MSGID_SESSION_STATUS:
        {
            PDPL_MESSAGE_SESSION_STATUS pStatusMsg;
            pStatusMsg = (PDPL_MESSAGE_SESSION_STATUS)msgBuffer;

            QString message;
            message.append(QString("%1: ").arg(pStatusMsg->hSender, 0, 16));
            switch (pStatusMsg->dwStatus)
            {
            case DPLSESSION_CONNECTED:
                message.append("Session connected");
                break;
            case DPLSESSION_COULDNOTCONNECT:
                message.append("Session could not connect");
                break;
            case DPLSESSION_DISCONNECTED:
                message.append("Session disconnected");
                break;
            case DPLSESSION_TERMINATED:
                message.append("Session terminated");
                break;
            case DPLSESSION_HOSTMIGRATED:
                message.append("Host migrated");
                break;
            case DPLSESSION_HOSTMIGRATEDHERE:
                message.append("Host migrated to this client");
                break;
            default:
                message.append(QStringLiteral("Unknown PDPL_MESSAGE_SESSION_STATUS: %1").arg(pStatusMsg->dwStatus));
                break;
            }

            CLogMessage(this).info(message);
            break;
        }

        case DPL_MSGID_CONNECTION_SETTINGS:
        {
            PDPL_MESSAGE_CONNECTION_SETTINGS pConnectionStatusMsg;
            pConnectionStatusMsg = (PDPL_MESSAGE_CONNECTION_SETTINGS)msgBuffer;
            Q_UNUSED(pConnectionStatusMsg)

            // The app has changed the connection settings.
            // This simple client doesn't handle this, but more complex clients may
            // want to.
            break;
        }
        }
        return S_OK;
    }
} // namespace swift::simplugin::fs9
