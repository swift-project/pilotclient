/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "../blackmisc/context.h"
#include "../blackmisc/avcallsign.h"
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
        virtual qint32 micTestResult() const = 0;

        //! micTestResult
        /*!
          \return Result of the mic test as human readable string
        */
        virtual QString micTestResultAsString() const = 0;

    public slots:

        //! setInputDevice
        /*!
          \param input device
        */
        virtual void setInputDevice(const BlackMisc::Voice::CAudioDevice &device) = 0;

        //! setOutputDevice
        /*!
          \param output device
        */
        virtual void setOutputDevice(const BlackMisc::Voice::CAudioDevice &device) = 0;

        /*!
         * Get COM1/2 voice rooms, which then allows to retrieve information
         * such as connection status etc.
         * \return
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRoomsWithAudioStatus() = 0;

        /*!
         * Get COM1/2 voice rooms, const and with no status update
         * \return
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRooms() const = 0;

        /*!
         * \brief Join voice room
         * \param comUnit
         * \param voiceRoom
         */
        virtual void joinVoiceRoom(const ComUnit comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom) = 0;

        /*!
         * \brief Leave voice room
         * \param comUnit
         */
        virtual void leaveVoiceRoom(const ComUnit comUnit) = 0;

        /*!
         * \brief Leave all voice rooms
         */
        virtual void leaveAllVoiceRooms() = 0;

        /*!
         * \brief Set room output volume
         * \param comUnit
         * \param volumne
         */
        virtual void setRoomOutputVolume(const ComUnit comUnit, const qint32 volumne) = 0;

        /*!
         * \brief Start transmitting
         * \param comUnit
         */
        virtual void startTransmitting(const ComUnit comUnit) = 0;

        /*!
         * \brief Stop transmitting
         * \param comUnit
         */
        virtual void stopTransmitting(const ComUnit comUnit) = 0;

        /*!
         * \brief Get voice room callsings
         * \param comUnit
         * \return
         */
        virtual QSet<QString> getVoiceRoomCallsings(const ComUnit comUnit) const = 0;

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
    };

} // namespace BlackCore

Q_DECLARE_METATYPE(BlackCore::IVoice::ComUnit)

#endif // guard
