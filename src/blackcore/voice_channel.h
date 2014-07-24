/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_VOICE_CHANNEL_H
#define BLACKCORE_VOICE_CHANNEL_H

#include "voice_vatlib.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/voiceroomlist.h"
#include "blackmisc/avcallsignlist.h"
#include <QObject>
#include <QScopedPointer>

//! \file

namespace BlackCore
{
    //! Interface to a voice channel
    class IVoiceChannel : public QObject
    {
        Q_OBJECT

    public:

        //! Com status
        enum ConnectionStatus
        {
            Disconnected = 0,   //!< Not connected
            Disconnecting,      //!< In transition to disconnected
            DisconnectedError,  //!< Disconnected due to socket error
            Connecting,         //!< Connection initiated but not established
            Connected,          //!< Connection established
            ConnectingFailed,   //!< Failed to connect
        };

        //! Join voice room
        virtual void joinVoiceRoom(const BlackMisc::Audio::CVoiceRoom &voiceRoom) = 0;

        //! Leave voice room
        virtual void leaveVoiceRoom() = 0;

        //! Set room output volume
        virtual void setRoomOutputVolume(const qint32 volume) = 0;

        //! Start transmitting
        virtual void startTransmitting() = 0;

        //! Stop transmitting
        virtual void stopTransmitting() = 0;

        //! Get voice room callsings
        virtual BlackMisc::Aviation::CCallsignList getVoiceRoomCallsigns() const = 0;

        //! Switch audio output, enable or disable
        virtual void switchAudioOutput(bool enable) = 0;

        //! Set own aircraft's callsign
        virtual void setMyAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Get voice room
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom() const = 0;

        //! Get assigned room index
        virtual qint32 getRoomIndex() const = 0;

        //! Is channel muted?
        virtual bool isMuted() const = 0;

        //! Set channel volume
        virtual void setVolume(quint32 volume) = 0;

        //! Get channel volume
        virtual quint32 getVolume() const = 0;

        //! Update room status
        virtual void updateRoomStatus(Cvatlib_Voice_Simple::roomStatusUpdate roomStatus) = 0;

    signals:

        //! We sent a message about the status of the network connection, for the attention of the user.
        void statusMessage(const BlackMisc::CStatusMessage &message);

        //! The status of a room has changed.
        void connectionStatusChanged(ConnectionStatus oldStatus, ConnectionStatus newStatus);

        // Signals about users joining and leaving

        //! User with callsign joined room
        void userJoinedRoom(const BlackMisc::Aviation::CCallsign &callsign);

        //! User with callsign left room
        void userLeftRoom(const BlackMisc::Aviation::CCallsign &callsign);

        // Audio signals

        //! Audio for given unit started
        void audioStarted();

        //! Audio for given unit stopped
        void audioStopped();

    protected:

        //! Constructor
        IVoiceChannel(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~IVoiceChannel() {}

    };
}

Q_DECLARE_METATYPE(BlackCore::IVoiceChannel::ConnectionStatus)

#endif // guard
