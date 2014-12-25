/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "audio_device.h"
#include "voice_channel.h"

#include "../blackmisc/avcallsignlist.h"
#include "../blackmisc/avselcal.h"
#include "../blackmisc/nwuserlist.h"
#include "../blackmisc/voiceroomlist.h"
#include "../blackmisc/audiodeviceinfolist.h"
#include "../blackmisc/statusmessage.h"

#include <QObject>

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
        virtual std::unique_ptr<IVoiceChannel> createVoiceChannel() = 0;

        //! Create input device object
        virtual std::unique_ptr<IAudioInputDevice> createInputDevice() = 0;

        //! Create output device object
        virtual std::unique_ptr<IAudioOutputDevice> createOutputDevice() = 0;

        //! Connect voice channel to an audio output device
        virtual void connectChannelOutputDevice(IVoiceChannel *channel, IAudioOutputDevice *device) = 0;

        /*!
         * Connect input device to voice channel to start/stop voice transmission
         * \param device Audio input device, e.g. a microphone. Pass nullptr to stop transmission
         * \param channel Voice channel audio should be transmitted to
         */
        virtual void connectChannelInputDevice(IAudioInputDevice *device, IVoiceChannel *channel) = 0;

        //! Connect audio input to an audio output to hear your own voice
        virtual void enableAudioLoopback(IAudioInputDevice *input, IAudioOutputDevice *output) = 0;
    };

} // namespace BlackCore

#endif // guard
