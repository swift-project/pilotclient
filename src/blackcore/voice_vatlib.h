/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_VATLIB_H
#define BLACKCORE_VOICE_VATLIB_H

#include "voice.h"
#include "../blackmisc/vaudiodevicelist.h"
#include "../blackmisc/nwuserlist.h"
#include "../blackmisc/avcallsignlist.h"

#include <QScopedPointer>
#include <QMap>
#include <QSet>
#include <QString>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace BlackCore
{
    /*!
     * Vatlib implementation of the IVoiceClient interface.
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

        /*!
         * \brief Destructor
         */
        virtual ~CVoiceVatlib();

        // Hardware devices
        // TODO: Vatlib supports multiple output devices. That basically means, you could connect
        // to different voice rooms and send their audio to different devices, e.g. ATIS to loudspeakers
        // and ATC to headspeakers. Is not important to implement that now, if ever.
        virtual const BlackMisc::Voice::CAudioDeviceList &audioDevices() const ;
        virtual const BlackMisc::Voice::CAudioDevice defaultAudioInputDevice() const;
        virtual const BlackMisc::Voice::CAudioDevice defaultAudioOutputDevice() const;
        virtual void setInputDevice(const BlackMisc::Voice::CAudioDevice &device);
        virtual void setOutputDevice(const BlackMisc::Voice::CAudioDevice &device);

        /************************************************
         * SETUP TESTS
         * *********************************************/

        // Mic tests
        virtual void runSquelchTest();
        virtual void runMicTest();

        virtual float inputSquelch() const;
        virtual qint32 micTestResult() const;
        virtual QString micTestResultAsString() const;


    public slots:
        /*!
         * \brief ATC station callsign, used for voice room designator
         * \param callsign
         */
        virtual void setMyAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        /*!
         * \brief Join a given voice room
         * \param comUnit
         * \param voiceRoom
         */
        virtual void joinVoiceRoom(const ComUnit comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom);

        /*!
         * \brief Leave voice room
         * \param comUnit
         */
        virtual void leaveVoiceRoom(const ComUnit comUnit);

        /*!
         * \brief Leave all voice rooms
         */
        virtual void leaveAllVoiceRooms();

        /*!
         * \brief Room output volume as per COM unit
         * \param comUnit
         * \param volumne
         */
        virtual void setRoomOutputVolume(const ComUnit comUnit, const qint32 volumne);

        /*!
         * \brief Start transmitting ("talk")
         * \param comUnit
         */
        virtual void startTransmitting(const ComUnit comUnit);

        /*!
         * \brief Stop transmitting ("talk")
         * \param comUnit
         */
        virtual void stopTransmitting(const ComUnit comUnit);

        /*!
         * Get COM1/2 voice rooms, which then allows to retrieve information
         * such as connection status etc.
         * \return
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRoomsWithAudioStatus();

        /*!
         * \brief Voice rooms, const version with no updates
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRooms() const
        {
            return this->m_voiceRooms;
        }

        /*!
         * \brief Get voice room callsigns
         * \param comUnit COM1/2
         * \return
         */
        virtual BlackMisc::Aviation::CCallsignList getVoiceRoomCallsigns(const ComUnit comUnit) const;

        /*!
          * \brief Switch audio output
          * \param comUnit
          * \param enable
          */
        virtual void switchAudioOutput(const ComUnit comUnit, bool enable);

        /*!
         * \brief Muted?
         */
        virtual bool isMuted() const
        {
            if (this->m_outputEnabled.isEmpty()) return false;
            bool enabled = this->m_outputEnabled[COM1] || this->m_outputEnabled[COM2];
            return !enabled;
        }

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
         * \param upd
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

        // slot to handle users
        void onUserJoinedLeft(const ComUnit comUnit);

    private:

        // shimlib callbacks
        static void onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, qint32 roomIndex, void *cbVar);
        static void onRoomUserReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);
        static void onInputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);
        static void onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar);

        BlackMisc::Voice::CVoiceRoom voiceRoomForUnit(const ComUnit comUnit) const;
        void setVoiceRoomForUnit(const IVoice::ComUnit comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom);
        void addTemporaryCallsignForRoom(const ComUnit comUnit, const BlackMisc::Aviation::CCallsign &callsign);
        void removeUserFromRoom(const ComUnit comUnit, const QString &callsign);
        void exceptionDispatcher(const char *caller);
        void enableAudio(const ComUnit comUnit);
        void handlePushToTalk();

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
                pointer->Destroy();
            }
        };

#ifdef Q_OS_WIN

        /*!
         * \brief Keyboard PTT handling class
         */
        class CKeyboard
        {
        public:
            // Keyboard hook
            static HHOOK s_keyboardHook;
            static CVoiceVatlib *s_voice;
            static LRESULT CALLBACK keyboardProcedure(int nCode, WPARAM wParam, LPARAM lParam);

            /*!
             * \brief Constructor, keyboard handling
             */
            CKeyboard(CVoiceVatlib *vatlib)
            {
                CVoiceVatlib::CKeyboard::s_voice = vatlib;
                CVoiceVatlib::CKeyboard::s_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, CVoiceVatlib::CKeyboard::keyboardProcedure, GetModuleHandle(NULL), 0);
            }

            /*!
             * Destructor
             */
            ~CKeyboard()
            {
                if (!CVoiceVatlib::CKeyboard::s_keyboardHook) return;
                UnhookWindowsHookEx(CVoiceVatlib::CKeyboard::s_keyboardHook);
                CVoiceVatlib::CKeyboard::s_keyboardHook = nullptr;
                CVoiceVatlib::CKeyboard::s_voice = nullptr;
            }
        };

#else

        /*!
         * \brief Keyboard PTT handling class
         */
        class CKeyboard
        {
        public:
            static CVoiceVatlib *s_voice;

            /*!
             * \brief Constructor, keyboard handling
             */
            CKeyboard(CVoiceVatlib *vatlib)
            {
                CVoiceVatlib::CKeyboard::s_voice = vatlib;
            }

            /*!
             * Destructor
             */
            ~CKeyboard()
            {
                // void
            }
        };

#endif

        QScopedPointer<Cvatlib_Voice_Simple, Cvatlib_Voice_Simple_Deleter> m_voice;
        BlackMisc::Aviation::CCallsign m_aircraftCallsign; /*!< own callsign to join voice rooms */
        BlackMisc::Voice::CVoiceRoomList m_voiceRooms;
        BlackMisc::Voice::CAudioDeviceList m_devices; /*!< in and output devices */
        QScopedPointer<CKeyboard> m_keyboardPtt; /*!< handler for PTT */
        bool m_pushToTalk; /*!< flag, PTT pressed */
        float m_inputSquelch;
        Cvatlib_Voice_Simple::agc m_micTestResult;
        QMap <ComUnit, BlackMisc::Aviation::CCallsignList> m_voiceRoomCallsigns; /*!< voice room callsigns */
        BlackMisc::Aviation::CCallsignList m_temporaryVoiceRoomCallsigns; /*!< temp. storage of voice rooms during update */
        QMap<ComUnit, bool> m_outputEnabled; /*!< output enabled, basically a mute flag */

        // Need to keep the roomIndex?
        // KB: I would remove this approach, it is potentially unsafe
        //     Maybe just use 2 "wrapper" callbacks, which then set explicitly the voice room (it is only 2 methods)
        qint32 m_temporaryUserRoomIndex; /*!< temp. storage of voice room, in order to retrieve it in static callback */
        const static qint32 InvalidRoomIndex = -1; /*! marks invalid room */

    };

} // namespace

#endif // guard
