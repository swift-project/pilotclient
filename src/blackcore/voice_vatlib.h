/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_VATLIB_H
#define BLACKCORE_VOICE_VATLIB_H

#include "voice.h"
#include "../blacksound/soundgenerator.h"
#include "../blackmisc/audiodevicelist.h"
#include "../blackmisc/nwuserlist.h"
#include "../blackmisc/avcallsignlist.h"
#include "../blackmisc/sharedlockablepointer.h"

#include <QMap>
#include <QSet>
#include <QString>
#include <QMutex>
#include <QReadWriteLock>
#include <utility>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

//! \file

namespace BlackCore
{
    /*!
     * \brief Deleter
     */
    struct Cvatlib_Voice_Simple_Deleter
    {
        /*!
         * \brief Cleanup
         * \param pointer
         */
        static inline void cleanup(Cvatlib_Voice_Simple *pointer)
        {
            if (pointer) pointer->Destroy();
        }
    };

    //! Shared lockable pointer to Cvatlib_Voice_Simple
    typedef BlackMisc::SharedLockablePtr<Cvatlib_Voice_Simple> TVatlibPointer;

    /*!
     * Vatlib implementation of the IVoice interface.
     */
    class CVoiceVatlib : public IVoice
    {
        Q_OBJECT

    public:
        /*!
         * \brief Constructor
         * \param parent
         */
        CVoiceVatlib(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CVoiceVatlib();

        // Hardware devices
        // TODO: Vatlib supports multiple output devices. That basically means, you could connect
        // to different voice rooms and send their audio to different devices, e.g. ATIS to loudspeakers
        // and ATC to headspeakers. Is not important to implement that now, if ever.

        //! \copydoc IVoice::audioDevices()
        virtual const BlackMisc::Audio::CAudioDeviceList &audioDevices() const override;

        //! \copydoc IVoice::defaultAudioInputDevice()
        virtual const BlackMisc::Audio::CAudioDevice defaultAudioInputDevice() const override;

        //! \copydoc IVoice::defaultAudioOutputDevice()
        virtual const BlackMisc::Audio::CAudioDevice defaultAudioOutputDevice() const override;

        /************************************************
         * SETUP TESTS
         ***********************************************/

        //! \copydoc IVoice::runSquelchTest
        virtual void runSquelchTest() override;

        //! \copydoc IVoice::runMicTest
        virtual void runMicrophoneTest() override;

        //! \copydoc IVoice::inputSquelch
        virtual float inputSquelch() const override;

        //! \copydoc IVoice::micTestResult()
        virtual qint32 micTestResult() const override;

        //! \copydoc IVoice::micTestResultAsString
        virtual QString micTestResultAsString() const override;

    public slots:
        //! \copydoc IVoice::setMyAircraftCallsign()
        virtual void setMyAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IVoice::joinVoiceRoom()
        virtual void joinVoiceRoom(const ComUnit comUnit, const BlackMisc::Audio::CVoiceRoom &voiceRoom) override;

        //! \copydoc IVoice::leaveVoiceRoom()
        virtual void leaveVoiceRoom(const ComUnit comUnit) override;

        //! \copydoc IVoice::leaveAllVoiceRooms()
        virtual void leaveAllVoiceRooms() override;

        //! \copydoc IVoice::setRoomOutputVolume()
        virtual void setRoomOutputVolume(const ComUnit comUnit, const qint32 volumne) override;

        //! \copydoc IVoice::startTransmitting()
        virtual void startTransmitting(const ComUnit comUnit) override;

        //! \copydoc IVoice::stopTransmitting()
        virtual void stopTransmitting(const ComUnit comUnit) override;

        //! \copydoc IVoice::getComVoiceRoomsWithAudioStatus()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const override;

        //! \copydoc IVoice::getComVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const override
        {
            QReadLocker lockForReading(&m_lockVoiceRooms);
            return this->m_voiceRooms;
        }

        //! \copydoc IVoice::getVoiceRoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getVoiceRoomCallsigns(const ComUnit comUnit) const override;

        //! \copydoc IVoice::setInputDevice
        virtual void setInputDevice(const BlackMisc::Audio::CAudioDevice &device) override;

        //! \copydoc IVoice::setOutputDevice
        virtual void setOutputDevice(const BlackMisc::Audio::CAudioDevice &device) override;

        //! \copydoc IVoice::getCurrentInputDevice()
        virtual BlackMisc::Audio::CAudioDevice getCurrentInputDevice() const override;

        //! \copydoc IVoice::getCurrentOutputDevice()
        virtual BlackMisc::Audio::CAudioDevice getCurrentOutputDevice() const override;

        //! \copydoc IVoice::switchAudioOutput
        virtual void switchAudioOutput(const ComUnit comUnit, bool enable) override;

        //! \copydoc IVoice::enableAudioLoopback
        virtual void enableAudioLoopback(bool enable = true) override;

        //! \copydoc IVoice::isMuted
        virtual bool isMuted() const override
        {
            QReadLocker lockForReading(&m_lockOutputEnabled);
            if (this->m_outputEnabled.isEmpty()) return false;
            bool enabled = this->m_outputEnabled[COM1] || this->m_outputEnabled[COM2];
            return !enabled;
        }

        /*!
         * \brief Starts or stops voice transmission
         * \param value
         */
        void handlePushToTalk(bool value = false);

        /************************************************
         * NON API METHODS:
         * The following methods are not part of the
         * public API. They are needed for internal
         * workflow.
         * *********************************************/

        /*!
         * \brief Voice room index
         * \return
         */
        qint32 temporaryUserRoomIndex() const
        {
            return m_temporaryUserRoomIndex;
        }

        /*!
         * \brief Room status update, used in callback
         * \param comUnit
         * \param roomStatus
         */
        void changeRoomStatus(ComUnit comUnit, Cvatlib_Voice_Simple::roomStatusUpdate roomStatus);

    signals:

        /*!
         * \brief User joined or left
         * \param comUnit
         */
        void userJoinedLeft(const ComUnit comUnit);

    protected: // QObject overrides

        /*!
         * \brief Process voice lib
         */
        virtual void timerEvent(QTimerEvent *);

    private slots:
        // slots for Mic tests
        void onEndFindSquelch();
        void onEndMicTest();

        /*!
         * \brief User (identified by callsign) joined or left voice room
         */
        void onUserJoinedLeft(const ComUnit comUnit);

    private:

        // shimlib callbacks
        static void onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, qint32 roomIndex, void *cbVar);
        static void onRoomUserReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);
        static void onInputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);
        static void onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);

        BlackMisc::Audio::CVoiceRoom voiceRoomForUnit(const ComUnit comUnit) const;
        void setVoiceRoomForUnit(const IVoice::ComUnit comUnit, const BlackMisc::Audio::CVoiceRoom &voiceRoom);
        void addTemporaryCallsignForRoom(const ComUnit comUnit, const BlackMisc::Aviation::CCallsign &callsign);
        void removeUserFromRoom(const ComUnit comUnit, const QString &callsign);
        void exceptionDispatcher(const char *caller);
        void enableAudio(const ComUnit comUnit);
        void changeConnectionStatus(ComUnit comUnit, ConnectionStatus newStatus);

        TVatlibPointer m_vatlib;
        // QScopedPointer<QAudioOutput> m_audioOutput; #227
        BlackMisc::Aviation::CCallsign m_aircraftCallsign; /*!< own callsign to join voice rooms */
        BlackMisc::Audio::CVoiceRoomList m_voiceRooms;
        BlackMisc::Audio::CAudioDeviceList m_devices; /*!< in and output devices */
        BlackMisc::Audio::CAudioDevice m_currentOutputDevice;
        BlackMisc::Audio::CAudioDevice m_currentInputDevice;
        float m_inputSquelch;
        Cvatlib_Voice_Simple::agc m_micTestResult;
        QMap <ComUnit, BlackMisc::Aviation::CCallsignList> m_voiceRoomCallsigns; /*!< voice room callsigns */
        BlackMisc::Aviation::CCallsignList m_temporaryVoiceRoomCallsigns; /*!< temp. storage of voice rooms during update */
        QMap<ComUnit, bool> m_outputEnabled; /*!< output enabled, basically a mute flag */
        QMap<ComUnit, ConnectionStatus> m_connectionStatus;  /*!< holds connection status for each com unit */
        bool m_isAudioLoopbackEnabled; /*!< A flag whether audio loopback is enabled or not */

        // Need to keep the roomIndex?
        // KB: I would remove this approach, it is potentially unsafe
        //     Maybe just use 2 "wrapper" callbacks, which then set explicitly the voice room (it is only 2 methods)
        qint32 m_temporaryUserRoomIndex; /*!< temp. storage of voice room, in order to retrieve it in static callback */
        const static qint32 InvalidRoomIndex = -1; /*! marks invalid room */

        // Thread serialization
        mutable QReadWriteLock m_lockVoiceRooms;
        mutable QReadWriteLock m_lockCallsigns;
        mutable QReadWriteLock m_lockCurrentOutputDevice;
        mutable QReadWriteLock m_lockCurrentInputDevice;
        mutable QReadWriteLock m_lockDeviceList;
        mutable QReadWriteLock m_lockOutputEnabled;
        mutable QReadWriteLock m_lockSquelch;
        mutable QReadWriteLock m_lockTestResult;
        mutable QReadWriteLock m_lockMyCallsign;
        mutable QReadWriteLock m_lockConnectionStatus;

    };

} // namespace

#endif // guard
