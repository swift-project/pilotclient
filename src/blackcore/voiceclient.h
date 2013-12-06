/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "../blackmisc/context.h"
#include "../blackmisc/avcallsign.h"
#include "../blackmisc/vvoiceroom.h"
#include "../blackmisc/vaudiodevice.h"

#include <vatlib/vatlib.h>

#include <QObject>
#include <QStringList>
#include <QMetaType>

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
        IVoiceClient(QObject *parent = 0);

    public:

        //! ComUnit
        /*! IVoiceClient currently supports two different com units */
        enum ComUnit {
            COM1 = 0,   /*!< ComUnit 1 */
            COM2        /*!< ComUnit 2 */
        };

        //! Virtual destructor.
        virtual ~IVoiceClient() {}

        //! roomUserList
        /*!
          \return A list of users currently connected to the voice room
        */
        virtual const QSet<QString> roomUserList(const ComUnit comUnit) = 0;

        //! audioInputDevices
        /*!
          \return A list of available input devices
        */
        virtual const QList<BlackMisc::Voice::CInputAudioDevice> & audioInputDevices() const = 0;

        //! audioOutputDevices
        /*!
          \return A list of available output devices
        */
        virtual const QList<BlackMisc::Voice::COutputAudioDevice> & audioOutputDevices() const = 0;

        //! defaultAudioInputDevice
        /*!
          \return Default input device
        */
        virtual const BlackMisc::Voice::CInputAudioDevice defaultAudioInputDevice() const = 0;

        //! defaultAudioOutputDevice
        /*!
          \return Default output device
        */
        virtual const BlackMisc::Voice::COutputAudioDevice defaultAudioOutputDevice() const = 0;

        //! setInputDevice
        /*!
          \param input device
        */
        virtual void setInputDevice(const BlackMisc::Voice::CInputAudioDevice &device) = 0;

        //! setOutputDevice
        /*!
          \param output device
        */
        virtual void setOutputDevice(const BlackMisc::Voice::COutputAudioDevice &device) = 0;

        //! enableAudio
        /*!
          \brief After you have joined a voice room, you must enable audio output.
          \param comUnit
        */
        virtual void enableAudio(const ComUnit comUnit) = 0;

        //! voiceRoom
        /*!
          \brief After you have joined a voice room, you must enable audio output.
          \param comUnit
          \return voiceRoom
        */
        virtual const BlackMisc::Voice::CVoiceRoom voiceRoom (const ComUnit comUnit) = 0;

        //! isConnected
        /*!
          \param comUnit
          \return if connected
        */
        virtual bool isConnected(const ComUnit comUnit) = 0;

        //! isReceiving
        /*!
          \param comUnit
        */
        virtual bool isReceiving(const ComUnit comUnit) = 0;


        /************************************************
         * SETUP TESTS
         * *********************************************/

        //! runSquelchTest
        /*!
          \brief Runs a 5 seconds test, measuring your background noise.
        */
        virtual void runSquelchTest() = 0;

        //! runMicTest
        /*!
          \brief Runs a 5 seconds test, measuring the qualitiy of your mic input
        */
        virtual void runMicTest() = 0;

        //! inputSquelch
        /*!
          \return Value of the measured squelch
        */
        virtual float inputSquelch() const = 0;

        //! micTestResult
        /*!
          \return Result of the mic test.
        */
        virtual int32_t micTestResult() const = 0;

        //! micTestResult
        /*!
          \return Result of the mic test as human readable string
        */
        virtual QString micTestResultAsString() const = 0;

    public slots:

        //! setCallsign
        /*!
          \param callsign
        */
        virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! joinVoiceRoom
        /*!
          \param comUnit
          \param voiceRoom
        */
        virtual void joinVoiceRoom(const ComUnit comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom) = 0;

        //! leaveVoiceRoom
        /*!
          \param comUnit
        */
        virtual void leaveVoiceRoom(const ComUnit comUnit) = 0;

        //! setVolume
        /*!
          \param comUnit
          \param volumne
        */
        virtual void setVolume(const ComUnit comUnit, const int32_t volumne) = 0;

        //! startTransmitting
        /*!
          \param comUnit
        */
        virtual void startTransmitting(const ComUnit comUnit) = 0;

        //! stopTransmitting
        /*!
          \param comUnit
        */
        virtual void stopTransmitting(const ComUnit comUnit) = 0;

    signals:
        // Signals regarding the voice server connection
        void notConnected(const ComUnit comUnit);
        void connecting(const ComUnit comUnit);
        void connected(const ComUnit comUnit);
        void connectionFailed(const ComUnit comUnit);
        void kicked(const ComUnit comUnit);
        void disconnecting(const ComUnit comUnit);
        void disconnected(const ComUnit comUnit);

        // Signals about users joining and leaving
        void userJoinedRoom(const QString &callsign);
        void userLeftRoom(const QString &callsign);

        // Audio signals
        void audioStarted(const ComUnit comUnit);
        void audioStopped(const ComUnit comUnit);
        void globalAudioStarted();
        void globalAudioStopped();

        // Test signals
        void squelchTestFinished();
        void micTestFinished();

        // non protocol related signals
        void exception(const QString &message, bool fatal = false); // let remote places know there was an exception

    protected:


    };

} // namespace BlackCore

Q_DECLARE_METATYPE(BlackCore::IVoiceClient::ComUnit)

#endif // BLACKCORE_VOICE_H
