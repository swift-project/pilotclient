/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#define _CRT_SECURE_NO_WARNINGS

#include "fs9_client.h"
#include "multiplayer_packets.h"
#include "multiplayer_packet_parser.h"
#include "blacksimplugin_freefunctions.h"
#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/logmessage.h"
#include <QScopedArrayPointer>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSimPlugin
{
    namespace Fs9
    {
        CFs9Client::CFs9Client(
            BlackCore::IInterpolator *interpolator, QObject *owner, const QString &callsign, const CTime &updateInterval) :
            CDirectPlayPeer(owner, callsign),
            m_interpolator(interpolator), m_updateInterval(updateInterval)
        {
        }

        CFs9Client::~CFs9Client()
        {
            if (m_hostAddress) m_hostAddress->Release();
            m_hostAddress = nullptr;
        }

        void CFs9Client::sendTextMessage(const QString &textMessage)
        {
            MPChatText mpChatText;
            mpChatText.chat_data = textMessage;
            QByteArray message;
            MultiPlayerPacketParser::writeType(message, CFs9Sdk::MPCHAT_PACKET_ID_CHAT_TEXT_SEND);
            MultiPlayerPacketParser::writeSize(message, mpChatText.size());
            message = MultiPlayerPacketParser::writeMessage(message, mpChatText);
            sendMessage(message);
        }

        void CFs9Client::setHostAddress(const QString &hostAddress)
        {
            HRESULT hr = S_OK;

            // Create our IDirectPlay8Address Host Address
            if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay8Address, nullptr,
                                             CLSCTX_INPROC_SERVER,
                                             IID_IDirectPlay8Address,
                                             reinterpret_cast<void **>(&m_hostAddress))))
            {
                printDirectPlayError(hr);
                return;
            }

            if (FAILED(hr = m_hostAddress->BuildFromURLA(hostAddress.toLatin1().data())))
            {
                printDirectPlayError(hr);
                return;
            }
        }

        void CFs9Client::timerEvent(QTimerEvent *event)
        {
            Q_UNUSED(event);
            Q_ASSERT(m_interpolator);

            if (m_clientStatus == Disconnected) { return; }


            IInterpolator::InterpolationStatus status;
            CAircraftSituation situation = this->m_interpolator->getInterpolatedSituation(m_callsign, -1, status);

            // Test only for successful interpolation. FS9 requires constant positions
            if (!status.interpolationSucceeded) return;

            sendMultiplayerPosition(situation);
            sendMultiplayerParamaters();
        }

        void CFs9Client::initialize()
        {
            initDirectPlay();
            createDeviceAddress();
            //enumDirectPlayHosts();
            connectToSession(m_callsign);
        }

        void CFs9Client::cleanup()
        {
            closeConnection();
        }

        HRESULT CFs9Client::enumDirectPlayHosts()
        {
            HRESULT hr = S_OK;

            if (FAILED(hr = createHostAddress()))
            {
                qWarning() << "Failed to create host address!";
                return hr;
            }

            // Now set up the Application Description
            DPN_APPLICATION_DESC    dpAppDesc;
            ZeroMemory(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
            dpAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
            dpAppDesc.guidApplication = CFs9Sdk::guid();

            // We now have the host address so lets enum
            if (FAILED(hr = m_directPlayPeer->EnumHosts(&dpAppDesc,             // pApplicationDesc
                            m_hostAddress,                  // pdpaddrHost
                            m_deviceAddress,                // pdpaddrDeviceInfo
                            nullptr, 0,                     // pvUserEnumData, size
                            0,                              // dwEnumCount
                            0,                              // dwRetryInterval
                            0,                              // dwTimeOut
                            nullptr,                        // pvUserContext
                            nullptr,                        // pAsyncHandle
                            DPNENUMHOSTS_SYNC)))            // dwFlags
            {
                return printDirectPlayError(hr);
            }
            return hr;
        }

        HRESULT CFs9Client::createHostAddress()
        {
            HRESULT hr = S_OK;

            // Create our IDirectPlay8Address Host Address
            if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay8Address, nullptr,
                                             CLSCTX_INPROC_SERVER,
                                             IID_IDirectPlay8Address,
                                             reinterpret_cast<void **>(&m_hostAddress))))
            {
                return printDirectPlayError(hr);
            }

            // Set the SP for our Host Address
            if (FAILED(hr = m_hostAddress->SetSP(&CLSID_DP8SP_TCPIP)))
            {
                return printDirectPlayError(hr);
            }

            // FIXME: Test if this is also working via network or if we have to use the IP address
            const wchar_t hostname[] = L"localhost";

            // Set the hostname into the address
            if (FAILED(hr = m_hostAddress->AddComponent(DPNA_KEY_HOSTNAME, hostname,
                            2 * (wcslen(hostname) + 1), /*bytes*/
                            DPNA_DATATYPE_STRING)))
            {
                return printDirectPlayError(hr);
            }

            return hr;
        }

        HRESULT CFs9Client::connectToSession(const QString &callsign)
        {
            HRESULT hr = S_OK;

            if (m_clientStatus == Connected) return hr;

            QMutexLocker locker(&m_mutexHostList);

            QScopedArrayPointer<wchar_t> wszPlayername(new wchar_t[callsign.size() + 1]);

            callsign.toWCharArray(wszPlayername.data());
            wszPlayername[callsign.size()] = 0;

            ZeroMemory(&m_playerInfo, sizeof(PLAYER_INFO_STRUCT));
            strcpy(m_playerInfo.szAircraft, "Boeing 737-400 Paint1");
            m_playerInfo.dwFlags = 6;

            // Prepare and set the player information structure.
            ZeroMemory(&m_player, sizeof(DPN_PLAYER_INFO));
            m_player.dwSize = sizeof(DPN_PLAYER_INFO);
            m_player.pvData = &m_playerInfo;
            m_player.dwDataSize = sizeof(PLAYER_INFO_STRUCT);
            m_player.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
            m_player.pwszName = wszPlayername.data();
            if (FAILED(hr = m_directPlayPeer->SetPeerInfo(&m_player, nullptr, nullptr, DPNSETPEERINFO_SYNC)))
            {
                return printDirectPlayError(hr);
            }

            // Now set up the Application Description
            DPN_APPLICATION_DESC dpAppDesc;
            ZeroMemory(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
            dpAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
            dpAppDesc.guidApplication = CFs9Sdk::guid();

            // We are now ready to host the app
            if (FAILED(hr = m_directPlayPeer->Connect(&dpAppDesc,      // AppDesc
                            m_hostAddress,
                            m_deviceAddress,
                            nullptr,
                            nullptr,
                            nullptr, 0,
                            nullptr,
                            nullptr,
                            nullptr,
                            DPNCONNECT_SYNC)))
            {
                return printDirectPlayError(hr);
            }

            MPChangePlayerPlane mpChangePlayerPlane;
            mpChangePlayerPlane.engine = CFs9Sdk::ENGINE_TYPE_JET;
            mpChangePlayerPlane.aircraft_name = "Boeing 737-400";
            QByteArray message;
            MultiPlayerPacketParser::writeType(message, CFs9Sdk::MULTIPLAYER_PACKET_ID_CHANGE_PLAYER_PLANE);
            MultiPlayerPacketParser::writeSize(message, mpChangePlayerPlane.size());
            message = MultiPlayerPacketParser::writeMessage(message, mpChangePlayerPlane);
            CLogMessage(this).debug() << m_callsign << " connected to session.";
            // Send it several times, since one got missed several times.
            sendMessage(message);
            sendMessage(message);
            sendMessage(message);

            m_timerId = startTimer(m_updateInterval.value(CTimeUnit::ms()));

            m_clientStatus = Connected;
            emit statusChanged(m_callsign, m_clientStatus);

            return hr;
        }

        HRESULT CFs9Client::closeConnection()
        {
            HRESULT hr = S_OK;

            if (m_clientStatus == Disconnected) return hr;

            BlackMisc::CLogMessage(this).debug() << "Closing DirectPlay connection for " << m_callsign;
            if (FAILED(hr = m_directPlayPeer->Close(0)))
            {
                return printDirectPlayError(hr);
            }

            m_clientStatus = Disconnected;
            emit statusChanged(m_callsign, m_clientStatus);
            return hr;
        }
    }
}
