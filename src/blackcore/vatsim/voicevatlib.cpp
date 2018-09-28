/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/audiodevice.h"
#include "blackcore/vatsim/audiodevicevatlib.h"
#include "blackcore/vatsim/audiomixervatlib.h"
#include "blackcore/voicechannel.h"
#include "blackcore/vatsim/voicechannelvatlib.h"
#include "blackcore/vatsim/voicevatlib.h"
#include "blackmisc/logmessage.h"
#include "vatlib/vatlib.h"

#include <QtGlobal>
#include <memory>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    namespace Vatsim
    {
        const CLogCategoryList &CVoiceVatlib::getLogCategories()
        {
            static const CLogCategoryList cats({ CLogCategory::vatsimSpecific(), CLogCategory::network() });
            return cats;
        }

        CVoiceVatlib::CVoiceVatlib(QObject *parent) :
            IVoice(parent),
            m_audioService(Vat_CreateAudioService())
        {
            const int udpPort = m_vatsimVoiceSettings.get().getVatsimUdpVoicePort();
            Vat_SetVoiceLogHandler(SeverityLevel::SeverityError, CVoiceVatlib::voiceLogHandler);
            m_udpPort.reset(Vat_CreateUDPAudioPort(m_audioService.data(), udpPort));

            // do processing
            this->startTimer(10);
        }

        CVoiceVatlib::~CVoiceVatlib() {}

        void CVoiceVatlib::setVoiceSetup(const CVoiceSetup &setup)
        {
            if (m_vatsimVoiceSettings.get() == setup) { return; }
            m_vatsimVoiceSettings.setAndSave(setup);

            // CHANGE VOICE PORT WOULD NEED TO GO HERE
        }

        CVoiceSetup CVoiceVatlib::getVoiceSetup() const
        {
            return m_vatsimVoiceSettings.get();
        }

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
            auto audioInputVatlib = qobject_cast<CAudioInputDeviceVatlib *>(device);
            Q_ASSERT_X(audioInputVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioInputDeviceVatlib pointer.");

            auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib *>(mixer);
            Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

            Vat_VoiceConnect(VatProducerLocalInput, audioInputVatlib->getVatLocalInputCodec(), 0,
                             VatConsumerAudioMixer, audioMixerVatlib->getVatAudioMixer(), inputPort);
        }

        void CVoiceVatlib::connectVoice(IVoiceChannel *channel, IAudioMixer *mixer, IAudioMixer::InputPort inputPort)
        {
            auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib *>(channel);
            Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectVoice", "No valid CVoiceChannelVatlib pointer.");

            auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib *>(mixer);
            Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

            Vat_VoiceConnect(VatProducerVoiceChannel, voiceChannelVatlib->getVoiceChannel(), 0,
                             VatConsumerAudioMixer, audioMixerVatlib->getVatAudioMixer(), inputPort);
        }

        void CVoiceVatlib::connectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort, IAudioOutputDevice *device)
        {
            auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib *>(mixer);
            Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

            auto audioDeviceVatlib = qobject_cast<CAudioOutputDeviceVatlib *>(device);
            Q_ASSERT_X(audioDeviceVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioOutputDeviceVatlib pointer.");

            Vat_VoiceConnect(VatProducerAudioMixer, audioMixerVatlib->getVatAudioMixer(), outputPort,
                             VatConsumerLocalOutput, audioDeviceVatlib->getVatLocalOutputCodec(), 0);
        }

        void CVoiceVatlib::connectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort, IVoiceChannel *channel)
        {
            auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib *>(mixer);
            Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");

            auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib *>(channel);
            Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectVoice", "No valid CVoiceChannelVatlib pointer.");

            Vat_VoiceConnect(VatProducerAudioMixer, audioMixerVatlib->getVatAudioMixer(), outputPort,
                             VatConsumerVoiceChannel, voiceChannelVatlib->getVoiceChannel(), 0);
        }

        void CVoiceVatlib::disconnectVoice(IAudioInputDevice *device)
        {
            auto audioInputVatlib = qobject_cast<CAudioInputDeviceVatlib *>(device);
            Q_ASSERT_X(audioInputVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioInputDeviceVatlib pointer.");
            Vat_VoiceConnect(VatProducerLocalInput, audioInputVatlib->getVatLocalInputCodec(), 0, VatConsumerNone, nullptr, 0);
        }

        void CVoiceVatlib::disconnectVoice(IVoiceChannel *channel)
        {
            auto voiceChannelVatlib = qobject_cast<CVoiceChannelVatlib *>(channel);
            Q_ASSERT_X(voiceChannelVatlib, "CVoiceVatlib::connectVoice", "No valid CVoiceChannelVatlib pointer.");
            Vat_VoiceConnect(VatProducerVoiceChannel, voiceChannelVatlib->getVoiceChannel(), 0, VatConsumerNone, nullptr, 0);
        }

        void CVoiceVatlib::disconnectVoice(IAudioMixer *mixer, IAudioMixer::OutputPort outputPort)
        {
            auto audioMixerVatlib = qobject_cast<CAudioMixerVatlib *>(mixer);
            Q_ASSERT_X(audioMixerVatlib, "CVoiceVatlib::connectVoice", "No valid CAudioMixerVatlib pointer.");
            Vat_VoiceConnect(VatProducerAudioMixer, audioMixerVatlib->getVatAudioMixer(), outputPort, VatConsumerNone, nullptr, 0);
        }

        void CVoiceVatlib::timerEvent(QTimerEvent *)
        {
            Q_ASSERT_X(m_audioService, "CVoiceVatlib", "VatAudioService invalid!");
            Vat_ExecuteVoiceTasks(m_audioService.data());
        }

        void CVoiceVatlib::voiceLogHandler(SeverityLevel /** severity **/, const char *context, const char *message)
        {
            QString errorMessage("vatlib ");
            errorMessage += context;
            errorMessage += ": ";
            errorMessage += message;
            CLogMessage(static_cast<CVoiceVatlib *>(nullptr)).error(errorMessage);
        }
    } // namespace
} // namespace
