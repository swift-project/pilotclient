/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTAUDIO_IMPL_H
#define BLACKCORE_CONTEXTAUDIO_IMPL_H

#include "context_audio.h"
#include "context_settings.h"
#include "context_runtime.h"
#include "dbus_server.h"
#include "voice.h"
#include "voice_channel.h"
#include "audio_device.h"
#include "audio_mixer.h"
#include "input_manager.h"
#include "blackinput/keyboard.h"
#include "blackmisc/voiceroomlist.h"

#include <QThread>
#include <QQueue>
#include <QPointer>
#include <QScopedPointer>

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

        //! \copydoc IContextAudio::getVoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom(int comUnitValue, bool withAudioStatus) const override;

        //! \copydoc IContextAudio::setComVoiceRooms
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &newRooms) override;

        //! \copydoc IContextAudio::setOwnCallsignForRooms
        virtual void setOwnCallsignForRooms(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IContextAudio::getRoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getRoomCallsigns(int comUnitValue) const override;

        //! \copydoc IContextAudio::getRoomUsers()
        virtual BlackMisc::Network::CUserList getRoomUsers(int comUnitValue) const override;

        //! \copydoc IContextAudio::leaveAllVoiceRooms
        virtual void leaveAllVoiceRooms() override;

        //! \copydoc IContextAudio::getAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const override;

        //! \copydoc IContextAudio::getCurrentAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const override;

        //! \copydoc IContextAudio::setCurrentAudioDevice()
        virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice) override;

        //! \copydoc IContext::setVoiceOutputVolume
        virtual void setVoiceOutputVolume(int volume) override;

        //! \copydoc IContext::getVoiceOutputVolume
        virtual int getVoiceOutputVolume() const override;

        //! \copydoc ICOntext::setMute
        virtual void setMute(bool muted) override;

        //! \copydoc IContextAudio::isMuted()
        virtual bool isMuted() const override;

        //! \copydoc IContextAudio::playSelcalTone()
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const override;

        //! \copydoc IContextAudio::playNotification()
        virtual void playNotification(uint notification, bool considerSettings) const override;

        //! \copydoc IContextAudio::enableAudioLoopback()
        virtual void enableAudioLoopback(bool enable = true) override;

        //! \copydoc ICOntextAudio::isAudioLoopbackEnabled
        virtual bool isAudioLoopbackEnabled() const override;

        //! \addtogroup commandline
        //! @{
        //! <pre>
        //! .mute                          mute             CContextAudio
        //! .unmute                        unmute           CContextAudio
        //! .vol .volume   volume 0..300   set volume       CContextAudio
        //! </pre>
        //! @}
        //! \copydoc IContextAudio::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const QString &originator) override;

    protected:
        //! Constructor
        CContextAudio(CRuntimeConfig::ContextMode mode, CRuntime *runtime);

        //! Register myself in DBus
        CContextAudio *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) { return this; }
            server->addObject(IContextAudio::ObjectPath(), this);
            return this;
        }

    private slots:

        //! \copydoc IVoice::connectionStatusChanged
        //! \sa IContextAudio::changedVoiceRooms
        void ps_connectionStatusChanged(IVoiceChannel::ConnectionStatus oldStatus, IVoiceChannel::ConnectionStatus newStatus);

        //! Init notification sounds
        void ps_initNotificationSounds();

        void ps_setVoiceTransmission(bool enable);

    private:
        const int MinUnmuteVolume = 20; //!< minimum volume when unmuted

        //! Connection in transition
        bool inTransitionState() const;

        QSharedPointer<IVoiceChannel> getVoiceChannelBy(const BlackMisc::Audio::CVoiceRoom &voiceRoom);

        CInputManager *m_inputManager = nullptr;
        CInputManager::RegistrationHandle m_handlePtt;

        std::unique_ptr<IVoice> m_voice; //!< underlying voice lib
        std::unique_ptr<IAudioMixer> m_audioMixer;
        int m_outVolumeBeforeMute = 90;

        // For easy access.
        QSharedPointer<IVoiceChannel> m_channel1;
        QSharedPointer<IVoiceChannel> m_channel2;
        std::unique_ptr<IAudioOutputDevice> m_voiceOutputDevice;
        std::unique_ptr<IAudioInputDevice> m_voiceInputDevice;

        QList<QSharedPointer<IVoiceChannel>> m_unusedVoiceChannels;
        QHash<ComUnit, QSharedPointer<IVoiceChannel>> m_voiceChannelMapping;
    };
} // namespace

#endif // guard
