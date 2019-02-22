/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_VOICE_CHANNEL_VATLIB_H
#define BLACKCORE_VATSIM_VOICE_CHANNEL_VATLIB_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/voicechannel.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/callsignset.h"
#include "vatlib/vatlib.h"

#include <stdbool.h>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace BlackCore
{
    namespace Vatsim
    {
        class CVoiceChannelVatlibPrivate;

        //! Class implementing the voice channel interface
        class BLACKCORE_EXPORT CVoiceChannelVatlib : public IVoiceChannel
        {
            Q_OBJECT

        public:

            //! Default constructor
            CVoiceChannelVatlib(VatAudioService *audioService, VatUDPAudioPort *udpPort, QObject *parent = nullptr);

            //! Destructor
            virtual ~CVoiceChannelVatlib();

            //! \copydoc IVoiceChannel::joinVoiceRoom
            virtual void joinVoiceRoom(const BlackMisc::Audio::CVoiceRoom &voiceRoom) override;

            //! \copydoc IVoiceChannel::leaveVoiceRoom
            virtual void leaveVoiceRoom() override;

            //! \copydoc IVoiceChannel::getVoiceRoomCallsigns
            virtual BlackMisc::Aviation::CCallsignSet getVoiceRoomCallsigns() const override;

            //! \copydoc IVoiceChannel::setOwnAircraftCallsign
            virtual void setOwnAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc IVoiceChannel::setUserId
            virtual void setUserId(const QString &id) override;

            //! \copydoc IVoiceChannel::getVoiceRoom
            virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom() const override;

            //! \copydoc IVoiceChannel::isMuted
            virtual bool isMuted() const override;

            //! Set channel volume
            virtual void setVolume(int volume) override;

            //! Get channel volume
            virtual int getVolume() const override;

            //! Get vatlib channel pointer
            VatVoiceChannel *getVoiceChannel() const;

        private:

            struct VatVoiceChannelDeleter
            {
                static inline void cleanup(VatVoiceChannel *obj)
                {
                    if (obj) Vat_DestroyVoiceChannel(obj);
                }
            };

            BlackMisc::Aviation::CCallsign extractCallsign(const QString &name);

            void userJoinedVoiceRoom(VatVoiceChannel *, int id, const char *name);
            void userLeftVoiceRoom(VatVoiceChannel *, int id, const char *name);
            void voiceReceptionChanged(VatVoiceChannel *, bool isVoiceReceiving);
            void updateRoomStatus(VatVoiceChannel *channel, VatConnectionStatus oldStatus, VatConnectionStatus newStatus);

            static void processUserJoined(VatVoiceChannel *channel, int id, const char *name, void *cbVar);
            static void processUserLeft(VatVoiceChannel *channel, int id, const char *name, void *cbVar);
            static void processVoiceReceptionChanged(VatVoiceChannel *channel, bool isVoiceReceiving, void *cbVar);

            static void roomStatusUpdate(VatVoiceChannel *channel, VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void *cbVar);

            BlackMisc::Aviation::CCallsign m_callsign; // Own callsign
            QString m_userId; // Pilot user id
            BlackMisc::Audio::CVoiceRoom m_voiceRoom; // Voice Room
            BlackMisc::Aviation::CCallsignSet m_listCallsigns; // Callsigns connected to room
            IVoiceChannel::ConnectionStatus m_roomStatus = IVoiceChannel::Disconnected; // Room connection status

            QScopedPointer<VatVoiceChannel, VatVoiceChannelDeleter> m_voiceChannel;
        };
    } // ns
} // ns

#endif // guard
