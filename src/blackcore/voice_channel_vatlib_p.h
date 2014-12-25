/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
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

        struct VatVoiceChannelDeleter
        {
            static inline void cleanup(VatProducerConsumer_tag *obj)
            {
                if (!obj) Vat_DestroyVoiceChannel(obj);
            }
        };

        // Default constructor
        CVoiceChannelVatlibPrivate(VatAudioService audioService, VatUDPAudioPort udpPort, CVoiceChannelVatlib *parent);

        // Destructor
        ~CVoiceChannelVatlibPrivate();

        // Set room output volume
        void setRoomOutputVolume(int volume);

        void updateRoomStatus(VatVoiceChannel channel, VatConnectionStatus /** oldStatus **/, VatConnectionStatus newStatus);

        BlackMisc::Aviation::CCallsign extractCallsign(const QString &name);

        static void processUserJoined(VatVoiceChannel channel, int id, const char *name, void *cbVar);
        static void processUserLeft(VatVoiceChannel channel, int id, const char *name, void *cbVar);
        static void processTransmissionChange(VatVoiceChannel channel, VatVoiceTransmissionStatus status, void *cbVar);

        static void onRoomStatusUpdate(VatVoiceChannel channel, VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void *cbVar);

        // Get shared room data for this channel
        static QHash<CVoiceChannelVatlib * const, QSharedPointer<CVoiceChannelVatlibPrivate>> &getSharedRoomData();

        BlackMisc::Aviation::CCallsign m_callsign; // Own callsign
        BlackMisc::Audio::CVoiceRoom m_voiceRoom; // Voice Room
        std::atomic<quint16> m_connectionRefCount; // Connection reference couting
        std::atomic<bool> m_outputEnabled; // Is room output enabled?
        BlackMisc::Aviation::CCallsignList m_listCallsigns; // Callsigns connected to room
        std::atomic<IVoiceChannel::ConnectionStatus> m_roomStatus; // Room connection status
        QList<CVoiceChannelVatlib *> m_voiceChannels;

        VatAudioService m_audioService;
        VatUDPAudioPort m_udpPort;
        QScopedPointer<VatProducerConsumer_tag, VatVoiceChannelDeleter> m_voiceChannel;

        static QHash<CVoiceChannelVatlib * const, QSharedPointer<CVoiceChannelVatlibPrivate>> m_sharedRoomData;
        const static qint32 InvalidRoomIndex = -1; // Invalid room index

        CVoiceChannelVatlib * const q_ptr = nullptr;
        Q_DECLARE_PUBLIC(CVoiceChannelVatlib)

    private:

        void userJoinedVoiceRoom(VatVoiceChannel, int id, const char *name);
        void userLeftVoiceRoom(VatVoiceChannel, int id, const char *name);
        void transmissionChanged(VatVoiceChannel, VatVoiceTransmissionStatus status);
    };
    //! \endcond
}

#endif // guard
