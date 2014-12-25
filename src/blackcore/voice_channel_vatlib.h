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

#include "voice_channel.h"
#include "voice_vatlib.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/voiceroomlist.h"
#include "blackmisc/avcallsignlist.h"
#include <QObject>
#include <QSharedPointer>

//! \file

namespace BlackCore
{
    class CVoiceChannelVatlibPrivate;

    //! Class implementing the voice channel interface
    class CVoiceChannelVatlib : public IVoiceChannel
    {
        Q_OBJECT

    public:

        //! Default constructor
        CVoiceChannelVatlib(TVatlibPointer vatlib, QObject *parent = nullptr);

        //! Destructor
        virtual ~CVoiceChannelVatlib();

        //! \copydoc IVoiceChannel::joinVoiceRoom
        virtual void joinVoiceRoom(const BlackMisc::Audio::CVoiceRoom &voiceRoom) override;

        //! \copydoc IVoiceChannel::leaveVoiceRoom
        virtual void leaveVoiceRoom() override;

        //! \copydoc IVoiceChannel::startTransmitting
        virtual void startTransmitting() override;

        //! \copydoc IVoiceChannel::stopTransmitting
        virtual void stopTransmitting() override;

        //! \copydoc IVoiceChannel::getVoiceRoomCallsigns
        virtual BlackMisc::Aviation::CCallsignList getVoiceRoomCallsigns() const override;

        //! \copydoc IVoiceChannel::switchAudioOutput
        virtual void switchAudioOutput(bool enable) override;

        //! \copydoc IVoiceChannel::setMyAircraftCallsign
        virtual void setMyAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IVoiceChannel::getVoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom() const override;

        //! \copydoc IVoiceChannel::getRoomIndex
        virtual qint32 getRoomIndex() const override;

        //! \copydoc IVoiceChannel::isMuted
        virtual bool isMuted() const override;

        //! Set channel volume
        virtual void setVolume(quint32 volume) override;

        //! Get channel volume
        virtual quint32 getVolume() const override;

        //! \copydoc IVoiceChannel::updateRoomStatus
        virtual void updateRoomStatus(Cvatlib_Voice_Simple::roomStatusUpdate roomStatus) override;

    private:
        QSharedPointer<CVoiceChannelVatlibPrivate> d_ptr;
    };
}

#endif // guard
