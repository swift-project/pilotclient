/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAUDIO_IMPL_H
#define BLACKCORE_CONTEXTAUDIO_IMPL_H

#include "context_audio.h"
#include "coreruntime.h"
#include "dbus_server.h"
#include "voice_vatlib.h"

namespace BlackCore
{
    //! \brief Audio context implementation
    class CContextAudio : public IContextAudio
    {
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)
        Q_OBJECT

    public:

        //! \brief Constructor
        CContextAudio(QObject *runtime);

        //! \brief Destructor
        virtual ~CContextAudio();

        /*!
         * \brief Register myself in DBus
         * \param server DBus server
         */
        void registerWithDBus(CDBusServer *server)
        {
            Q_ASSERT(server);
            server->addObject(IContextAudio::ObjectPath(), this);
        }

        //! \brief Runtime
        CCoreRuntime *getRuntime()
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        //! \brief Const runtime
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        //! \copydoc IContextAudio::usingLocalObjects()
        virtual bool usingLocalObjects() const override { return true; }

    public slots:
        //! \copydoc IContextAudio::setOwnAircraft()
        virtual void setOwnAircraft(const BlackMisc::Aviation::CAircraft &ownAircraft) override;

        //! \copydoc IContextAudio::getComVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const override;

        //! \copydoc IContextAudio::getComVoiceRoomsWithAudioStatus()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const override;

        //! \copydoc IContextAudio::getCom1VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom1VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextAudio::getCom2VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom2VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextAudio::setComVoiceRooms()
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoom &voiceRoomCom1, const BlackMisc::Audio::CVoiceRoom &voiceRoomCom2) override;

        //! \copydoc IContextAudio::getCom1RoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getCom1RoomCallsigns() const override;

        //! \copydoc IContextAudio::getCom2RoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getCom2RoomCallsigns() const override;

        //! \copydoc IContextAudio::getCom1RoomUsers()
        virtual BlackMisc::Network::CUserList getCom1RoomUsers() const override;

        //! \copydoc IContextAudio::getCom2RoomUsers()
        virtual BlackMisc::Network::CUserList getCom2RoomUsers() const override;

        //! \copydoc IContextAudio::leaveAllVoiceRooms
        virtual void leaveAllVoiceRooms() override;

        //! \copydoc IContextAudio::getAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceList getAudioDevices() const override;

        //! \copydoc IContextAudio::getCurrentAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceList getCurrentAudioDevices() const override;

        //! \copydoc IContextAudio::setCurrentAudioDevice()
        virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDevice &audioDevice) override;

        //! \copydoc IContextAudio::setVolumes()
        virtual void setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2) override;

        //! \copydoc IContextAudio::isMuted()
        virtual bool isMuted() const override;

        //! \copydoc IContextAudio::playSelcalTone()
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const override;

        //! \copydoc IContextAudio::playNotification()
        virtual void playNotification(uint notification) const override;

        //! \copydoc IContextAudio::runMicrophoneTest()
        virtual void runMicrophoneTest() override;

        //! \copydoc IContextAudio::runSquelchTest()
        virtual void runSquelchTest() override;

        //! \copydoc IContextAudio::getMicrophoneTestResult()
        virtual QString getMicrophoneTestResult() const override;

        //! \copydoc IContextAudio::getSquelchValue()
        virtual double getSquelchValue() const override;

    private:
        CVoiceVatlib *m_voice; //!< underlying voice lib
    };
}

#endif // guard
