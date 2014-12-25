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

    std::unique_ptr<IAudioInputDevice> CVoiceVatlib::createInputDevice()
    {
        return make_unique<CAudioInputDeviceVatlib>(m_audioService.data(), this);
    }

    std::unique_ptr<IAudioOutputDevice> CVoiceVatlib::createOutputDevice()
    {
        return make_unique<CAudioOutputDeviceVatlib>(m_audioService.data(), this);
    }

    IVoiceChannel *CVoiceVatlib::getVoiceChannel(qint32 channelIndex) const
    {
        IVoiceChannel *channel = m_hashChannelIndex.value(channelIndex, nullptr);
        Q_ASSERT(channel);

        return channel;
    }

    /*
     * Handle PTT
     */
    void CVoiceVatlib::handlePushToTalk(bool value)
    {
        qDebug() << "PTT";
        if (!this->m_vatlib) return;

        if (value) qDebug() << "Start transmitting...";
        else qDebug() << "Stop transmitting...";

        // FIXME: Set only once channel to active for transmitting
        if (value)
        {
            getVoiceChannel(0)->startTransmitting();
            getVoiceChannel(1)->startTransmitting();
        }
        else
        {
            getVoiceChannel(0)->stopTransmitting();
            getVoiceChannel(1)->stopTransmitting();
        }
    }

    /*
     * Process voice handling
     */
    void CVoiceVatlib::timerEvent(QTimerEvent *)
    {
        Q_ASSERT_X(m_audioService, "CVoiceVatlib", "VatAudioService invalid!");
        Vat_ExecuteTasks(m_audioService.data());
    }

    /*
     * Room status update
     */
    void CVoiceVatlib::onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, qint32 roomIndex, void *cbVar)
    {
        Q_UNUSED(obj)
        CVoiceVatlib *vatlibRoom = cbvar_cast_voice(cbVar);
        vatlibRoom->onRoomStatusUpdate(roomIndex, upd);
    }

    void CVoiceVatlib::onRoomStatusUpdate(qint32 roomIndex, Cvatlib_Voice_Simple::roomStatusUpdate roomStatus)
    {
        QList<IVoiceChannel *> voiceChannels = m_hashChannelIndex.values();
        auto iterator = std::find_if(voiceChannels.begin(), voiceChannels.end(), [&](const IVoiceChannel * voiceChannel)
        {
            return voiceChannel->getRoomIndex() == roomIndex;
        });

        if (iterator == voiceChannels.end())
        {
            qWarning() << "Unknown room index";
            return;
        }

        (*iterator)->updateRoomStatus(roomStatus);
    }

    void CVoiceVatlib::voiceErrorHandler(const char *message)
    {
        CLogMessage(static_cast<CVoiceVatlib*>(nullptr)).error(message);
    }

} // namespace
