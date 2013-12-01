/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "../blackmisc/context.h"
#include "../blackmisc/avcallsign.h"
#include "../blackmisc/vvoiceroom.h"
#include "../blackmisc/vaudiodevice.h"

#include <vatlib/vatlib.h>

#include <QObject>
#include <QStringList>

namespace BlackCore
{
    /*!
     * Interface to a connection to a ATC voice server for use in flight simulation.
     *
     * \warning If an INetwork signal is connected to a slot, and that slot emits a signal
     *          which is connected to an INetwork slot, then at least one of those connections
     *          must be a Qt::QueuedConnection.
     *          Reason: IVoiceClient implementations are not re-entrant.
     */
    class IVoiceClient : public QObject
    {

        /* TODOS:
         * - Find a replacement for comUnit. Maybe map it to the ComUnit in the aircraft as a class
         * - Settings: Settings classes to store hardware settings (squelch, background noise, hardware device)
         */

        Q_OBJECT

    protected:

        /*!
         * \brief Default constructor with parent
         * \param parent
         */
        IVoiceClient(QObject *parent = 0) : QObject(parent) {}

    public:
        BLACK_INTERFACE(BlackCore::IVoice)

        virtual ~IVoiceClient() {}

        // The following method should be called everytime you receive a user update signal
        virtual const QStringList roomUserList(const int32_t comUnit) = 0;

        // Hardware devices
        virtual const QList<BlackMisc::Voice::CInputAudioDevice> & audioInputDevices() const = 0;
        virtual const QList<BlackMisc::Voice::COutputAudioDevice> & audioOutputDevices() const = 0;

        virtual const BlackMisc::Voice::CInputAudioDevice & defaultAudioInputDevice() const = 0;
        virtual const BlackMisc::Voice::COutputAudioDevice & defaultAudioOutputDevice() const = 0;

        virtual void setInputDevice(const BlackMisc::Voice::CInputAudioDevice &device) = 0;
        virtual void setOutputDevice(const BlackMisc::Voice::COutputAudioDevice &device) = 0;

        virtual void enableAudio(const int32_t comUnit) = 0;

        // Mic tests

        virtual void runSquelchTest() = 0;
        virtual void runMicTest() = 0;

        virtual float inputSquelch() const = 0;
        virtual Cvatlib_Voice_Simple::agc micTestResult() const = 0;

        virtual const BlackMisc::Voice::CVoiceRoom &voiceRoom (const uint32_t comUnit) = 0;

    public slots:
        virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void joinVoiceRoom(const int32_t comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom) = 0;
        virtual void leaveVoiceRoom(const int32_t comUnit) = 0;
        virtual void setVolume(const int32_t comUnit, const uint32_t volumne) = 0;
        virtual void startTransmitting(const int32_t comUnit) = 0;
        virtual void stopTransmitting(const int32_t comUnit) = 0;
        virtual bool isReceiving(const int32_t comUnit) = 0;
        virtual bool isConnected(const int32_t comUnit) = 0;


    signals:
        // Signals regarding the voice server connection
        void notConnected(const int32_t comUnit);
        void connecting(const int32_t comUnit);
        void connected(const int32_t comUnit);
        void connectionFailed(const int32_t comUnit);
        void kicked(const int32_t comUnit);
        void disconnecting(const int32_t comUnit);
        void disconnected(const int32_t comUnit);

        // Signals about users joining and leaving
        void userJoinedRoom(const QString &callsign);
        void userLeftRoom(const QString &callsign);

        // Audio signals
        void audioStarted(const int32_t comUnit);
        void audioStopped(const int32_t comUnit);
        void audioStarted();
        void audioStopped();

        // Test signals
        void squelchTestFinished();
        void micTestFinished();

        // non protocol related signals
        void exception(const QString &message, bool fatal = false); // let remote places know there was an exception

    public slots:

    };

} // namespace BlackCore

#endif // BLACKCORE_VOICE_H
