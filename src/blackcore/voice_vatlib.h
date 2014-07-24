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
#include <atomic>
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

        //! \copydoc IVoice::getVoiceChannel
        virtual IVoiceChannel *getVoiceChannel(qint32 channelIndex) const override;

        //! \copydoc IVoice::setInputDevice
        virtual void setInputDevice(const BlackMisc::Audio::CAudioDevice &device) override;

        //! \copydoc IVoice::setOutputDevice
        virtual void setOutputDevice(const BlackMisc::Audio::CAudioDevice &device) override;

        //! \copydoc IVoice::getCurrentInputDevice()
        virtual BlackMisc::Audio::CAudioDevice getCurrentInputDevice() const override;

        //! \copydoc IVoice::getCurrentOutputDevice()
        virtual BlackMisc::Audio::CAudioDevice getCurrentOutputDevice() const override;

        //! \copydoc IVoice::enableAudioLoopback
        virtual void enableAudioLoopback(bool enable = true) override;

        /*!
         * \brief Starts or stops voice transmission
         * \param value
         */
        void handlePushToTalk(bool value = false);

    protected: // QObject overrides

        /*!
         * \brief Process voice lib
         */
        virtual void timerEvent(QTimerEvent *);

    private slots:
        // slots for Mic tests
        void onEndFindSquelch();
        void onEndMicTest();

    private:

        // shimlib callbacks
        static void onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, qint32 roomIndex, void *cbVar);
        static void onInputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);
        static void onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);

        void exceptionDispatcher(const char *caller);
        void onRoomStatusUpdate(qint32 roomIndex, Cvatlib_Voice_Simple::roomStatusUpdate roomStatus);

        TVatlibPointer m_vatlib;
        BlackMisc::Audio::CAudioDeviceList m_devices; /*!< in and output devices */
        BlackMisc::Audio::CAudioDevice m_currentOutputDevice;
        BlackMisc::Audio::CAudioDevice m_currentInputDevice;
        std::atomic<float> m_inputSquelch;
        std::atomic<Cvatlib_Voice_Simple::agc> m_micTestResult;
        QHash<qint32, IVoiceChannel *> m_hashChannelIndex;
        bool m_isAudioLoopbackEnabled; /*!< A flag whether audio loopback is enabled or not */

        // Thread serialization
        mutable QMutex m_lockCurrentOutputDevice;
        mutable QMutex m_lockCurrentInputDevice;
        mutable QMutex m_lockDeviceList;
    };

} // namespace

#endif // guard
