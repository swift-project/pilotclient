/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VOICECLIENT_VATLIB_H
#define VOICECLIENT_VATLIB_H

#include "voiceclient.h"

#include <QScopedPointer>
#include <QMap>

namespace BlackCore
{

    class CVoiceClientVatlib : public IVoiceClient
    {
        Q_OBJECT

    public:
        CVoiceClientVatlib(QObject *parent = 0);
        virtual ~CVoiceClientVatlib();

        virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void joinVoiceRoom(const uint32_t comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom);
        virtual void leaveVoiceRoom(const uint32_t comUnit);
        virtual void setVolume(const uint32_t comUnit, const uint32_t volumne);
        virtual void startTransmitting(const uint32_t comUnit);
        virtual void stopTransmitting(const uint32_t comUnit);
        virtual bool isReceiving(const uint32_t comUnit);
        virtual bool isConnected(const uint32_t comUnit);

        virtual void roomUserList(const uint32_t comUnit);
        virtual const QList<BlackMisc::Voice::CInputAudioDevice> &audioInputDevices(const uint32_t comUnit) const ;
        virtual const QList<BlackMisc::Voice::COutputAudioDevice> & audioOutputDevices(const uint32_t comUnit) const;

        virtual void setInputDevice(const uint32_t comUnit, BlackMisc::Voice::CInputAudioDevice &device);
        virtual void setOutputDevice(const uint32_t comUnit, BlackMisc::Voice::COutputAudioDevice &device);

        virtual const BlackMisc::Voice::CVoiceRoom &voiceRoom (const uint32_t comUnit);

    signals:

    public slots:

    private:

        // shimlib callbacks
        static void onRoomStatusUpdate(Cvatlib_Voice_Simple* obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, INT roomIndex, void* cbVar);
        static void onRoomUserReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);
        static void onInputHardwareDeviceReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);
        static void onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);

        QScopedPointer<Cvatlib_Voice_Simple> m_voice;
        BlackMisc::Aviation::CCallsign m_callsign;
        QMap<uint32_t, BlackMisc::Voice::CVoiceRoom> m_voiceRoomMap;
        QList<BlackMisc::Voice::CInputAudioDevice> m_inputDevices;
        QList<BlackMisc::Voice::COutputAudioDevice> m_outputDevices;
    };

} // namespace BlackCore

#endif // VOICECLIENT_VATLIB_H
