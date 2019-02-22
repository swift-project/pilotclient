/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VOICE_CHANNEL_H
#define BLACKCORE_VOICE_CHANNEL_H

#include "blackcoreexport.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/callsignset.h"

#include <QObject>
#include <QScopedPointer>

namespace BlackCore
{
    //! Interface to a voice channel
    class BLACKCORE_EXPORT IVoiceChannel : public QObject
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
            ConnectingFailed    //!< Failed to connect
        };
        Q_ENUM(ConnectionStatus)

        //! Constructor
        IVoiceChannel(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~IVoiceChannel() {}

        //! Join voice room
        virtual void joinVoiceRoom(const BlackMisc::Audio::CVoiceRoom &voiceRoom) = 0;

        //! Leave voice room
        virtual void leaveVoiceRoom() = 0;

        //! Get voice room callsings
        virtual BlackMisc::Aviation::CCallsignSet getVoiceRoomCallsigns() const = 0;

        //! Set own aircraft's callsign
        virtual void setOwnAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Set user id
        virtual void setUserId(const QString &id) = 0;

        //! Get voice room
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom() const = 0;

        //! Is channel muted?
        virtual bool isMuted() const = 0;

        //! Set channel volume 0..100
        virtual void setVolume(int volume) = 0;

        //! Get channel volume 0..100
        virtual int getVolume() const = 0;

    signals:

        //! We sent a message about the status of the network connection, for the attention of the user.
        void statusMessage(const BlackMisc::CStatusMessage &message);

        //! The status of a room has changed.
        void connectionStatusChanged(BlackCore::IVoiceChannel::ConnectionStatus oldStatus,
                                     BlackCore::IVoiceChannel::ConnectionStatus newStatus);

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

    };
} // ns

Q_DECLARE_METATYPE(BlackCore::IVoiceChannel::ConnectionStatus)

#endif // guard
