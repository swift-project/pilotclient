/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_VOICE_CHANNEL_VATLIB_H
#define BLACKCORE_VOICE_CHANNEL_VATLIB_H

#include "blackcoreexport.h"
#include "voice_channel.h"
#include "voice_vatlib.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/callsignset.h"
#include <QObject>
#include <QSharedPointer>

//! \file

namespace BlackCore
{
    class CVoiceChannelVatlibPrivate;

    //! Class implementing the voice channel interface
    class BLACKCORE_EXPORT CVoiceChannelVatlib : public IVoiceChannel
    {
        Q_OBJECT

    public:

        //! Default constructor
        CVoiceChannelVatlib(VatAudioService audioService, VatUDPAudioPort udpPort, QObject *parent = nullptr);

        //! Destructor
        virtual ~CVoiceChannelVatlib();

        //! \copydoc IVoiceChannel::joinVoiceRoom
        virtual void joinVoiceRoom(const BlackMisc::Audio::CVoiceRoom &voiceRoom) override;

        //! \copydoc IVoiceChannel::leaveVoiceRoom
        virtual void leaveVoiceRoom() override;

        //! \copydoc IVoiceChannel::getVoiceRoomCallsigns
        virtual BlackMisc::Aviation::CCallsignSet getVoiceRoomCallsigns() const override;

        //! \copydoc IVoiceChannel::setMyAircraftCallsign
        virtual void setOwnAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IVoiceChannel::getVoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom() const override;

        //! \copydoc IVoiceChannel::isMuted
        virtual bool isMuted() const override;

        //! Set channel volume
        virtual void setVolume(int volume) override;

        //! Get channel volume
        virtual int getVolume() const override;

        //! Get vatlib channel pointer
        VatVoiceChannel getVoiceChannel() const;

    private:

        struct VatVoiceChannelDeleter
        {
            static inline void cleanup(VatProducerConsumer_tag *obj)
            {
                if (obj) Vat_DestroyVoiceChannel(obj);
            }
        };

        BlackMisc::Aviation::CCallsign extractCallsign(const QString &name);

        void userJoinedVoiceRoom(VatVoiceChannel, int id, const char *name);
        void userLeftVoiceRoom(VatVoiceChannel, int id, const char *name);
        void transmissionChanged(VatVoiceChannel, VatVoiceTransmissionStatus status);
        void updateRoomStatus(VatVoiceChannel channel, VatConnectionStatus oldStatus, VatConnectionStatus newStatus);

        static void processUserJoined(VatVoiceChannel channel, int id, const char *name, void *cbVar);
        static void processUserLeft(VatVoiceChannel channel, int id, const char *name, void *cbVar);
        static void processTransmissionChange(VatVoiceChannel channel, VatVoiceTransmissionStatus status, void *cbVar);

        static void roomStatusUpdate(VatVoiceChannel channel, VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void *cbVar);

        BlackMisc::Aviation::CCallsign m_callsign; // Own callsign
        BlackMisc::Audio::CVoiceRoom m_voiceRoom; // Voice Room
        BlackMisc::Aviation::CCallsignSet m_listCallsigns; // Callsigns connected to room
        IVoiceChannel::ConnectionStatus m_roomStatus = IVoiceChannel::Disconnected; // Room connection status

        VatAudioService m_audioService;
        VatUDPAudioPort m_udpPort;

        QScopedPointer<VatProducerConsumer_tag, VatVoiceChannelDeleter> m_voiceChannel;
    };
}

#endif // guard
