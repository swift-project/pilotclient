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
#include <QScopedArrayPointer>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSimPlugin
{
    namespace Fs9
    {
        CFs9Client::CFs9Client(const QString &callsign, const CTime &updateInterval, QObject *parent) :
            CDirectPlayPeer(callsign, parent),
            m_updateInterval(updateInterval),
            m_callbackWrapper(this, &CFs9Client::directPlayMessageHandler)
        {
        }

        CFs9Client::~CFs9Client()
        {
             if(m_hostAddress) m_hostAddress->Release();
             m_hostAddress = nullptr;
        }

        void CFs9Client::init()
        {
            initDirectPlay();
            createDeviceAddress();
            //enumDirectPlayHosts();
            connectToSession(m_callsign);
        }

        void CFs9Client::sendTextMessage(const QString &textMessage)
        {
            MPChatText mpChatText;
            mpChatText.chat_data = textMessage;
            QByteArray message;
            MultiPlayerPacketParser::writeType(message, CFs9Sdk::MPCHAT_PACKET_ID_CHAT_TEXT_SEND);
            MultiPlayerPacketParser::writeSize(message, mpChatText.chat_data.size() + 1);
            message = MultiPlayerPacketParser::writeMessage(message, mpChatText);
            sendMessage(message);
        }

        void CFs9Client::disconnectFrom()
        {
            qDebug() << "Disconnecting...";
            killTimer(m_timerId);
            closeConnection();
        }

        void CFs9Client::setHostAddress(const QString &hostAddress)
        {
            HRESULT hr = S_OK;

            // Create our IDirectPlay8Address Host Address
            if( FAILED( hr = CoCreateInstance(CLSID_DirectPlay8Address, nullptr,
                                            CLSCTX_INPROC_SERVER,
                                            IID_IDirectPlay8Address,
                                            reinterpret_cast<void **>(&m_hostAddress) ) ) )
            {
                printDirectPlayError(hr);
                return;
            }

            if (FAILED (hr = m_hostAddress->BuildFromURLA(hostAddress.toLatin1().data())))
            {
                printDirectPlayError(hr);
                return;
            }
        }

        void CFs9Client::addAircraftSituation(const CAircraftSituation &situation)
        {
            QMutexLocker locker(&m_mutexInterpolator);
            m_interpolator.addAircraftSituation(situation);
        }

        void CFs9Client::timerEvent(QTimerEvent * /*event*/)
        {
            if (m_clientStatus == Disconnected) return;

            QMutexLocker locker(&m_mutexInterpolator);

            if (m_interpolator.getTimeOfLastReceivedSituation().secsTo(QDateTime::currentDateTimeUtc()) > 15)
            {
                emit clientTimedOut(m_callsign);
                return;
            }

            if (m_interpolator.hasEnoughAircraftSituations())
            {
                CAircraftSituation situation = m_interpolator.getCurrentSituation();
                MPPositionVelocity positioneVelocity = aircraftSituationtoFS9(m_lastAircraftSituation,
                                                             situation,
                                                             m_updateInterval.value(CTimeUnit::s()));

                QByteArray positionMessage;
                MultiPlayerPacketParser::writeType(positionMessage, CFs9Sdk::MULTIPLAYER_PACKET_ID_POSITION_VELOCITY);
                MultiPlayerPacketParser::writeSize(positionMessage, 52);
                positioneVelocity.packet_index = m_packetIndex;
                ++m_packetIndex;
                positionMessage = MultiPlayerPacketParser::writeMessage(positionMessage, positioneVelocity);

                sendMessage(positionMessage);

                m_lastAircraftSituation = situation;
            }
        }

        HRESULT CFs9Client::enumDirectPlayHosts()
        {
            HRESULT hr = S_OK;

            if( FAILED( hr = createHostAddress() ) )
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
            if( FAILED( hr = m_directPlayPeer->EnumHosts(&dpAppDesc,            // pApplicationDesc
                                                m_hostAddress,                  // pdpaddrHost
                                                m_deviceAddress,                // pdpaddrDeviceInfo
                                                nullptr, 0,                     // pvUserEnumData, size
                                                0,                              // dwEnumCount
                                                0,                              // dwRetryInterval
                                                0,                              // dwTimeOut
                                                nullptr,                        // pvUserContext
                                                nullptr,                        // pAsyncHandle
                                                DPNENUMHOSTS_SYNC ) ) )         // dwFlags
            {
                qWarning() << "Failed to enum hosts!";
                return hr;
            }
            return hr;
        }

        HRESULT CFs9Client::createHostAddress()
        {
            HRESULT hr = S_OK;

            // Create our IDirectPlay8Address Host Address
            if( FAILED( hr = CoCreateInstance(CLSID_DirectPlay8Address, nullptr,
                                            CLSCTX_INPROC_SERVER,
                                            IID_IDirectPlay8Address,
                                            reinterpret_cast<void **>(&m_hostAddress) ) ) )
            {
                qWarning() << "Failed to create DirectPlay8Address!";
                return hr;
            }

            // Set the SP for our Host Address
            if( FAILED( hr = m_hostAddress->SetSP(&CLSID_DP8SP_TCPIP ) ) )
            {
                qWarning() << "Failed to set SP!";
                return hr;
            }

            // FIXME: Test if this is also working via network or if we have to use the IP address
            const wchar_t hostname[] = L"localhost";

            // Set the hostname into the address
            if( FAILED( hr = m_hostAddress->AddComponent(DPNA_KEY_HOSTNAME, hostname,
                                                            2*(wcslen(hostname) + 1), /*bytes*/
                                                            DPNA_DATATYPE_STRING ) ) )
            {
                qWarning() << "Failed to add component!";
                return hr;
            }

            return hr;
        }

        HRESULT CFs9Client::connectToSession(const QString &callsign)
        {
            HRESULT hr = S_OK;

            if(m_clientStatus == Connected) return hr;

            DPN_APPLICATION_DESC dpAppDesc;

            QMutexLocker locker(&m_mutexHostList);

            QScopedArrayPointer<wchar_t> wszPlayername(new wchar_t[callsign.size() + 1]);

            callsign.toWCharArray(wszPlayername.data());
            wszPlayername[callsign.size()] = 0;

            PLAYER_INFO_STRUCT playerInfo;
            ZeroMemory(&playerInfo, sizeof (PLAYER_INFO_STRUCT) );
                strcpy (playerInfo.szAircraft, "Boeing 737-400 Paint1");
            playerInfo.dwFlags = 6;

            // Prepare and set the player information structure.
            DPN_PLAYER_INFO player;
            ZeroMemory( &player, sizeof( DPN_PLAYER_INFO ) );
            player.dwSize = sizeof( DPN_PLAYER_INFO );
            player.pvData = &playerInfo;
            player.dwDataSize = sizeof( PLAYER_INFO_STRUCT );
            player.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
            player.pwszName = wszPlayername.data();
            if( FAILED( hr = m_directPlayPeer->SetPeerInfo( &player, nullptr, nullptr, DPNSETPEERINFO_SYNC ) ) )
            {
                qWarning() << "Failed to set peer info!";
                return hr;
            }

            // Now set up the Application Description
            ZeroMemory(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
            dpAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
            dpAppDesc.guidApplication = CFs9Sdk::guid();


            // We are now ready to host the app
            if( FAILED( hr = m_directPlayPeer->Connect( &dpAppDesc,    // AppDesc
                                                        m_hostAddress,
                                                        m_deviceAddress,
                                                        nullptr,
                                                        nullptr,
                                                        nullptr, 0,
                                                        nullptr,
                                                        nullptr,
                                                        nullptr,
                                                        DPNCONNECT_SYNC ) ) )
            {
                qWarning() << "Failed to connect to host!";
                return hr;
            }

            MPChangePlayerPlane mpChangePlayerPlane;
            mpChangePlayerPlane.engine = CFs9Sdk::ENGINE_TYPE_JET;
            mpChangePlayerPlane.aircraft_name = "Boeing 737-400";
            QByteArray message;
            MultiPlayerPacketParser::writeType(message, CFs9Sdk::MULTIPLAYER_PACKET_ID_CHANGE_PLAYER_PLANE);
            MultiPlayerPacketParser::writeSize(message, mpChangePlayerPlane.aircraft_name.size() + 1);
            message = MultiPlayerPacketParser::writeMessage(message, mpChangePlayerPlane);
            sendMessage(message);

            m_timerId = startTimer(m_updateInterval.value(CTimeUnit::ms()));

            m_clientStatus = Connected;
            emit statusChanged(m_clientStatus);

            return hr;
        }

        HRESULT CFs9Client::closeConnection()
        {
            HRESULT hr = S_OK;

            if (m_clientStatus == Disconnected) return hr;

            qDebug() << "Closing connection for " << m_callsign;
            if( FAILED( hr = m_directPlayPeer->Close(0) ))
            {
                qWarning() << "Failed to close connection!";
            }

            m_clientStatus = Disconnected;
            emit statusChanged(m_clientStatus);
            return hr;
        }
    }
}
