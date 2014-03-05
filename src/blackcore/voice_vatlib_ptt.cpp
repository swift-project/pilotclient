/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voice_vatlib.h"
#include <QDebug>

using namespace BlackMisc::Audio;

namespace BlackCore
{
    /*
     * Handle PTT
     */
    void CVoiceVatlib::handlePushToTalk(bool value)
    {
        qDebug() << "PTT";
        if (!this->m_voice) return;
        CVoiceRoomList rooms = this->getComVoiceRoomsWithAudioStatus();
        CVoiceRoom room1 = rooms[0];
        CVoiceRoom room2 = rooms[1];

        if (value) qDebug() << "Start transmitting...";
        else qDebug() << "Stop transmitting...";

        if (room1.isConnected())
        {
            if (value)
            {
                this->startTransmitting(IVoice::COM1);
            }
            else
            {
                this->stopTransmitting(IVoice::COM1);
            }
        }
        if (room2.isConnected())
        {
            if (value)
                this->startTransmitting(IVoice::COM2);
            else
                this->stopTransmitting(IVoice::COM2);
        }
    }
} // namespace
