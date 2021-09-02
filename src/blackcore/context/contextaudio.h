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
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"

#include <QObject>
#include <QString>
#include <QCommandLineOption>

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
        class BLACKCORE_EXPORT IContextAudio : public IContext
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)

            friend class BlackCore::CCoreFacade;

        public:
            //! Interface name
            static const QString &InterfaceName();

            //! Object path
            static const QString &ObjectPath();

            //! \copydoc IContext::getPathAndContextId()
            virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextAudio *create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

            // ------------- only use DBus signals here -------------
        signals:
            //! Authentication failed, ....
            void voiceClientFailure(const BlackMisc::CStatusMessage &msg);

        public slots:
            // ------------- DBus ---------------

            //! All registered devices
            virtual BlackMisc::Audio::CAudioDeviceInfoList getRegisteredDevices() const = 0;

            //! Register a device on a machine (for core/GUI it will return all known devices on all machines)
            virtual void registerDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) = 0;

            //! Unregister devices
            virtual void unRegisterDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) = 0;

            //! Remove all devices for identifer (i.e. "a machine")
            virtual void unRegisterDevicesFor(const BlackMisc::CIdentifier &identifier) = 0;

            //! Register an audio callsign (used with AFV)
            //! \remarks normally called with login
            virtual void registerAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign,  const BlackMisc::CIdentifier &identifier) = 0;

            //! Un-register an audio callsign (used with AFV)
            //! \remarks normally called with logoff
            virtual void unRegisterAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign,  const BlackMisc::CIdentifier &identifier) = 0;

            //! Un-register an audio callsign (used with AFV)
            //! \remarks normally called with logoff
            virtual bool hasRegisteredAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            // ------------- DBus ---------------

        protected:
            //! Constructor
            IContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Devices have been changed
            void onChangedLocalDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices);
        };

        //! Audio context interface
        class BLACKCORE_EXPORT CContextAudioBase :
            public IContextAudio,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT
            friend class BlackCore::CCoreFacade;

        protected:
            //! Constructor
            CContextAudioBase(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        public:
            //! Destructor
            virtual ~CContextAudioBase() override;

            //! Graceful shutdown
            void gracefulShutdown();

            //! Enable/disable
            //! @{
            void enableVoiceClient()  { this->initVoiceClient(); }
            void enableVoiceClientAndStart();
            void disableVoiceClient() { this->terminateVoiceClient(); }
            //! @}

            //! Receive/transmit
            //! @{
            void setRxTx(bool  rx1, bool  tx1, bool  rx2, bool  tx2);
            void getRxTx(bool &rx1, bool &tx1, bool &rx2, bool &tx2) const;
            //! @}

            // -------- parts which can run in core and GUI, referring to local voice client ------------

            //! Audio devices
            //! @{
            BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const;
            BlackMisc::Audio::CAudioDeviceInfoList getAudioInputDevices()  const;
            BlackMisc::Audio::CAudioDeviceInfoList getAudioOutputDevices() const;
            BlackMisc::Audio::CAudioDeviceInfoList getAudioDevicesPlusDefault() const;
            BlackMisc::Audio::CAudioDeviceInfoList getAudioInputDevicesPlusDefault()  const;
            BlackMisc::Audio::CAudioDeviceInfoList getAudioOutputDevicesPlusDefault() const;
            //! @}

            //! Get current audio device
            //! \return input and output devices
            BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const;

            //! Set current audio devices
            void setCurrentAudioDevices(const BlackMisc::Audio::CAudioDeviceInfo &inputDevice, const BlackMisc::Audio::CAudioDeviceInfo &outputDevice);

            //! Volume
            //! @{
            void setVoiceOutputVolume(BlackMisc::Aviation::CComSystem::ComUnit comUnit, int volume);
            int  getVoiceOutputVolume(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const;
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

            //! Connect to audio with network credentials
            //! \remark if there is no network connection/credential this just returns
            bool connectAudioWithNetworkCredentials();

            //! Is audio connected?
            bool isAudioConnected() const;

            //! Is audio started?
            bool isAudioStarted() const;

            //! Is audio enabled?
            bool isAudioEnabled() const { return m_voiceClient; }

            //! Integrated with COM unit?
            bool isComUnitIntegrated() const;

            //! \todo WORKAROUND to hide the "local signals"
            Afv::Clients::CAfvClient *afvClient() const { return m_voiceClient; }

            //! Cmd.line arguments
            static const QList<QCommandLineOption> &getCmdLineOptions();

            //! No audio?
            static bool isNoAudioSet();

            //! Register the commands
            static void registerHelp()
            {
                if (BlackMisc::CSimpleCommandParser::registered("BlackCore::Context::CContextAudioBase")) { return; }
                BlackMisc::CSimpleCommandParser::registerCommand({".mute", "mute audio"});
                BlackMisc::CSimpleCommandParser::registerCommand({".unmute", "unmute audio"});
                BlackMisc::CSimpleCommandParser::registerCommand({".vol volume", "volume 0..100"});
                BlackMisc::CSimpleCommandParser::registerCommand({".aliased on|off", "aliased HF frequencies"});
            }

            // -------- parts which can run in core and GUI, referring to local voice client ------------

        public slots:
            // ------------- DBus ---------------

            //! \cond
            //! Parse command line
            //! \addtogroup swiftdotcommands
            //! <pre>
            //! .mute                          mute             BlackCore::Context::CContextAudioBase
            //! .unmute                        unmute           BlackCore::Context::CContextAudioBase
            //! .vol .volume   volume 0..100   set volume       BlackCore::Context::CContextAudioBase
            //! .aliased on|off                aliased stations BlackCore::Context::CContextAudioBase
            //! </pre>
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;
            //! \endcond

            // ------------- DBus ---------------

            // ------------ local signals -------
        signals:
            //! Audio volume changed
            //! \sa setVoiceOutputVolume
            void changedAudioVolume(int volume);

            //! Mute changed
            void changedMute(bool muted);

            //! Changed audio devices (e.g. device enabled/disable)
            void changedLocalAudioDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Audio started with devices
            void startedAudio(const BlackMisc::Audio::CAudioDeviceInfo &input, const BlackMisc::Audio::CAudioDeviceInfo &output);

            //! Audio stopped
            void stoppedAudio();

            //! PTT in voice client received
            void ptt(bool active, BlackMisc::Audio::PTTCOM pttcom, const BlackMisc::CIdentifier &identifier);

            /*
             * Workaround those must be invisible for DBus
             *

            //! VU levels
            //! @{
            void inputVolumePeakVU (double value);
            void outputVolumePeakVU(double value);
            //! @}

            //! Callsigns I receive have changed
            void receivingCallsignsChanged(const BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs &args);

            //! Client updated from own aicraft data
            void updatedFromOwnAircraftCockpit();

            * end workaround */

            // ------------ local signals -------

        private:
            //! Init the voice client
            void initVoiceClient();

            //! Terminate the voice client
            void terminateVoiceClient();

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

            //! Network connection status
            void xCtxNetworkConnectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

            //! AFV client connection status changed
            void onAfvConnectionStatusChanged(int status);

            //! AFV client authentication failed
            void onAfvConnectionFailure(const BlackMisc::CStatusMessage &msg);

            CActionBind m_actionPtt     { BlackMisc::Input::pttHotkeyAction(),     BlackMisc::Input::pttHotkeyIcon(), this, &CContextAudioBase::setVoiceTransmissionComActive };
            CActionBind m_actionPttCom1 { BlackMisc::Input::pttCom1HotkeyAction(), BlackMisc::Input::pttHotkeyIcon(), this, &CContextAudioBase::setVoiceTransmissionCom1 };
            CActionBind m_actionPttCom2 { BlackMisc::Input::pttCom2HotkeyAction(), BlackMisc::Input::pttHotkeyIcon(), this, &CContextAudioBase::setVoiceTransmissionCom2 };
            CActionBind m_actionAudioVolumeIncrease { BlackMisc::Input::audioVolumeIncreaseHotkeyAction(), BlackMisc::Input::audioVolumeIncreaseHotkeyIcon(), this, &CContextAudioBase::audioIncreaseVolume };
            CActionBind m_actionAudioVolumeDecrease { BlackMisc::Input::audioVolumeDecreaseHotkeyAction(), BlackMisc::Input::audioVolumeDecreaseHotkeyIcon(), this, &CContextAudioBase::audioDecreaseVolume };

            int m_outVolumeBeforeMuteCom1 = 90;
            int m_outVolumeBeforeMuteCom2 = 90;
            static constexpr int MinUnmuteVolume = 20; //!< minimum volume when unmuted

            //! Do we use a local core
            static bool isRunningWithLocalCore();

            // settings
            BlackMisc::CSetting<BlackMisc::Audio::TSettings>   m_audioSettings { this, &CContextAudioBase::onChangedAudioSettings };
            BlackMisc::CSetting<BlackMisc::Audio::TVoiceSetup> m_voiceSettings { this, &CContextAudioBase::onChangedVoiceSettings };

            BlackMisc::CSetting<Audio::TInputDevice>  m_inputDeviceSetting  { this, &CContextAudioBase::changeDeviceSettings };
            BlackMisc::CSetting<Audio::TOutputDevice> m_outputDeviceSetting { this, &CContextAudioBase::changeDeviceSettings };

            // AFV
            Afv::Clients::CAfvClient *m_voiceClient = nullptr;
            bool m_winCoInitialized = false;
            BlackMisc::Audio::CAudioDeviceInfoList m_activeLocalDevices;

            // Players
            BlackSound::CSelcalPlayer      *m_selcalPlayer = nullptr;
            BlackSound::CNotificationPlayer m_notificationPlayer;
        };
    } // ns
} // ns

#endif // guard
