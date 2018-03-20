/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#define _CRT_SECURE_NO_WARNINGS

#include "fs9client.h"
#include "multiplayerpackets.h"
#include "multiplayerpacketparser.h"
#include "directplayerror.h"
#include "directplayutils.h"
#include "blackcore/simulator.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/logmessage.h"
#include <QScopedArrayPointer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackCore;

namespace BlackSimPlugin
{
    namespace Fs9
    {
        CFs9Client::CFs9Client(const CCallsign &callsign, const QString &modelName, const CTime &updateInterval,
                               CInterpolationLogger *logger, ISimulator *owner) :
            CDirectPlayPeer(owner, callsign),
            m_updateInterval(updateInterval),
            m_interpolator(callsign),
            m_modelName(modelName)
        {
            m_interpolator.attachLogger(logger);
            Q_ASSERT_X(this->simulator(), Q_FUNC_INFO, "Wrong owner, expect simulator object");
        }

        MPPositionVelocity aircraftSituationToFS9(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, double updateInterval)
        {
            MPPositionVelocity positionVelocity;

            // Latitude - integer and decimal places
            const double latitude = newSituation.getPosition().latitude().value(CAngleUnit::deg()) * 10001750.0 / 90.0;
            positionVelocity.lat_i = static_cast<qint32>(latitude);
            positionVelocity.lat_f = qAbs((latitude - positionVelocity.lat_i) * 65536);

            // Longitude - integer and decimal places
            const double longitude = newSituation.getPosition().longitude().value(CAngleUnit::deg()) * (65536.0 * 65536.0) / 360.0;
            positionVelocity.lon_hi = static_cast<qint32>(longitude);
            positionVelocity.lon_lo = qAbs((longitude - positionVelocity.lon_hi) * 65536);

            // Altitude - integer and decimal places
            const double altitude = newSituation.getAltitude().value(CLengthUnit::m());
            positionVelocity.alt_i = static_cast<qint32>(altitude);
            positionVelocity.alt_f = (altitude - positionVelocity.alt_i) * 65536;

            // Pitch, Bank and Heading
            FS_PBH pbhstrct;
            pbhstrct.hdg = newSituation.getHeading().value(CAngleUnit::deg()) * CFs9Sdk::headingMultiplier();
            pbhstrct.pitch = std::floor(newSituation.getPitch().value(CAngleUnit::deg()) * CFs9Sdk::pitchMultiplier());
            pbhstrct.bank = std::floor(newSituation.getBank().value(CAngleUnit::deg()) * CFs9Sdk::bankMultiplier());
            // MSFS has inverted pitch and bank angles
            pbhstrct.pitch = ~pbhstrct.pitch;
            pbhstrct.bank = ~pbhstrct.bank;
            positionVelocity.pbh = pbhstrct.pbh;

            // Ground velocity
            positionVelocity.ground_velocity = newSituation.getGroundSpeed().value(CSpeedUnit::m_s());

            // Altitude velocity
            CCoordinateGeodetic oldPosition = oldSituation.getPosition();
            CCoordinateGeodetic newPosition = newSituation.getPosition();
            CCoordinateGeodetic helperPosition;

            // We want the distance in Latitude direction. Longitude must be equal for old and new position.
            helperPosition.setLatitude(newPosition.latitude());
            helperPosition.setLongitude(oldPosition.longitude());
            const CLength distanceLatitudeObj = calculateGreatCircleDistance(oldPosition, helperPosition);

            // Now we want the Longitude distance. Latitude must be equal for old and new position.
            helperPosition.setLatitude(oldPosition.latitude());
            helperPosition.setLongitude(newSituation.longitude());
            const CLength distanceLongitudeObj = calculateGreatCircleDistance(oldPosition, helperPosition);

            // Latitude and Longitude velocity
            positionVelocity.lat_velocity = distanceLatitudeObj.value(CLengthUnit::ft()) * 65536.0 / updateInterval;
            if (oldPosition.latitude().value() > newSituation.latitude().value()) positionVelocity.lat_velocity *= -1;
            positionVelocity.lon_velocity = distanceLongitudeObj.value(CLengthUnit::ft()) * 65536.0 / updateInterval;
            if (oldPosition.longitude().value() > newSituation.longitude().value()) positionVelocity.lon_velocity *= -1;

            return positionVelocity;
        }

