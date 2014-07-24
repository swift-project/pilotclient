/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_VOICE_CHANNEL_P_H
#define BLACKCORE_VOICE_CHANNEL_P_H

#include "voice_channel_vatlib.h"
#include "blackmisc/voiceroom.h"
#include "blackmisc/avcallsign.h"
#include <QObject>
#include <QMutex>
#include <atomic>

//! \file

namespace BlackCore
{
    // Inhibit doxygen warnings about missing documentation
    //! \cond PRIVATE

    // Private Implementation of CVoiceChannelVatlib
    class CVoiceChannelVatlibPrivate : public QObject
    {
        Q_OBJECT

    public:

        // Default constructor
        CVoiceChannelVatlibPrivate(TVatlibPointer vatlib, CVoiceChannelVatlib *parent);

        // Destructor
        ~CVoiceChannelVatlibPrivate();

        // Enable or disable channel audio output
        void switchAudioOutput(bool enable);

        // Start transmitting
        void startTransmitting();

        // Stop transmitting
        void stopTransmitting();

        // Set room output volume
        void setRoomOutputVolume(const qint32 volume);

        // FIXME Move into free function
        void exceptionDispatcher(const char *caller);

        // Update connected room users
        static void updateRoomUsers(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);

    public slots:

        // Process user joined/left signals
        void processUserJoinedLeft();

    signals:

        // Signal when user has joined or left
        void userJoinedLeft();

    public:

        // Add a callsign temporarily. This is used for the getUserList callback
        void addTemporaryCallsignForRoom(const BlackMisc::Aviation::CCallsign &callsign);

        // Get shared room data for this channel
        static QHash<CVoiceChannelVatlib * const, QSharedPointer<CVoiceChannelVatlibPrivate>> &getSharedRoomData();

        TVatlibPointer m_vatlib; // Shared pointer to vatlib object
        BlackMisc::Aviation::CCallsign m_callsign; // Own callsign
        std::atomic<qint32> m_roomIndex; // Room index
        BlackMisc::Audio::CVoiceRoom m_voiceRoom; // Voice Room
        std::atomic<qint32> m_volume; // Room volume
        std::atomic<quint16> m_connectionRefCount; // Connection reference couting
        std::atomic<bool> m_outputEnabled; // Is room output enabled?
        BlackMisc::Aviation::CCallsignList m_listCallsigns; // Callsigns connected to room
        std::atomic<IVoiceChannel::ConnectionStatus> m_roomStatus; // Room connection status

        // Mutexes
        QMutex m_mutexSharedRoomData;
        QMutex m_mutexCallSign;
        QMutex m_mutexVoiceRoom;
        QMutex m_mutexCallsignList;

        BlackMisc::Aviation::CCallsignList m_temporaryVoiceRoomCallsigns; // temp. storage of voice rooms during update

        static QList<qint32> m_availableRooms; // Static list of not used room indexes
        static QHash<CVoiceChannelVatlib * const, QSharedPointer<CVoiceChannelVatlibPrivate>> m_sharedRoomData;
        const static qint32 InvalidRoomIndex = -1; // Invalid room index

        CVoiceChannelVatlib * const q_ptr = nullptr;
        Q_DECLARE_PUBLIC(CVoiceChannelVatlib)

    private:

        // Change room connection status
        void changeConnectionStatus(IVoiceChannel::ConnectionStatus newStatus);

        // Allocate a not used room identified by its index
        qint32 allocateRoom();
    };
    //! \endcond
}

#endif // guard
