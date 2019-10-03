/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAUDIO_H
#define BLACKCORE_CONTEXT_CONTEXTAUDIO_H

#include "blackcore/afv/clients/afvclient.h"
#include "blackcore/audio/audiosettings.h"
#include "blackcore/context/context.h"
#include "blackcore/actionbind.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blacksound/selcalplayer.h"
#include "blacksound/notificationplayer.h"
#include "blackmisc/macos/microphoneaccess.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/audio/ptt.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/input/actionhotkeydefs.h"
#include "blackmisc/identifier.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace BlackMisc { class CDBusServer; }

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTAUDIO_INTERFACENAME "org.swift_project.blackcore.contextaudio"

//! DBus object path for context
#define BLACKCORE_CONTEXTAUDIO_OBJECTPATH "/audio"

//! @}

namespace BlackCore
{
    namespace Context
    {
        //! Audio context interface
        class BLACKCORE_EXPORT IContextAudio : public IContext
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)

            friend class BlackCore::CCoreFacade;

        protected:
            //! Constructor
            IContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        public:
            //! Interface name
            static const QString &InterfaceName();

            //! Object path
            static const QString &ObjectPath();

            //! \copydoc CContext::getPathAndContextId()
            virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextAudio *create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

            //! Destructor
            virtual ~IContextAudio() override;

            // -------- parts which can run in core and GUI, referring to local voice client ------------

            //! Reference to voice client
            BlackCore::Afv::Clients::CAfvClient &voiceClient() { return m_voiceClient; }

            //! Audio devices
            //! @{
            BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const;
            BlackMisc::Audio::CAudioDeviceInfoList getAudioInputDevices()  const { return this->getAudioDevices().getInputDevices(); }
            BlackMisc::Audio::CAudioDeviceInfoList getAudioOutputDevices() const { return this->getAudioDevices().getOutputDevices(); }
            //! @}

            //! Get current audio device
            //! \return input and output devices
            BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const;

            //! Set current audio device
            //! \param audioDevice can be input or audio device
            void setCurrentAudioDevices(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice, const BlackMisc::Audio::CAudioDeviceInfo &outputDevice);

            //! Volume
            //! @{
            void setVoiceOutputVolume(int volume);
            int  getVoiceOutputVolume() const;
            void setMute(bool muted);
            bool isMuted() const;
            //! @}

            //! SELCAL
            void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal);

            //! Notification sounds
            void playNotification(BlackMisc::Audio::CNotificationSounds::NotificationFlag notification, bool considerSettings, int volume = -1);

            //! Loopback
            //! @{
            void enableAudioLoopback(bool enable = true);
            bool isAudioLoopbackEnabled() const;
            //! @}

            //! Voice setup
            //! @{
            BlackMisc::Audio::CVoiceSetup getVoiceSetup() const;
            void setVoiceSetup(const BlackMisc::Audio::CVoiceSetup &setup);
            //! @}

            //! Info string about audio
            QString audioRunsWhereInfo() const;

            //! Audio runs where
            const BlackMisc::CIdentifier &audioRunsWhere() const;

            // -------- parts which can run in core and GUI, referring to local voice client ------------

        signals:
            // -------- local settings, not DBus relayed -------

            //! Audio volume changed
            //! \sa setVoiceOutputVolume
            void changedAudioVolume(int volume);

            //! PTT status in a particular voice client
            void ptt(bool active, BlackMisc::Audio::PTTCOM pttcom, const BlackMisc::CIdentifier &identifier);

            //! Mute changed
            void changedMute(bool muted);

            //! Changed audio devices (e.g. device enabled/disable)
            void changedAudioDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Changed slection of audio devices
            void changedSelectedAudioDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            // -------- local settings, not DBus relayed -------

        public slots:
            // ------------- DBus ---------------

            //! \addtogroup swiftdotcommands
            //! <pre>
            //! .mute                          mute             BlackCore::Context::CContextAudio
            //! .unmute                        unmute           BlackCore::Context::CContextAudio
            //! .vol .volume   volume 0..100   set volume       BlackCore::Context::CContextAudio
            //! </pre>
            //! \copydoc IContextAudio::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            // ------------- DBus ---------------

        private:
            //! Enable/disable voice transmission, nornally used with hotkey
            //! @{
            void setVoiceTransmission(bool enable, BlackMisc::Audio::PTTCOM com);
            void setVoiceTransmissionCom1(bool enabled);
            void setVoiceTransmissionCom2(bool enabled);
            void setVoiceTransmissionComActive(bool enabled);
            //! @}

            //! Change the device settings
            void changeDeviceSettings();

            //! Changed audio settings
            void onChangedAudioSettings();

            //! Audio increase/decrease volume
            //! @{
            void audioIncreaseVolume(bool enabled);
            void audioDecreaseVolume(bool enabled);
            //! @}

            //! Get current COM unit from cockpit
            //! \remark cross context
            //! @{
            BlackMisc::Aviation::CComSystem getOwnComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const;
            bool isComIntegratedWithSimulator() const;
            //! @}

            //! Changed cockpit
            //! \remark cross context
            void xCtxChangedAircraftCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Network connection status
            void xCtxNetworkConnectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

            CActionBind m_actionPtt     { BlackMisc::Input::pttHotkeyAction(),     BlackMisc::Input::pttHotkeyIcon(), this, &IContextAudio::setVoiceTransmissionComActive };
            CActionBind m_actionPttCom1 { BlackMisc::Input::pttCom1HotkeyAction(), BlackMisc::Input::pttHotkeyIcon(), this, &IContextAudio::setVoiceTransmissionCom1 };
            CActionBind m_actionPttCom2 { BlackMisc::Input::pttCom2HotkeyAction(), BlackMisc::Input::pttHotkeyIcon(), this, &IContextAudio::setVoiceTransmissionCom2 };
            CActionBind m_actionAudioVolumeIncrease { BlackMisc::Input::audioVolumeIncreaseHotkeyAction(), BlackMisc::Input::audioVolumeIncreaseHotkeyIcon(), this, &IContextAudio::audioIncreaseVolume };
            CActionBind m_actionAudioVolumeDecrease { BlackMisc::Input::audioVolumeDecreaseHotkeyAction(), BlackMisc::Input::audioVolumeDecreaseHotkeyIcon(), this, &IContextAudio::audioDecreaseVolume };

            int m_outVolumeBeforeMute = 90;
            static constexpr int MinUnmuteVolume = 20; //!< minimum volume when unmuted

            // settings
            BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings  { this, &IContextAudio::onChangedAudioSettings };
            BlackMisc::CSetting<Audio::TInputDevice>    m_inputDeviceSetting  { this, &IContextAudio::changeDeviceSettings };
            BlackMisc::CSetting<Audio::TOutputDevice>   m_outputDeviceSetting { this, &IContextAudio::changeDeviceSettings };

            // AFV
            Afv::Clients::CAfvClient m_voiceClient;

            // Players
            BlackSound::CSelcalPlayer      *m_selcalPlayer = nullptr;
            BlackSound::CNotificationPlayer m_notificationPlayer;

#ifdef Q_OS_MAC
            BlackMisc::CMacOSMicrophoneAccess m_micAccess;
#endif
            //! Init microphone
            void delayedInitMicrophone();
        };
    } // ns
} // ns

#endif // guard