        MPPositionSlewMode aircraftSituationToFS9(const CAircraftSituation &situation)
        {
            MPPositionSlewMode positionSlewMode;

            // Latitude - integer and decimal places
            const double latitude = situation.getPosition().latitude().value(CAngleUnit::deg()) * 10001750.0 / 90.0;
            positionSlewMode.lat_i = static_cast<qint32>(latitude);
            positionSlewMode.lat_f = qAbs((latitude - positionSlewMode.lat_i) * 65536);

            // Longitude - integer and decimal places
            const double longitude = situation.getPosition().longitude().value(CAngleUnit::deg()) * (65536.0 * 65536.0) / 360.0;
            positionSlewMode.lon_hi = static_cast<qint32>(longitude);
            positionSlewMode.lon_lo = qAbs((longitude - positionSlewMode.lon_hi) * 65536);

            // Altitude - integer and decimal places
            double altitude = situation.getAltitude().value(CLengthUnit::m());
            positionSlewMode.alt_i = static_cast<qint32>(altitude);
            positionSlewMode.alt_f = (altitude - positionSlewMode.alt_i) * 65536;

            // Pitch, Bank and Heading
            FS_PBH pbhstrct;
            pbhstrct.hdg = situation.getHeading().value(CAngleUnit::deg()) * CFs9Sdk::headingMultiplier();
            pbhstrct.pitch = std::floor(situation.getPitch().value(CAngleUnit::deg()) * CFs9Sdk::pitchMultiplier());
            pbhstrct.bank = std::floor(situation.getBank().value(CAngleUnit::deg()) * CFs9Sdk::bankMultiplier());
            // MSFS has inverted pitch and bank angles
            pbhstrct.pitch = ~pbhstrct.pitch;
            pbhstrct.bank = ~pbhstrct.bank;
            positionSlewMode.pbh = pbhstrct.pbh;

            return positionSlewMode;
        }

        CFs9Client::~CFs9Client()
        {
            SafeRelease(m_hostAddress);
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
                logDirectPlayError(hr);
                return;
            }

            if (FAILED(hr = m_hostAddress->BuildFromURLA(hostAddress.toLatin1().data())))
            {
                logDirectPlayError(hr);
                return;
            }
        }

        void CFs9Client::timerEvent(QTimerEvent *event)
        {
            Q_UNUSED(event);

            if (m_clientStatus == Disconnected) { return; }
            CInterpolationStatus status;
            CInterpolationAndRenderingSetupPerCallsign setup = this->simulator()->getInterpolationSetupPerCallsignOrDefault(m_callsign);
            const CAircraftSituation situation = m_interpolator.getInterpolatedSituation(-1, setup, status);

            // Test only for successful position. FS9 requires constant positions
            if (!status.hasValidSituation()) { return; }

            sendMultiplayerPosition(situation);
            sendMultiplayerParamaters();
        }

        void CFs9Client::initialize()
        {
            initDirectPlay();
            createDeviceAddress();
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
            DPN_APPLICATION_DESC dpAppDesc;
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
                return logDirectPlayError(hr);
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
                return logDirectPlayError(hr);
            }

            // Set the SP for our Host Address
            if (FAILED(hr = m_hostAddress->SetSP(&CLSID_DP8SP_TCPIP)))
            {
                return logDirectPlayError(hr);
            }

            // FIXME: Test if this is also working via network or if we have to use the IP address
            const wchar_t hostname[] = L"localhost";

