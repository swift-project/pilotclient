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

#include "blackcore/blackcoreexport.h"
#include "blackcore/contextaudio.h"
#include "blackcore/contextruntime.h"
#include "blackcore/voice.h"
#include "blackcore/voicechannel.h"
#include "blackcore/audiodevice.h"
#include "blackcore/audiomixer.h"
#include "blackcore/actionbind.h"
#include "blackinput/keyboard.h"
#include "blackcore/settings/audio.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/audio/voiceroomlist.h"

#include <QThread>
#include <QQueue>
#include <QPointer>
#include <QScopedPointer>

namespace BlackCore
{
    class IVoiceChannel;

    //! Audio context implementation
    class BLACKCORE_EXPORT CContextAudio : public IContextAudio
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
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue, bool withAudioStatus) const override;

        //! \copydoc IContextAudio::setComVoiceRooms
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &newRooms) override;

        //! \copydoc IContextAudio::setOwnCallsignForRooms
        virtual void setOwnCallsignForRooms(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IContextAudio::getRoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignSet getRoomCallsigns(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const override;

        //! \copydoc IContextAudio::getRoomUsers()
        virtual BlackMisc::Network::CUserList getRoomUsers(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const override;

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
        virtual void playNotification(BlackMisc::Audio::CNotificationSounds::Notification notification, bool considerSettings) const override;

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
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

    protected:
        //! Constructor
        CContextAudio(CRuntimeConfig::ContextMode mode, CRuntime *runtime);

        //! Register myself in DBus
        CContextAudio *registerWithDBus(BlackMisc::CDBusServer *server)
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

        //! User joined the room
        void ps_userJoinedRoom(const BlackMisc::Aviation::CCallsign &callsign);

        //! User left the room
        void ps_userLeftRoom(const BlackMisc::Aviation::CCallsign &callsign);

    private:
        //! Connection in transition
        bool inTransitionState() const;

        //! Voice channel by room
        QSharedPointer<IVoiceChannel> getVoiceChannelBy(const BlackMisc::Audio::CVoiceRoom &voiceRoom);

        const int MinUnmuteVolume = 20; //!< minimum volume when unmuted
        CActionBind m_actionPtt { "/Voice/Activate push-to-talk", this, &CContextAudio::ps_setVoiceTransmission };
        std::unique_ptr<IVoice> m_voice; //!< underlying voice lib
        std::unique_ptr<IAudioMixer> m_audioMixer;
        int m_outVolumeBeforeMute = 90;

        // For easy access.
        QSharedPointer<IVoiceChannel> m_channel1;
        QSharedPointer<IVoiceChannel> m_channel2;
        std::unique_ptr<IAudioOutputDevice> m_voiceOutputDevice;
        std::unique_ptr<IAudioInputDevice> m_voiceInputDevice;

        QList<QSharedPointer<IVoiceChannel>> m_unusedVoiceChannels;
        QHash<BlackMisc::Aviation::CComSystem::ComUnit, QSharedPointer<IVoiceChannel>> m_voiceChannelMapping;

        // settings
        BlackMisc::CSetting<BlackCore::Settings::Audio::AudioSettings> m_audioSettings { this };
    };
} // namespace

#endif // guard
