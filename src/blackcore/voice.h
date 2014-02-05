/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "../blackmisc/context.h"
#include "../blackmisc/avcallsignlist.h"
#include "../blackmisc/avselcal.h"
#include "../blackmisc/nwuserlist.h"
#include "../blackmisc/vvoiceroomlist.h"
#include "../blackmisc/vaudiodevicelist.h"
#include <vatlib/vatlib.h>
#include <QObject>
#include <QSet>
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
    class IVoice : public QObject
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
        IVoice(QObject *parent = nullptr);

    public:

        //! ComUnit
        /*! IVoiceClient currently supports two different com units */
        enum ComUnit
        {
            COM1 = 0,   /*!< ComUnit 1 */
            COM2        /*!< ComUnit 2 */
        };

        //! Virtual destructor.
        virtual ~IVoice() {}

        /*!
         * \brief Own aircraft's callsign
         * \param callsign
         */
        virtual void setMyAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * \brief Audio devices
         * \return
         */
        virtual const BlackMisc::Voice::CAudioDeviceList &audioDevices() const = 0;

        /*!
         * \brief Default input device
         * \return
         */
        virtual const BlackMisc::Voice::CAudioDevice defaultAudioInputDevice() const = 0;

        /*!
         * \brief Default output device
         * \return
         */
        virtual const BlackMisc::Voice::CAudioDevice defaultAudioOutputDevice() const = 0;

        /************************************************
         * SETUP TESTS
         * *********************************************/

        /*!
         * \brief Runs a 5 seconds test, measuring your background noise.
         */
        virtual void runSquelchTest() = 0;

        /*!
         * \brief Runs a 5 seconds test, measuring the qualitiy of your mic input
         */
        virtual void runMicTest() = 0;

        /*!
         * \brief Value of the measured squelch
         * \return
         */
        virtual float inputSquelch() const = 0;

        /*!
         * \brief Result of the mic test.
         * \return
         */
        virtual qint32 micTestResult() const = 0;

        /*!
         * \brief Result of the mic test as human readable string
         * \return
         */
        virtual QString micTestResultAsString() const = 0;

    public slots:

        /*!
         * \brief Current input device
         */
        virtual BlackMisc::Voice::CAudioDevice getCurrentInputDevice() const = 0;

        /*!
         * \brief Current output device
         */
        virtual BlackMisc::Voice::CAudioDevice getCurrentOutputDevice() const = 0;

        /*!
         * \brief Output device to be used
         */
        virtual void setOutputDevice(const BlackMisc::Voice::CAudioDevice &device) = 0;

        /*!
         * \brief Input device to be used
         */
        virtual void setInputDevice(const BlackMisc::Voice::CAudioDevice &device) = 0;

        /*!
         * Get COM1/2 voice rooms, which then allows to retrieve information
         * such as audio status etc.
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const = 0;

        /*!
         * Get COM1/2 voice rooms, const and with no status update
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRooms() const = 0;

        /*!
         * \brief Join voice room
         * \param comUnit   COM1/2
         * \param voiceRoom
         */
        virtual void joinVoiceRoom(const ComUnit comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom) = 0;

        /*!
         * \brief Leave voice room
         * \param comUnit   COM1/2
         */
        virtual void leaveVoiceRoom(const ComUnit comUnit) = 0;

        /*!
         * \brief Leave all voice rooms
         */
        virtual void leaveAllVoiceRooms() = 0;

        /*!
         * \brief Set room output volume for COM unit
         */
        virtual void setRoomOutputVolume(const ComUnit comUnit, const qint32 volumne) = 0;

        /*!
         * \brief Start transmitting
         */
        virtual void startTransmitting(const ComUnit comUnit) = 0;

        /*!
         * \brief Stop transmitting
         */
        virtual void stopTransmitting(const ComUnit comUnit) = 0;

        /*!
         * \brief Get voice room callsings
         * \param comUnit
         * \return
         */
        virtual BlackMisc::Aviation::CCallsignList getVoiceRoomCallsigns(const ComUnit comUnit) const = 0;

        /*!
         * \brief Is muted?
         */
        virtual bool isMuted() const = 0;

        /*!
         * \brief Switch audio output, enable or disable given COM unit.
         * \param comUnit
         * \param enable   enable or disable output
         */
        virtual void switchAudioOutput(const ComUnit comUnit, bool enable) = 0;

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
        /*!
         * \brief User with callsign joined room
         */
        void userJoinedRoom(const BlackMisc::Aviation::CCallsign &callsign);
        /*!
         * \brief User with callsign left room
         */
        void userLeftRoom(const BlackMisc::Aviation::CCallsign &callsign);

        // Audio signals
        /*!
         * \brief Audio for given unit started
         */
        void audioStarted(const ComUnit comUnit);
        /*!
         * \brief Audio for given unit stopped
         */
        void audioStopped(const ComUnit comUnit);
        void globalAudioStarted();
        void globalAudioStopped();

        // Test signals
        /*!
         * \brief Squelch test completed
         */
        void squelchTestFinished();
        /*!
         * \brief Microphone test completed
         */
        void micTestFinished();

        // non protocol related signals
        /*!
         * \brief Exception
         */
        void exception(const QString &message, bool fatal = false); // let remote places know there was an exception
    };

} // namespace BlackCore

Q_DECLARE_METATYPE(BlackCore::IVoice::ComUnit)

#endif // guard