            // Set the hostname into the address
            if (FAILED(hr = m_hostAddress->AddComponent(DPNA_KEY_HOSTNAME, hostname,
                            2 * (wcslen(hostname) + 1), /*bytes*/
                            DPNA_DATATYPE_STRING)))
            {
                return logDirectPlayError(hr);
            }

            return hr;
        }

        HRESULT CFs9Client::connectToSession(const CCallsign &callsign)
        {
            HRESULT hr = S_OK;
            if (m_clientStatus == Connected) { return hr; }

            QMutexLocker locker(&m_mutexHostList);

            QScopedArrayPointer<wchar_t> wszPlayername(new wchar_t[callsign.toQString().size() + 1]);
            callsign.toQString().toWCharArray(wszPlayername.data());
            wszPlayername[callsign.toQString().size()] = 0;

            Q_ASSERT(!m_modelName.isEmpty());
            ZeroMemory(&m_playerInfo, sizeof(PLAYER_INFO_STRUCT));
            strcpy(m_playerInfo.szAircraft, qPrintable(m_modelName));
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
                return logDirectPlayError(hr);
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
                return logDirectPlayError(hr);
            }


            CLogMessage(this).debug() << m_callsign << " connected to session.";
            sendMultiplayerChangePlayerPlane();

            m_timerId = startTimer(m_updateInterval.value(CTimeUnit::ms()));

            m_clientStatus = Connected;
            emit statusChanged(m_callsign, m_clientStatus);

            return hr;
        }

        HRESULT CFs9Client::closeConnection()
        {
            HRESULT hr = S_OK;

            if (m_clientStatus == Disconnected) { return hr; }
            CLogMessage(this).debug() << "Closing DirectPlay connection for " << m_callsign;
            if (FAILED(hr = m_directPlayPeer->Close(0)))
            {
                return logDirectPlayError(hr);
            }

            m_clientStatus = Disconnected;
            emit statusChanged(m_callsign, m_clientStatus);
            return hr;
        }

        void CFs9Client::sendMultiplayerPosition(const CAircraftSituation &situation)
        {
            MPPositionSlewMode positionSlewMode = aircraftSituationToFS9(situation);

            QByteArray positionMessage;
            MultiPlayerPacketParser::writeType(positionMessage, CFs9Sdk::MULTIPLAYER_PACKET_ID_POSITION_SLEWMODE);
            MultiPlayerPacketParser::writeSize(positionMessage, positionSlewMode.size());
            positionSlewMode.packet_index = m_packetIndex;
            ++m_packetIndex;
            positionMessage = MultiPlayerPacketParser::writeMessage(positionMessage, positionSlewMode);

            sendMessage(positionMessage);
        }

        void CFs9Client::sendMultiplayerParamaters()
        {
            QByteArray paramMessage;
            MPParam param;
            MultiPlayerPacketParser::writeType(paramMessage, CFs9Sdk::MULTIPLAYER_PACKET_ID_PARAMS);
            MultiPlayerPacketParser::writeSize(paramMessage, param.size());
            param.packet_index = m_packetIndex;
            ++m_packetIndex;
            paramMessage = MultiPlayerPacketParser::writeMessage(paramMessage, param);
            sendMessage(paramMessage);
        }

        void CFs9Client::sendMultiplayerChangePlayerPlane()
        {
            MPChangePlayerPlane mpChangePlayerPlane;
            mpChangePlayerPlane.engine = CFs9Sdk::ENGINE_TYPE_JET;
            mpChangePlayerPlane.aircraft_name = m_modelName;
            QByteArray message;
            MultiPlayerPacketParser::writeType(message, CFs9Sdk::MULTIPLAYER_PACKET_ID_CHANGE_PLAYER_PLANE);
            MultiPlayerPacketParser::writeSize(message, mpChangePlayerPlane.size());
            message = MultiPlayerPacketParser::writeMessage(message, mpChangePlayerPlane);
            sendMessage(message);
        }

        const ISimulator *CFs9Client::simulator() const
        {
            return qobject_cast<const ISimulator *>(this->owner());
        }
    }
}
