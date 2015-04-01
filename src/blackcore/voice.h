/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "audio_device.h"
#include "voice_channel.h"
#include "audio_mixer.h"

#include "blackmisc/aviation/callsignlist.h"
#include "blackmisc/aviation/selcal.h"
#include "../blackmisc/nwuserlist.h"
#include "../blackmisc/voiceroomlist.h"
#include "../blackmisc/audiodeviceinfolist.h"
#include "../blackmisc/statusmessage.h"

#include <QObject>
#include <QSharedPointer>

#include <memory>

namespace BlackCore
{
    //! Interface to a connection to a ATC voice server for use in flight simulation.
    class IVoice : public QObject
    {
        Q_OBJECT

    public:

        /*!
         * \brief Default constructor with parent
         * \param parent
         */
        IVoice(QObject *parent = nullptr);

        //! Virtual destructor.
        virtual ~IVoice() {}

        //! Create voice channel object
        virtual QSharedPointer<IVoiceChannel> createVoiceChannel() = 0;

        //! Create input device object
        virtual std::unique_ptr<IAudioInputDevice> createInputDevice() = 0;

        //! Create output device object
        virtual std::unique_ptr<IAudioOutputDevice> createOutputDevice() = 0;

        //! Create audio mixer object
        virtual std::unique_ptr<IAudioMixer> createAudioMixer() = 0;

        //! Connect audio input device to audio mixer
        virtual void connectVoice(IAudioInputDevice *device, IAudioMixer *mixer, IAudioMixer::InputPort inputPort) = 0;

        //! Connect voice channel to audio mixer
        virtual void connectVoice(IVoiceChannel *channel, IAudioMixer *mixer, IAudioMixer::InputPort inputPort) = 0;

        //! Connect audio mixer to audio output device
        virtual void connectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort, IAudioOutputDevice *device) = 0;

        //! Connect audio mixer to audio input device
        virtual void connectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort, IVoiceChannel *channel) = 0;

        //! Disconnect input device
        virtual void disconnectVoice(IAudioInputDevice *device) = 0;

        //! Disconnect voice channel
        virtual void disconnectVoice(IVoiceChannel *channel) = 0;

        //! Disconnect audio mixer output port
        virtual void disconnectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort) = 0;
    };

} // namespace BlackCore

#endif // guard
