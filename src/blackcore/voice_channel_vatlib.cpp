/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "voice_channel_vatlib.h"
#include "voice_channel_vatlib_p.h"
#include "blackmisc/logmessage.h"

#include <mutex>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    // Room data hash shared between all CVoiceChannel objects
    QHash<CVoiceChannelVatlib * const, QSharedPointer<CVoiceChannelVatlibPrivate>> CVoiceChannelVatlibPrivate::m_sharedRoomData;

    // Constructor
    // Don't set the QObject parent. It will conflict with @QSharedPointer@ memory management
    CVoiceChannelVatlibPrivate::CVoiceChannelVatlibPrivate(VatAudioService audioService, VatUDPAudioPort udpPort, CVoiceChannelVatlib *parent)
        : m_audioService(audioService),
          m_udpPort(udpPort),
          q_ptr(parent)
    {
        Q_ASSERT(m_audioService);
        Q_ASSERT(m_udpPort);

        m_connectionRefCount = 0;
        m_roomStatus = IVoiceChannel::Disconnected;

        m_voiceChannels.push_back(parent);

        m_voiceChannel.reset(Vat_CreateVoiceChannel(m_audioService, "", 3782, "", "", m_udpPort));
        Vat_SetConnectionChangedHandler(m_voiceChannel.data(), onRoomStatusUpdate, this);
        Vat_SetClientJoinedHandler(m_voiceChannel.data(), processUserJoined, this);
        Vat_SetClientLeftHandler(m_voiceChannel.data(), processUserLeft, this);
        Vat_SetVoiceTransmissionChangedHandler(m_voiceChannel.data(), processTransmissionChange, this);
    }

    CVoiceChannelVatlibPrivate::~CVoiceChannelVatlibPrivate()
    {
    }

    void CVoiceChannelVatlibPrivate::setRoomOutputVolume(int volume)
    {
        // FIXME
        Q_UNUSED(volume)
    }

    void CVoiceChannelVatlibPrivate::updateRoomStatus(VatVoiceChannel /* channel */, VatConnectionStatus /** oldVatStatus **/, VatConnectionStatus newVatStatus)
    {
        IVoiceChannel::ConnectionStatus oldStatus = m_roomStatus;
        switch (newVatStatus)
        {
        case vatStatusConnecting:
        {
            m_roomStatus = IVoiceChannel::Connecting;
            break;
        }
        case vatStatusConnected:
        {
            m_voiceRoom.setConnected(true);
            m_roomStatus = IVoiceChannel::Connected;
            break;
        }
        case vatStatusDisconnecting:
        {
            m_roomStatus = IVoiceChannel::Disconnecting;
            break;
        }
        case vatStatusDisconnected:
        {
            // Clear all internals
            m_listCallsigns.clear();
            m_voiceRoom = {};
            m_roomStatus = IVoiceChannel::Disconnected;
            break;
        }
        default:
            break;
        }
        emit q_ptr->connectionStatusChanged(oldStatus, m_roomStatus);
    }

    void CVoiceChannelVatlibPrivate::userJoinedVoiceRoom(VatVoiceChannel, int /** id **/, const char *name)
    {
        CCallsign callsign(extractCallsign(name));
        m_listCallsigns.push_back(callsign);

        for (const auto &e : m_voiceChannels)
            emit e->userJoinedRoom(callsign);
    }

    void CVoiceChannelVatlibPrivate::userLeftVoiceRoom(VatVoiceChannel, int /** id **/, const char *name)
    {
        CCallsign callsign(extractCallsign(name));
        m_listCallsigns.remove(callsign);

        for (const auto &e : m_voiceChannels)
            emit e->userLeftRoom(callsign);
    }

    void CVoiceChannelVatlibPrivate::transmissionChanged(VatVoiceChannel, VatVoiceTransmissionStatus status)
    {
        if (status == vatVoiceStarted)
            emit q_ptr->audioStarted();
        else
            emit q_ptr->audioStopped();
    }

    CVoiceChannelVatlibPrivate *cbvar_cast_voiceChannelPrivate(void *cbvar)
    {
        return static_cast<CVoiceChannelVatlibPrivate *>(cbvar);
    }

    CCallsign CVoiceChannelVatlibPrivate::extractCallsign(const QString &name)
    {
        CCallsign callsign;
        if (name.isEmpty()) return callsign;

        // callsign might contain: VATSIM id, user name
        if (name.contains(" "))
        {
            QStringList parts = name.split(" ");
            callsign = CCallsign(parts[0]);
            // I throw away VATSIM id here, maybe we could use it
        }
        else
        {
            callsign = CCallsign(name);
        }

        return callsign;
    }

    void CVoiceChannelVatlibPrivate::processUserJoined(VatVoiceChannel channel, int id, const char *name, void *cbVar)
    {
        CVoiceChannelVatlibPrivate *voiceChannelPrivate = cbvar_cast_voiceChannelPrivate(cbVar);
        voiceChannelPrivate->userJoinedVoiceRoom(channel, id, name);
    }

    void CVoiceChannelVatlibPrivate::processUserLeft(VatVoiceChannel channel, int id, const char *name, void *cbVar)
    {
        CVoiceChannelVatlibPrivate *voiceChannelPrivate = cbvar_cast_voiceChannelPrivate(cbVar);
        voiceChannelPrivate->userLeftVoiceRoom(channel, id, name);
    }

    void CVoiceChannelVatlibPrivate::processTransmissionChange(VatVoiceChannel channel, VatVoiceTransmissionStatus status, void *cbVar)
    {
        CVoiceChannelVatlibPrivate *voiceChannelPrivate = cbvar_cast_voiceChannelPrivate(cbVar);

        voiceChannelPrivate->transmissionChanged(channel, status);
    }

    /*
     * Room status update
     */
    void CVoiceChannelVatlibPrivate::onRoomStatusUpdate(VatVoiceChannel channel, VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void *cbVar)
    {
        auto obj = cbvar_cast_voiceChannelPrivate(cbVar);
        obj->updateRoomStatus(channel, oldStatus, newStatus);
    }

    // Get shared room data
    QHash<CVoiceChannelVatlib * const, QSharedPointer<CVoiceChannelVatlibPrivate>> &CVoiceChannelVatlibPrivate::getSharedRoomData()
    {
        return m_sharedRoomData;
    }

    // Constructor
    CVoiceChannelVatlib::CVoiceChannelVatlib(VatAudioService audioService, VatUDPAudioPort udpPort, QObject *parent)
        : IVoiceChannel(parent),
          d_ptr(new CVoiceChannelVatlibPrivate(audioService, udpPort, this))
    {
    }

    // Destructor
    CVoiceChannelVatlib::~CVoiceChannelVatlib()
    {
        d_ptr->m_voiceChannels.removeAll(this);
    }

    // Join room
    void CVoiceChannelVatlib::joinVoiceRoom(const CVoiceRoom &voiceRoom)
    {
        // Find if a different channel is connected already to this voice room
        auto roomDataList = CVoiceChannelVatlibPrivate::getSharedRoomData().values();
        auto iterator = std::find_if(roomDataList.begin(), roomDataList.end(), [&](const QSharedPointer<CVoiceChannelVatlibPrivate> roomData)
        {
            return roomData->m_voiceRoom.getVoiceRoomUrl() == voiceRoom.getVoiceRoomUrl();
        });

        // If we found another channel
        if (iterator != roomDataList.end())
        {
            // Increase the connection reference counter
            (*iterator)->m_connectionRefCount++;

            d_ptr = (*iterator);

            // Assign shared room data to this channel index
            CVoiceChannelVatlibPrivate::getSharedRoomData().insert(this, *iterator);

            // Add ourselfes as listener
            d_ptr->m_voiceChannels.push_back(this);


            // Since the room is used already, we have to simulate the state changes
            emit connectionStatusChanged(IVoiceChannel::Disconnected, IVoiceChannel::Connecting);
            emit connectionStatusChanged(IVoiceChannel::Connecting, IVoiceChannel::Connected);
        }
        else
        {
            // No one else is using this voice room, so prepare to join
            d_ptr->m_voiceRoom = voiceRoom;
            Vat_SetRoomInfo(d_ptr->m_voiceChannel.data(), qPrintable(voiceRoom.getHostname()), 3782,
                            qPrintable(voiceRoom.getChannel()),
                            qPrintable(d_ptr->m_callsign.toQString()));

            d_ptr->m_roomStatus = IVoiceChannel::Disconnected;
            Vat_JoinRoom(d_ptr->m_voiceChannel.data());

            CVoiceChannelVatlibPrivate::m_sharedRoomData.insert(this, d_ptr);
            ++d_ptr->m_connectionRefCount;
        }

    }

    // Leave room
    void CVoiceChannelVatlib::leaveVoiceRoom()
    {
        // If this room is not connected, there is nothing to do
        if (d_ptr->m_roomStatus == IVoiceChannel::Disconnecting || d_ptr->m_roomStatus == IVoiceChannel::Disconnected) return;

        // Decrease the connection reference counter
        --d_ptr->m_connectionRefCount;

        // If this was the last channel, connected to the room, leave it.
        if (d_ptr->m_connectionRefCount == 0)
        {
            qDebug() << "Leaving voice room!";
            Vat_DisconnectFromRoom(d_ptr->m_voiceChannel.data());
        }
        else
        {
            d_ptr->m_voiceChannels.removeAll(this);

            // We need to assign a private class
            // This automatically clears callsign list etc.
            VatAudioService audioService = d_ptr->m_audioService;
            VatUDPAudioPort udpPort = d_ptr->m_udpPort;

            d_ptr.reset(new CVoiceChannelVatlibPrivate(audioService, udpPort, this));
            CVoiceChannelVatlibPrivate::getSharedRoomData().insert(this, d_ptr);

            // Simulate the state change
            emit connectionStatusChanged(IVoiceChannel::Connected, IVoiceChannel::Disconnecting);
            emit connectionStatusChanged(IVoiceChannel::Disconnecting, IVoiceChannel::Disconnected);
        }
    }

    CCallsignList CVoiceChannelVatlib::getVoiceRoomCallsigns() const
    {
        return d_ptr->m_listCallsigns;
    }

    void CVoiceChannelVatlib::setMyAircraftCallsign(const CCallsign &callsign)
    {
        d_ptr->m_callsign = callsign;
    }

    BlackMisc::Audio::CVoiceRoom CVoiceChannelVatlib::getVoiceRoom() const
    {
        return d_ptr->m_voiceRoom;
    }

    bool CVoiceChannelVatlib::isMuted() const
    {
        return !d_ptr->m_outputEnabled;
    }

    void CVoiceChannelVatlib::setVolume(int volume)
    {
        d_ptr->setRoomOutputVolume(volume);
    }

    int CVoiceChannelVatlib::getVolume() const
    {
        // FIXME
        return 100;
    }

    VatVoiceChannel CVoiceChannelVatlib::getVoiceChannel() const
    {
        return d_ptr->m_voiceChannel.data();
    }
}
