/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/audiodevice.h"
#include "blackcore/audiodevicevatlib.h"
#include "blackcore/audiomixervatlib.h"
#include "blackcore/voicechannel.h"
#include "blackcore/voicechannelvatlib.h"
#include "blackcore/voicevatlib.h"
#include "blackmisc/logmessage.h"
#include "vatlib/vatlib.h"

#include <QtGlobal>
#include <memory>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    CVoiceVatlib::CVoiceVatlib(QObject *parent) :
        IVoice(parent),
        m_audioService(Vat_CreateAudioService()),
        m_udpPort(Vat_CreateUDPAudioPort(m_audioService.data(), 0))
    {
        Vat_SetVoiceLogHandler(SeverityLevel::SeverityError, CVoiceVatlib::voiceLogHandler);

        // do processing
        this->startTimer(10);
    }

    CVoiceVatlib::~CVoiceVatlib() {}

    QSharedPointer<IVoiceChannel> CVoiceVatlib::createVoiceChannel()
    {
        return QSharedPointer<IVoiceChannel>(new CVoiceChannelVatlib(m_audioService.data(), m_udpPort.data(), this));
    }

    std::unique_ptr<IAudioInputDevice> CVoiceVatlib::createInputDevice()
    {
        return std::make_unique<CAudioInputDeviceVatlib>(m_audioService.data(), this);
    }

    std::unique_ptr<IAudioOutputDevice> CVoiceVatlib::createOutputDevice()
    {
        return std::make_unique<CAudioOutputDeviceVatlib>(m_audioService.data(), this);
    }

    std::unique_ptr<IAudioMixer> CVoiceVatlib::createAudioMixer()
    {
        return std::make_unique<CAudioMixerVatlib>(this);
    }

    void CVoiceVatlib::connectVoice(IAudioInputDevice *device, IAudioMixer *mixer, IAudioMixer::InputPort inputPort)
    {
        auto audioInputVatlib = qobject_cast<CAudioInputDeviceVatlib*>(device);
        Q_ASSERT_X(audioInputVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioInputDeviceVatlib pointer.");

        auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib*>(mixer);
        Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

        Vat_ConnectProducerToProducerConsumer(audioInputVatlib->getVatLocalInputCodec(), 0, audioMixerVatlib->getVatAudioMixer(), inputPort);
    }

    void CVoiceVatlib::connectVoice(IVoiceChannel *channel, IAudioMixer *mixer, IAudioMixer::InputPort inputPort)
    {
        auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib*>(channel);
        Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectVoice", "No valid CVoiceChannelVatlib pointer.");

        auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib*>(mixer);
        Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

        Vat_ConnectProducerConsumerToProducerConsumer(voiceChannelVatlib->getVoiceChannel(), 0, audioMixerVatlib->getVatAudioMixer(), inputPort);
    }

    void CVoiceVatlib::connectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort, IAudioOutputDevice *device)
    {
        auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib*>(mixer);
        Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

        auto audioDeviceVatlib = qobject_cast<CAudioOutputDeviceVatlib*>(device);
        Q_ASSERT_X(audioDeviceVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioOutputDeviceVatlib pointer.");

        Vat_ConnectProducerConsumerToConsumer(audioMixerVatlib->getVatAudioMixer(), outputPort, audioDeviceVatlib->getVatLocalOutputCodec(), 0);
    }

    void CVoiceVatlib::connectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort, IVoiceChannel *channel)
    {
        auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib*>(mixer);
        Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

        auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib*>(channel);
        Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectVoice", "No valid CVoiceChannelVatlib pointer.");

        Vat_ConnectProducerConsumerToProducerConsumer(audioMixerVatlib->getVatAudioMixer(), outputPort, voiceChannelVatlib->getVoiceChannel(), 0);
    }

    void CVoiceVatlib::disconnectVoice(IAudioInputDevice *device)
    {
        auto audioInputVatlib = qobject_cast<CAudioInputDeviceVatlib*>(device);
        Q_ASSERT_X(audioInputVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioInputDeviceVatlib pointer.");
        Vat_ConnectProducerToConsumer(audioInputVatlib->getVatLocalInputCodec(), 0, nullptr, 0);
    }

    void CVoiceVatlib::disconnectVoice(IVoiceChannel *channel)
    {
        auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib*>(channel);
        Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectVoice", "No valid CVoiceChannelVatlib pointer.");
        Vat_ConnectProducerConsumerToConsumer(voiceChannelVatlib->getVoiceChannel(), 0, nullptr, 0);
    }

    void CVoiceVatlib::disconnectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort)
    {
        auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib*>(mixer);
        Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");
        Vat_ConnectProducerConsumerToConsumer(audioMixerVatlib->getVatAudioMixer(), outputPort, nullptr, 0);
    }

    /*
     * Process voice handling
     */
    void CVoiceVatlib::timerEvent(QTimerEvent *)
    {
        Q_ASSERT_X(m_audioService, "CVoiceVatlib", "VatAudioService invalid!");
        Vat_ExecuteTasks(m_audioService.data());
    }

    void CVoiceVatlib::voiceLogHandler(SeverityLevel /** severity **/, const char *message)
    {
        CLogMessage(static_cast<CVoiceVatlib*>(nullptr)).error(message);
    }

} // namespace
