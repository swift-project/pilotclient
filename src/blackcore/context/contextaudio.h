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

#include "blackcore/audio/audiosettings.h"
#include "blackcore/context/context.h"
#include "blackcore/actionbind.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blackcore/afv/audio/receiversampleprovider.h"
#include "blacksound/selcalplayer.h"
#include "blacksound/notificationplayer.h"
#include "blackmisc/macos/microphoneaccess.h"
#include "blackmisc/audio/settings/voicesettings.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/ptt.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/input/actionhotkeydefs.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot


//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTAUDIO_INTERFACENAME "org.swift_project.blackcore.contextaudio"

//! DBus object path for context
#define BLACKCORE_CONTEXTAUDIO_OBJECTPATH "/audio"

//! @}

class QDBusConnection;

namespace BlackMisc { class CDBusServer; }
namespace BlackCore
{
    namespace Afv { namespace Clients { class CAfvClient; }}
    namespace Context
    {
        //! Audio context interface
        class BLACKCORE_EXPORT IContextAudio :
            public IContext,
            public BlackMisc::CIdentifiable
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

            //! \copydoc IContext::getPathAndContextId()
            virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextAudio *create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

            //! Destructor
            virtual ~IContextAudio() override;

            //! Graceful shutdown
            void gracefulShutdown();

            // -------- parts which can run in core and GUI, referring to local voice client ------------

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

            //! Is COM unit enabled?
            bool isEnabledComUnit(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const;

            //! Is COM unit transmitting?
            bool isTransmittingComUnit(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const;

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

            //! Audio started with
            void startedAudio(const BlackMisc::Audio::CAudioDeviceInfo &input, const BlackMisc::Audio::CAudioDeviceInfo &output);

            //! VU levels @{
            void inputVolumePeakVU(double value);
            void outputVolumePeakVU(double value);
            //! @}

            //! Callsigns I receive have changed
            void receivingCallsignsChanged(const BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs &args);

            //! Client updated from own aicraft data
            void updatedFromOwnAircraftCockpit();

            // -------- local settings, not DBus relayed -------

        public slots:
            // ------------- DBus ---------------

            //! \addtogroup swiftdotcommands
            //! <pre>
            //! .mute                          mute             BlackCore::Context::CContextAudio
            //! .unmute                        unmute           BlackCore::Context::CContextAudio
            //! .vol .volume   volume 0..100   set volume       BlackCore::Context::CContextAudio
            //! </pre>
            //! \copydoc IContext::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! Register a device on a machine (for core/GUI it will return all known devices on all machines)
            virtual void registerDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) = 0;

            //! Unregister devices
            virtual void unRegisterDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) = 0;

            //! All registered devices
            virtual BlackMisc::Audio::CAudioDeviceInfoList getRegisteredDevices() const = 0;

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

            //! Changed voice settings
            void onChangedVoiceSettings();

            //! Audio increase/decrease volume
            //! @{
            void audioIncreaseVolume(bool enabled);
            void audioDecreaseVolume(bool enabled);
            //! @}

            //! Get current COM unit from cockpit
            //! \remark cross context
            //! @{
            BlackMisc::Aviation::CComSystem xCtxGetOwnComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const;
            bool xCtxIsComIntegratedWithSimulator() const;
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
            BlackMisc::CSetting<BlackMisc::Audio::TSettings>             m_audioSettings { this, &IContextAudio::onChangedAudioSettings };
            BlackMisc::CSetting<BlackMisc::Audio::Settings::TVoiceSetup> m_voiceSettings { this, &IContextAudio::onChangedVoiceSettings };

            BlackMisc::CSetting<Audio::TInputDevice>  m_inputDeviceSetting  { this, &IContextAudio::changeDeviceSettings };
            BlackMisc::CSetting<Audio::TOutputDevice> m_outputDeviceSetting { this, &IContextAudio::changeDeviceSettings };

            // AFV
            Afv::Clients::CAfvClient *m_voiceClient = nullptr;

            // Players
            BlackSound::CSelcalPlayer      *m_selcalPlayer = nullptr;
            BlackSound::CNotificationPlayer m_notificationPlayer;
        };
    } // ns
} // ns

#endif // guard
