/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAUDIO_IMPL_H
#define BLACKCORE_CONTEXTAUDIO_IMPL_H

#include "context_audio.h"
#include "context_settings.h"
#include "context_runtime.h"
#include "dbus_server.h"
#include "voice_vatlib.h"
#include "voice_channel.h"
#include "input_manager.h"
#include "blackinput/keyboard.h"
#include "blackmisc/voiceroomlist.h"

#include <QThread>
#include <QQueue>
#include <QPointer>

namespace BlackCore
{
    class IVoiceChannel;

    //! Audio context implementation
    class CContextAudio : public IContextAudio
    {
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)
        Q_OBJECT

        friend class CRuntime;
        friend class IContextAudio;

    public:

        //! Destructor
        virtual ~CContextAudio();

    public slots:
        //! \copydoc IContextAudio::getComVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const override;

        //! \copydoc IContextAudio::getComVoiceRoomsWithAudioStatus()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const override;

        //! \copydoc IContextAudio::getCom1VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom1VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextAudio::getCom2VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom2VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextAudio::setComVoiceRooms
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &newRooms) override;

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
        virtual void playNotification(uint notification, bool considerSettings) const override;

        //! \copydoc IContextAudio::runMicrophoneTest()
        virtual void runMicrophoneTest() override;

        //! \copydoc IContextAudio::runSquelchTest()
        virtual void runSquelchTest() override;

        //! \copydoc IContextAudio::getMicrophoneTestResult()
        virtual QString getMicrophoneTestResult() const override;

        //! \copydoc IContextAudio::getSquelchValue()
        virtual double getSquelchValue() const override;

        //! \copydoc IContextAudio::enableAudioLoopback()
        virtual void enableAudioLoopback(bool enable = true) override;

    protected:
        //! Constructor
        CContextAudio(CRuntimeConfig::ContextMode mode, CRuntime *runtime);

        //! Register myself in DBus
        CContextAudio *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextAudio::ObjectPath(), this);
            return this;
        }

    private slots:

        //! \copydoc IVoice::connectionStatusChanged
        //! \sa IContextAudio::changedVoiceRooms
        void ps_com1ConnectionStatusChanged(IVoiceChannel::ConnectionStatus oldStatus, IVoiceChannel::ConnectionStatus newStatus);

        //! \copydoc IVoice::connectionStatusChanged
        //! \sa IContextAudio::changedVoiceRooms
        void ps_com2ConnectionStatusChanged(IVoiceChannel::ConnectionStatus oldStatus, IVoiceChannel::ConnectionStatus newStatus);

        //! Init notification sounds
        void ps_initNotificationSounds();

    private:
        static const auto c_logContext = CRuntime::LogForAudio;

        //! Connection in transition
        bool inTransitionState() const;

        CVoiceVatlib *m_voice; //!< underlying voice lib
        CInputManager *m_inputManager;
        CInputManager::RegistrationHandle m_handlePtt;
        QThread m_threadVoice;
        QPointer<IVoiceChannel> m_channelCom1;
        QPointer<IVoiceChannel> m_channelCom2;
    };
}

#endif // guard
