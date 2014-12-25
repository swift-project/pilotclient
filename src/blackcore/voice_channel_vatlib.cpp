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

    // Static list of available rooms
    QList<qint32> CVoiceChannelVatlibPrivate::m_availableRooms = {0, 1};

    // Constructor
    // Don't set the QObject parent. It will conflict with @QSharedPointer@ memory management
    CVoiceChannelVatlibPrivate::CVoiceChannelVatlibPrivate(TVatlibPointer vatlib, CVoiceChannelVatlib *parent)
        : m_vatlib(vatlib),
          m_mutexSharedRoomData(QMutex::Recursive),
          m_mutexCallSign(QMutex::Recursive),
          m_mutexVoiceRoom(QMutex::Recursive),
          m_mutexCallsignList(QMutex::Recursive),
          q_ptr(parent)
    {
        m_roomIndex.store(InvalidRoomIndex);
        m_connectionRefCount.store(0);
        m_outputEnabled.store(true);
        m_roomStatus.store(IVoiceChannel::Disconnected);

        connect(this, &CVoiceChannelVatlibPrivate::userJoinedLeft, this, &CVoiceChannelVatlibPrivate::processUserJoinedLeft, Qt::QueuedConnection);
    }

    CVoiceChannelVatlibPrivate::~CVoiceChannelVatlibPrivate()
    {
    }

    void CVoiceChannelVatlibPrivate::changeConnectionStatus(IVoiceChannel::ConnectionStatus newStatus)
    {
        Q_Q(CVoiceChannelVatlib);

        m_roomStatus = newStatus;
        emit q->connectionStatusChanged(m_roomStatus, newStatus);
    }

    void CVoiceChannelVatlibPrivate::switchAudioOutput(bool enable)
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceChannelVatlibPrivate", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_vatlib->IsRoomValid(m_roomIndex), "CVoiceChannelVatlibPrivate", "Room index out of bounds!");

        m_outputEnabled = enable;

        try
        {
            m_vatlib->SetOutputState(m_roomIndex, 0, enable);
        }
        catch (...)
        {
            exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CVoiceChannelVatlibPrivate::startTransmitting()
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceChannelVatlibPrivate", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_vatlib->IsRoomValid(m_roomIndex), "CVoiceChannelVatlibPrivate", "Room index out of bounds!");

        if (m_roomStatus != IVoiceChannel::Connected) return;

        try
        {
            m_vatlib->SetMicState(m_roomIndex, true);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CVoiceChannelVatlibPrivate::stopTransmitting()
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceChannelVatlibPrivate", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_vatlib->IsRoomValid(m_roomIndex), "CVoiceChannelVatlibPrivate", "Room index out of bounds!");

        if (m_roomStatus != IVoiceChannel::Connected) return;

        try
        {
            m_vatlib->SetMicState(m_roomIndex, false);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CVoiceChannelVatlibPrivate::setRoomOutputVolume(const qint32 volume)
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceChannelVatlibPrivate", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_vatlib->IsRoomValid(m_roomIndex), "CVoiceChannelVatlibPrivate", "Room index out of bounds!");

        try
        {
            m_vatlib->SetRoomVolume(m_roomIndex, volume);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    // Forward exception as signal
    void CVoiceChannelVatlibPrivate::exceptionDispatcher(const char *caller)
    {
        Q_Q(CVoiceChannelVatlib);

        QString msg("Caller: ");
        msg.append(caller).append(" ").append("Exception: ");
        try
        {
            throw;
        }
        catch (const NetworkNotConnectedException &e)
        {
            // this could be caused by a race condition during normal operation, so not an error
            CLogMessage(q).debug() << "NetworkNotConnectedException" << e.what() << "in" << caller;
        }
        catch (const VatlibException &e)
        {
            CLogMessage(q).error("VatlibException %1 in %2") << e.what() << caller;
            Q_ASSERT(false);
        }
        catch (const std::exception &e)
        {
            CLogMessage(q).error("std::exception %1 in %2") << e.what() << caller;
            Q_ASSERT(false);
        }
        catch (...)
        {
            CLogMessage(q).error("Unknown exception in %1") << caller;
            Q_ASSERT(false);
        }
    }

    void CVoiceChannelVatlibPrivate::processUserJoinedLeft()
    {
        Q_Q(CVoiceChannelVatlib);

        try
        {
            std::lock_guard<TVatlibPointer> locker(m_vatlib);
            // Paranoia... clear list completely
            if (!m_vatlib->IsRoomConnected(m_roomIndex))
            {
                QMutexLocker lockCallsignList(&m_mutexCallsignList);
                m_listCallsigns.clear();
                return;
            }

            // Callbacks already completed when function GetRoomUserList returns,
            // thereafter m_voiceRoomCallsignsUpdate is filled with the latest callsigns

            m_vatlib->GetRoomUserList(m_roomIndex, updateRoomUsers, this);

            QMutexLocker lockCallsignList(&m_mutexCallsignList);
            // we have all current users in m_temporaryVoiceRoomCallsigns
            foreach(CCallsign callsign, m_listCallsigns)
            {
                if (!m_temporaryVoiceRoomCallsigns.contains(callsign))
                {
                    // User has left
                    emit q->userLeftRoom(callsign);
                }
            }

            foreach(CCallsign callsign, m_temporaryVoiceRoomCallsigns)
            {
                if (!m_listCallsigns.contains(callsign))
                {
                    // he joined
                    emit q->userJoinedRoom(callsign);
                }
            }

            // Finally we update it with our new list
            m_listCallsigns = m_temporaryVoiceRoomCallsigns;
            m_temporaryVoiceRoomCallsigns.clear();
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    CVoiceChannelVatlibPrivate *cbvar_cast_voiceChannelPrivate(void *cbvar)
    {
        return static_cast<CVoiceChannelVatlibPrivate *>(cbvar);
    }

    /*
     * Room user received
     */
    void CVoiceChannelVatlibPrivate::updateRoomUsers(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)

        // sanity check
        QString callsign = QString(name);
        if (callsign.isEmpty()) return;

        // add callsign
        CVoiceChannelVatlibPrivate *voiceChannelPrivate = cbvar_cast_voiceChannelPrivate(cbVar);

        // add user
        // callsign might contain: VATSIM id, user name
        if (callsign.contains(" "))
        {
            QStringList parts = callsign.split(" ");
            callsign = parts[0];
            // I throw away VATSIM id here, maybe we could use it
        }

        voiceChannelPrivate->addTemporaryCallsignForRoom(CCallsign(callsign));
    }

    /*
     * Add temp.callsign for room
     */
    void CVoiceChannelVatlibPrivate::addTemporaryCallsignForRoom(const CCallsign &callsign)
    {
        if (m_temporaryVoiceRoomCallsigns.contains(callsign)) return;
        m_temporaryVoiceRoomCallsigns.push_back(callsign);
    }

    // Get shared room data
    QHash<CVoiceChannelVatlib * const, QSharedPointer<CVoiceChannelVatlibPrivate>> &CVoiceChannelVatlibPrivate::getSharedRoomData()
    {
        return m_sharedRoomData;
    }

    // Allocate a new room
    qint32 CVoiceChannelVatlibPrivate::allocateRoom()
    {
        Q_ASSERT(!m_availableRooms.isEmpty());
        return m_availableRooms.takeFirst();
    }

    // Constructor
    CVoiceChannelVatlib::CVoiceChannelVatlib(TVatlibPointer vatlib, QObject *parent)
        : IVoiceChannel(parent),
          d_ptr(new CVoiceChannelVatlibPrivate(vatlib, this))
    {
    }

    // Destructor
    CVoiceChannelVatlib::~CVoiceChannelVatlib()
    {
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

        try
        {
            // If we found an another channel
            if (iterator != roomDataList.end())
            {
                // Increase the connection reference counter
                (*iterator)->m_connectionRefCount++;

                d_ptr = (*iterator);

                // Assign shared room data to this channel index
                CVoiceChannelVatlibPrivate::getSharedRoomData().insert(this, *iterator);

                // Since the room is used already, we have to simulate the state changes
                emit connectionStatusChanged(IVoiceChannel::Disconnected, IVoiceChannel::Connecting);
                emit connectionStatusChanged(IVoiceChannel::Connecting, IVoiceChannel::Connected);
                emit d_ptr->userJoinedLeft();
            }
            else
            {
                QMutexLocker lockVoiceRoom(&d_ptr->m_mutexVoiceRoom);
                // No one else is using this voice room, so prepare to join
                d_ptr->m_voiceRoom = voiceRoom;
                d_ptr->m_roomIndex = d_ptr->allocateRoom();
                d_ptr->m_roomStatus = IVoiceChannel::Disconnected;

                std::lock_guard<TVatlibPointer> locker(d_ptr->m_vatlib);
                QMutexLocker lockerCallsign(&d_ptr->m_mutexCallSign);
                bool jr = d_ptr->m_vatlib->JoinRoom(d_ptr->m_roomIndex, d_ptr->m_callsign.toQString().toLatin1().constData(),
                                             d_ptr->m_voiceRoom.getVoiceRoomUrl().toLatin1().constData());
                if (!jr) qWarning() << "Could not join voice room";
                CVoiceChannelVatlibPrivate::m_sharedRoomData.insert(this, d_ptr);
                ++d_ptr->m_connectionRefCount;
            }

        }
        catch (...)
        {
            d_ptr->exceptionDispatcher(Q_FUNC_INFO);
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
            try
            {
                qDebug() << "Leaving voice room!";
                if(d_ptr->m_vatlib->IsRoomConnected(d_ptr->m_roomIndex))
                {
                    std::lock_guard<TVatlibPointer> locker(d_ptr->m_vatlib);
                    d_ptr->m_vatlib->LeaveRoom(d_ptr->m_roomIndex);
                    d_ptr->m_availableRooms.append(d_ptr->m_roomIndex);
                }
            }
            catch (...)
            {
                d_ptr->exceptionDispatcher(Q_FUNC_INFO);
            }
        }
        else
        {
            // We need to assign a private class
            // This automatically clears callsign list etc.
            TVatlibPointer vatlib = d_ptr->m_vatlib;
            d_ptr.reset(new CVoiceChannelVatlibPrivate(vatlib, this));
            CVoiceChannelVatlibPrivate::getSharedRoomData().insert(this, d_ptr);

            // Simulate the state change
            d_ptr->changeConnectionStatus(IVoiceChannel::Disconnecting);
            d_ptr->changeConnectionStatus(IVoiceChannel::Disconnected);
        }
    }

    void CVoiceChannelVatlib::startTransmitting()
    {
        d_ptr->startTransmitting();
    }

    void CVoiceChannelVatlib::stopTransmitting()
    {
        d_ptr->stopTransmitting();
    }

    CCallsignList CVoiceChannelVatlib::getVoiceRoomCallsigns() const
    {
        QMutexLocker lockCallsignList(&d_ptr->m_mutexCallsignList);
        return d_ptr->m_listCallsigns;
    }

    void CVoiceChannelVatlib::switchAudioOutput(bool enable)
    {
        d_ptr->switchAudioOutput(enable);
    }

    void CVoiceChannelVatlib::setMyAircraftCallsign(const CCallsign &callsign)
    {
        QMutexLocker lockerCallsign(&d_ptr->m_mutexCallSign);
        d_ptr->m_callsign = callsign;
    }

    BlackMisc::Audio::CVoiceRoom CVoiceChannelVatlib::getVoiceRoom() const
    {
        QMutexLocker lockVoiceRoom(&d_ptr->m_mutexVoiceRoom);
        return d_ptr->m_voiceRoom;
    }

    qint32 CVoiceChannelVatlib::getRoomIndex() const
    {
        return d_ptr->m_roomIndex;
    }

    void CVoiceChannelVatlib::updateRoomStatus(Cvatlib_Voice_Simple::roomStatusUpdate roomStatus)
    {
        switch (roomStatus)
        {
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinSuccess:
        {
            QMutexLocker lockVoiceRoom(&d_ptr->m_mutexVoiceRoom);
            d_ptr->m_voiceRoom.setConnected(true);
            d_ptr->changeConnectionStatus(IVoiceChannel::Connected);
            bool isOutputEnabled = d_ptr->m_outputEnabled;
            switchAudioOutput(isOutputEnabled);
            emit d_ptr->userJoinedLeft();
            break;
        }
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinFail:
        {
            QMutexLocker lockVoiceRoom(&d_ptr->m_mutexVoiceRoom);
            d_ptr->m_voiceRoom.setConnected(false);
            d_ptr->changeConnectionStatus(IVoiceChannel::ConnectingFailed);
            break;
        }
        case Cvatlib_Voice_Simple::roomStatusUpdate_UnexpectedDisconnectOrKicked:
            d_ptr->m_voiceRoom.setConnected(false);
            d_ptr->changeConnectionStatus(IVoiceChannel::DisconnectedError);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_LeaveComplete:
        {
            // Instead of clearing and resetting all internals, we just assign a new default room data
            // The former one will be deallocated automatically.
            TVatlibPointer vatlib = d_ptr->m_vatlib;
            d_ptr.reset(new CVoiceChannelVatlibPrivate(vatlib, this));
            CVoiceChannelVatlibPrivate::getSharedRoomData().insert(this, d_ptr);
            d_ptr->changeConnectionStatus(IVoiceChannel::Disconnected);
            break;
        }
        case Cvatlib_Voice_Simple::roomStatusUpdate_UserJoinsLeaves:
            // FIXME: We cannot call GetRoomUserList because vatlib is not reentrent safe.
            emit d_ptr->userJoinedLeft();
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_AudioStarted:
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_AudioStopped:
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStarted:
            emit audioStarted();
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStopped:
            emit audioStopped();
            break;
        default:
            break;
        }
    }

    bool CVoiceChannelVatlib::isMuted() const
    {
        return !d_ptr->m_outputEnabled;
    }

    void CVoiceChannelVatlib::setVolume(quint32 volume)
    {
        d_ptr->m_volume.store(volume);
        Q_ASSERT_X(d_ptr->m_vatlib->IsValid() && d_ptr->m_vatlib->IsSetup(), "CVoiceChannelVatlibPrivate", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(d_ptr->m_vatlib->IsRoomValid(d_ptr->m_roomIndex.load()), "CVoiceChannelVatlibPrivate", "Room index out of bounds!");

        d_ptr->m_vatlib->SetOutputVolume(d_ptr->m_roomIndex.load(), volume);
    }

    quint32 CVoiceChannelVatlib::getVolume() const
    {
        return d_ptr->m_volume.load();
    }
}
