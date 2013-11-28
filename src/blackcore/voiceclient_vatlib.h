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
        virtual void joinVoiceServer(const uint32_t comUnit, const QString &serverSpec);
        virtual void leaveVoiceServer(const uint32_t comUnit) = 0;
        virtual void setVolume(const uint32_t comUnit, const uint32_t volumne);
        virtual void startTransmitting(const uint32_t comUnit);
        virtual void stopTransmitting(const uint32_t comUnit);
        virtual bool isReceiving(const uint32_t comUnit);
        virtual bool isConnected(const uint32_t comUnit);

        virtual void getUserList(const uint32_t comUnit);
        virtual void getInputDevices(const uint32_t comUnit);
        virtual void getOutputDevices(const uint32_t comUnit);

    signals:

    public slots:

    private:

        // shimlib callbacks
        void onRoomStatusUpdate(Cvatlib_Voice_Simple* obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, INT roomIndex, void* cbVar);
        void onRoomUserReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);
        void onHardwareDeviceReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);

        QScopedPointer<Cvatlib_Voice_Simple> m_voice;
        BlackMisc::Aviation::CCallsign m_callsign;
        QMap<uint32_t, BlackMisc::Voice::CVoiceRoom> m_voiceRoomMap;

    };

} // namespace BlackCore

#endif // VOICECLIENT_VATLIB_H
