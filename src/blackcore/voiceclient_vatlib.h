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
    /*!
     * Vatlib implementation of the IVoiceClient interface.
     */
    class CVoiceClientVatlib : public IVoiceClient
    {
        Q_OBJECT

    public:
        CVoiceClientVatlib(QObject *parent = 0);
        virtual ~CVoiceClientVatlib();

        virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void joinVoiceRoom(const int32_t comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom);
        virtual void leaveVoiceRoom(const int32_t comUnit);
        virtual void setVolume(const int32_t comUnit, const uint32_t volumne);
        virtual void startTransmitting(const int32_t comUnit);
        virtual void stopTransmitting(const int32_t comUnit);
        virtual bool isReceiving(const int32_t comUnit);
        virtual bool isConnected(const int32_t comUnit);

        virtual const QStringList roomUserList(const int32_t comUnit);

        // Hardware devices
        // TODO: Vatlib supports multiple output devices. That basically means, you could connect
        // to different voice rooms and send their audio to different devices, e.g. ATIS to loudspeakers
        // and ATC to headspeakers. Is not important to implement that now, if ever.
        virtual const QList<BlackMisc::Voice::CInputAudioDevice> & audioInputDevices() const ;
        virtual const QList<BlackMisc::Voice::COutputAudioDevice> & audioOutputDevices() const;

        virtual const BlackMisc::Voice::CInputAudioDevice & defaultAudioInputDevice() const;
        virtual const BlackMisc::Voice::COutputAudioDevice & defaultAudioOutputDevice() const;

        virtual void setInputDevice(const BlackMisc::Voice::CInputAudioDevice &device);
        virtual void setOutputDevice(const BlackMisc::Voice::COutputAudioDevice &device);

        virtual void enableAudio(const int32_t comUnit);

        // Mic tests
        virtual void runSquelchTest();
        virtual void runMicTest();

        virtual float inputSquelch() const;
        virtual Cvatlib_Voice_Simple::agc micTestResult() const;


        virtual const BlackMisc::Voice::CVoiceRoom &voiceRoom (const uint32_t comUnit);

        int32_t queryUserRoomIndex() const {return m_queryUserRoomIndex;}

    signals:

    public slots:

    protected: // QObject overrides
        virtual void timerEvent(QTimerEvent *);

    private slots:
        // slots for Mic tests
        void onEndFindSquelch();
        void onEndMicTest();

        // slot to handle users
        void onUserJoinedLeft(const int32_t roomIndex);

    signals:
        void userJoinedLeft(const int32_t roomIndex);

    private:

        // shimlib callbacks
        static void onRoomStatusUpdate(Cvatlib_Voice_Simple* obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, INT roomIndex, void* cbVar);
        static void onRoomUserReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);
        static void onInputHardwareDeviceReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);
        static void onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple* obj, const char* name, void* cbVar);

        void addUserInRoom(const int32_t roomIndex, const QString &callsign);
        void removeUserFromRoom(const int32_t roomIndex, const QString &callsign);

        void exceptionDispatcher(const char *caller);

        struct Cvatlib_Voice_Simple_Deleter
        {
            static inline void cleanup(Cvatlib_Voice_Simple *pointer)
            {
                pointer->Destroy();
            }
        };

        QScopedPointer<Cvatlib_Voice_Simple, Cvatlib_Voice_Simple_Deleter> m_voice;
        BlackMisc::Aviation::CCallsign m_callsign;
        QMap<uint32_t, BlackMisc::Voice::CVoiceRoom> m_voiceRooms;
        QList<BlackMisc::Voice::CInputAudioDevice> m_inputDevices;
        QList<BlackMisc::Voice::COutputAudioDevice> m_outputDevices;

        float m_inputSquelch;
        Cvatlib_Voice_Simple::agc m_micTestResult;

        typedef QMap<int32_t, Cvatlib_Voice_Simple::roomStatusUpdate> TMapRoomsStatus;
        TMapRoomsStatus m_voiceRoomsStatus;

        typedef QMap<int32_t, QStringList> TMapRoomsUsers;
        TMapRoomsUsers m_voiceRoomsUsers;

        // Need to keep the roomIndex because GetRoomUserList does not specifiy it
        int32_t m_queryUserRoomIndex;
    };

} // namespace BlackCore

#endif // VOICECLIENT_VATLIB_H
