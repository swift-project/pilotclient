/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voice_vatlib.h"
#include "voice_channel_vatlib.h"
#include "audio_device_vatlib.h"
#include "audio_mixer_vatlib.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QDebug>
#include <QTimer>
#include <memory>
#include <mutex>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    /*
     * Constructor
     */
    CVoiceVatlib::CVoiceVatlib(QObject *parent) :
        IVoice(parent),
        m_audioService(Vat_CreateAudioService()),
        m_udpPort(Vat_CreateUDPAudioPort(m_audioService.data(), 3782))
    {
        Vat_SetVoiceErrorHandler(CVoiceVatlib::voiceErrorHandler);

        // do processing
        this->startTimer(10);
    }

    /*
     * Destructor
     */
    CVoiceVatlib::~CVoiceVatlib() {}

    std::unique_ptr<IVoiceChannel> CVoiceVatlib::createVoiceChannel()
    {
        return make_unique<CVoiceChannelVatlib>(m_audioService.data(), m_udpPort.data(), this);
    }

    std::unique_ptr<IAudioInputDevice> CVoiceVatlib::createInputDevice()
    {
        return make_unique<CAudioInputDeviceVatlib>(m_audioService.data(), this);
    }

    std::unique_ptr<IAudioOutputDevice> CVoiceVatlib::createOutputDevice()
    {
        return make_unique<CAudioOutputDeviceVatlib>(m_audioService.data(), this);
    }

    std::unique_ptr<IAudioMixer> CVoiceVatlib::createAudioMixer()
    {
        return make_unique<CAudioMixerVatlib>(this);
    }

    /* FIXME:
       Can the following methods be more general somehow?
       E.g.:
       template <typename Input, typename Output>
       connectVoice(Input input, Output output)
       {
           ...
       }
     */

    void CVoiceVatlib::connectChannelOutputDevice(IVoiceChannel *channel, IAudioOutputDevice *device)
    {
        auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib*>(channel);
        Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectChannelOutputDevice", "No valid CVoiceChannelVatlib pointer.");

        if (!device)
        {
            Vat_VoiceConnect(voiceChannelVatlib->getVoiceChannel(), 0, nullptr, 0);
            return;
        }

        auto audioDeviceVatlib = qobject_cast<CAudioOutputDeviceVatlib*>(device);
        Q_ASSERT_X(audioDeviceVatlib, "CVoiceVatlib::connectOutputDevice", "No valid CAudioOutputDeviceVatlib pointer.");

        Vat_VoiceConnect(voiceChannelVatlib->getVoiceChannel(), 0, audioDeviceVatlib->getVatLocalOutputCodec(), 0);
    }

    void CVoiceVatlib::connectChannelInputDevice(IAudioInputDevice *device, IVoiceChannel *channel)
    {
        auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib*>(channel);
        Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectChannelInputDevice", "No valid CVoiceChannelVatlib pointer.");

        if (!device)
        {
            Vat_VoiceConnect(voiceChannelVatlib->getVoiceChannel(), 0, nullptr, 0);
            return;
        }

        auto audioDeviceVatlib = qobject_cast<CAudioInputDeviceVatlib*>(device);
        Q_ASSERT_X(audioDeviceVatlib, "CVoiceVatlib::connectChannelInputDevice", "No valid CAudioInputDeviceVatlib pointer.");

        Vat_VoiceConnect(audioDeviceVatlib->getVatLocalInputCodec(), 0, voiceChannelVatlib->getVoiceChannel(), 0);
    }

    void CVoiceVatlib::enableAudioLoopback(IAudioInputDevice *input, IAudioOutputDevice *output)
    {
        auto vatlibInput = qobject_cast<CAudioInputDeviceVatlib*>(input);
        Q_ASSERT_X(vatlibInput, "CVoiceVatlib::enableAudioLoopback", "No valid CAudioInputDeviceVatlib pointer.");

        if (!output)
        {
            Vat_VoiceConnect(vatlibInput->getVatLocalInputCodec(), 0, nullptr, 0);
            return;
        }

        auto vatlibOutput = qobject_cast<CAudioOutputDeviceVatlib*>(output);
        Q_ASSERT_X(vatlibOutput, "CVoiceVatlib::enableAudioLoopback", "No valid CAudioOutputDeviceVatlib pointer.");

        Vat_VoiceConnect(vatlibInput->getVatLocalInputCodec(), 0, vatlibOutput->getVatLocalOutputCodec(), 0);
    }

    /*
     * Process voice handling
     */
    void CVoiceVatlib::timerEvent(QTimerEvent *)
    {
        Q_ASSERT_X(m_audioService, "CVoiceVatlib", "VatAudioService invalid!");
        Vat_ExecuteTasks(m_audioService.data());
    }

    void CVoiceVatlib::voiceErrorHandler(const char *message)
    {
        CLogMessage(static_cast<CVoiceVatlib*>(nullptr)).error(message);
    }

} // namespace
