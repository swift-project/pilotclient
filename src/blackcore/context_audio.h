/* Copyright (C) 2013x VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKCORE_CONTEXTAUDIO_H
#define BLACKCORE_CONTEXTAUDIO_H

#include "blackcore/context.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/audiodevicelist.h"
#include "blackmisc/voiceroomlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/avcallsignlist.h"
#include "blackmisc/avselcal.h"
#include <QObject>

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTAUDIO_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextAudio"

//! DBus object path for context
#define BLACKCORE_CONTEXTAUDIO_OBJECTPATH "/Audio"

//! @}

namespace BlackCore
{

    //! \brief Audio context interface
    class IContextAudio : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)

    protected:
        //! \brief Constructor
        IContextAudio(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}

    public:
        //! \brief Interface name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTAUDIO_INTERFACENAME);
            return s;
        }

        //! \brief Object path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTAUDIO_OBJECTPATH);
            return s;
        }

        //! \copydoc CContext::getPathAndContextId()
        virtual QString getPathAndContextId() const { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextAudio *create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn);

        //! \brief Destructor
        virtual ~IContextAudio() {}

    signals:
        //! \brief Audio test has been completed
        void audioTestCompleted();

        //! Voice rooms changed
        //! \details the flag indicates, whether a room got connected or disconnected
        void changedVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms, bool connected);

        //! Volumes changed (COM1, COM2)
        //! \sa setVolumes
        // KB: Is see some potential changes here, which we should do when we have the new 2.0 vatlib
        // 1. volume integrated in voice room?
        // 2. Value object for volumes CVolume / CVolumeList?
        void changedAudioVolumes(QList<qint32> volumes);

        //! Mute changed
        void changedMute(bool muted);

    public slots:
        //! Get voice rooms for COM1, COM2:
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const = 0;

        //! Get voice rooms for COM1, COM2, but without latest audio status
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const = 0;

        /*!
         * \brief COM 1 voice room
         * \param withAudioStatus   update audio status
         * \return
         */
        virtual BlackMisc::Audio::CVoiceRoom getCom1VoiceRoom(bool withAudioStatus) const = 0;

        /*!
         * \brief COM 2 voice room
         * \param withAudioStatus   update audio status
         * \return
         */
        virtual BlackMisc::Audio::CVoiceRoom getCom2VoiceRoom(bool withAudioStatus) const = 0;

        //! Set voice rooms
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms) = 0;

        //! Leave all voice rooms
        virtual void leaveAllVoiceRooms() = 0;

        //! COM1 room users callsigns
        virtual BlackMisc::Aviation::CCallsignList getCom1RoomCallsigns() const = 0;

        //! COM2 room users callsigns
        virtual BlackMisc::Aviation::CCallsignList getCom2RoomCallsigns() const = 0;

        //! COM1 room users
        virtual BlackMisc::Network::CUserList getCom1RoomUsers() const = 0;

        //! COM2 room users
        virtual BlackMisc::Network::CUserList getCom2RoomUsers() const = 0;

        //! Audio devices
        virtual BlackMisc::Audio::CAudioDeviceList getAudioDevices() const = 0;

        /*!
         * \brief Get current audio device
         * \return input and output devices
         */
        virtual BlackMisc::Audio::CAudioDeviceList getCurrentAudioDevices() const = 0;

        /*!
         * \brief Set current audio device
         * \param audioDevice can be input or audio device
         */
        virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDevice &audioDevice) = 0;

        /*!
         * \brief Set volumes via com units, also allows to mute
         * \see BlackMisc::Aviation::CComSystem::setVolumeInput()
         * \see BlackMisc::Aviation::CComSystem::setVolumeOutput()
         */
        virtual void setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2) = 0;

        //! Set the volumes (0..100)
        virtual void setVolumes(qint32 volumeCom1, qint32 volumeCom2) = 0;

        //! Set mute state
        virtual void setMute(bool mute) = 0;

        //! Is muted?
        virtual bool isMuted() const = 0;

        //! Play SELCAL tone
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const = 0;

        /*!
         * \brief Play notification sound
         * \param notification CSoundGenerator::Notification
         * \param considerSettings consider settings (notification on/off), false means settings ignored
         */
        virtual void playNotification(uint notification, bool considerSettings) const = 0;

        //! Microphone test
        virtual void runMicrophoneTest() = 0;

        //! Microphone test
        virtual void runSquelchTest() = 0;

        //! Get the microphone test result
        virtual QString getMicrophoneTestResult() const = 0;

        //! Get the squelch value
        virtual double getSquelchValue() const = 0;

        //! Enable audio loopback
        virtual void enableAudioLoopback(bool enable = true) = 0;
    };
}

#endif // guard
