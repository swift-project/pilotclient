/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voice_vatlib.h"
#include "voice_channel_vatlib.h"
#include "audio_device_vatlib.h"
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
