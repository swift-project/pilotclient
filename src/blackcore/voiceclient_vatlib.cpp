/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voiceclient_vatlib.h"

namespace BlackCore
{

    CVoiceClientVatlib::CVoiceClientVatlib(QObject *parent) :
        IVoiceClient(parent),
        m_voice(Create_Cvatlib_Voice_Simple())
    {
    }

    void CVoiceClientVatlib::setCallsign(const BlackMisc::Aviation::CCallsign &callsign)
    {
        m_callsign = callsign;
    }

    void CVoiceClientVatlib::joinVoiceRoom(const uint32_t comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom)
    {

    }

    void CVoiceClientVatlib::leaveVoiceRoom(const uint32_t comUnit)
    {

    }

    void CVoiceClientVatlib::setVolume(const uint32_t comIndex, const uint32_t volumne)
    {

    }

    void CVoiceClientVatlib::startTransmitting(const uint32_t comIndex)
    {

    }

    void CVoiceClientVatlib::stopTransmitting(const uint32_t comIndex)
    {

    }

    bool CVoiceClientVatlib::isReceiving(const uint32_t comIndex)
    {

    }

    bool CVoiceClientVatlib::isConnected(const uint32_t comIndex)
    {

    }

    void CVoiceClientVatlib::roomUserList(const uint32_t comIndex)
    {

    }

    void CVoiceClientVatlib::audioInputDevices(const uint32_t comIndex)
    {

    }

    void CVoiceClientVatlib::audioOutputDevices(const uint32_t comIndex)
    {

    }

    void CVoiceClientVatlib::setInputDevice(const uint32_t comUnit, BlackMisc::Voice::CInputAudioDevice &device)
    {

    }

    void CVoiceClientVatlib::setOutputDevice(const uint32_t comUnit, BlackMisc::Voice::COutputAudioDevice &device)
    {

    }

    const BlackMisc::Voice::CVoiceRoom &CVoiceClientVatlib::voiceRoom(const uint32_t comUnit)
    {

    }

    void CVoiceClientVatlib::onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, int32_t roomIndex, void *cbVar)
    {

    }

    void CVoiceClientVatlib::onRoomUserReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {

    }

    void CVoiceClientVatlib::onHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {

    }

} // namespace BlackCore
