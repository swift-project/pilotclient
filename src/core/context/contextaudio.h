// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTAUDIO_H
#define SWIFT_CORE_CONTEXT_CONTEXTAUDIO_H

#include <QCommandLineOption>
#include <QString>

#include "core/actionbind.h"
#include "core/afv/audio/receiversampleprovider.h"
#include "core/audio/audiosettings.h"
#include "core/context/context.h"
#include "core/corefacade.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/audio/audiodeviceinfolist.h"
#include "misc/audio/audiosettings.h"
#include "misc/audio/notificationsounds.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/selcal.h"
#include "misc/genericdbusinterface.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/input/actionhotkeydefs.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/userlist.h"
#include "misc/simplecommandparser.h"
#include "sound/notificationplayer.h"
#include "sound/selcalplayer.h"

// clazy:excludeall=const-signal-or-slot

//! \ingroup dbus
//! DBus interface for context
#define SWIFT_CORE_CONTEXTAUDIO_INTERFACENAME "org.swift_project.swift_core.contextaudio"

//! \ingroup dbus
//! DBus object path for context
#define SWIFT_CORE_CONTEXTAUDIO_OBJECTPATH "/audio"

class QDBusConnection;

namespace swift::misc
{
    class CDBusServer;
}
namespace swift::core
{
    namespace afv::clients
    {
        class CAfvClient;
    }
    namespace context
    {
        //! Audio context interface
        class SWIFT_CORE_EXPORT IContextAudio : public IContext
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTAUDIO_INTERFACENAME)

            friend class swift::core::CCoreFacade;

        public:
            //! Interface name
            static const QString &InterfaceName();

            //! Object path
            static const QString &ObjectPath();

            //! \copydoc IContext::getPathAndContextId()
            QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextAudio *create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode,
                                         swift::misc::CDBusServer *server, QDBusConnection &connection);

            // ------------- only use DBus signals here -------------
        signals:
            //! Authentication failed, ....
            void voiceClientFailure(const swift::misc::CStatusMessage &msg);

        public slots:
            // ------------- DBus ---------------

            //! All registered devices
            virtual swift::misc::audio::CAudioDeviceInfoList getRegisteredDevices() const = 0;

            //! Register a device on a machine (for core/GUI it will return all known devices on all machines)
            virtual void registerDevices(const swift::misc::audio::CAudioDeviceInfoList &devices) = 0;

            //! Unregister devices
            virtual void unRegisterDevices(const swift::misc::audio::CAudioDeviceInfoList &devices) = 0;

            //! Remove all devices for identifier (i.e. "a machine")
            virtual void unRegisterDevicesFor(const swift::misc::CIdentifier &identifier) = 0;

            //! Register an audio callsign (used with AFV)
            //! \remarks normally called with login
            virtual void registerAudioCallsign(const swift::misc::aviation::CCallsign &callsign,
                                               const swift::misc::CIdentifier &identifier) = 0;

            //! Un-register an audio callsign (used with AFV)
            //! \remarks normally called with logoff
            virtual void unRegisterAudioCallsign(const swift::misc::aviation::CCallsign &callsign,
                                                 const swift::misc::CIdentifier &identifier) = 0;

            //! Un-register an audio callsign (used with AFV)
            //! \remarks normally called with logoff
            virtual bool hasRegisteredAudioCallsign(const swift::misc::aviation::CCallsign &callsign) const = 0;

            // ------------- DBus ---------------

        protected:
            //! Constructor
            IContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Devices have been changed
            void onChangedLocalDevices(const swift::misc::audio::CAudioDeviceInfoList &devices);
        };

        //! Audio context base class
        class SWIFT_CORE_EXPORT CContextAudioBase : public IContextAudio, public swift::misc::CIdentifiable
        {
            Q_OBJECT
            friend class swift::core::CCoreFacade;

        protected:
            //! Constructor
            CContextAudioBase(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        public:
            //! Destructor
            ~CContextAudioBase() override;

            //! @{
            //! Copy operations
            CContextAudioBase(const CContextAudioBase &) = delete;
            CContextAudioBase &operator=(const CContextAudioBase &) = delete;
            //! @}

            //! @{
            //! Move operations
            CContextAudioBase(CContextAudioBase &&) = delete;
            CContextAudioBase &operator=(CContextAudioBase &&) = delete;
            //! @}

            //! Graceful shutdown
            void gracefulShutdown();

            //! @{
            //! Receive/transmit
            void setRxTx(bool rx1, bool tx1, bool rx2, bool tx2);
            void getRxTx(bool &rx1, bool &tx1, bool &rx2, bool &tx2) const;
            //! @}

            // -------- parts which can run in core and GUI, referring to local voice client ------------

            //! @{
            //! Audio devices
            swift::misc::audio::CAudioDeviceInfoList getAudioDevices() const;
            swift::misc::audio::CAudioDeviceInfoList getAudioInputDevices() const;
            swift::misc::audio::CAudioDeviceInfoList getAudioOutputDevices() const;
            //! @}

            //! Get current audio device
            //! \return input and output devices
            swift::misc::audio::CAudioDeviceInfoList getCurrentAudioDevices() const;

            //! Set current audio devices
            void setCurrentAudioDevices(const swift::misc::audio::CAudioDeviceInfo &inputDevice,
                                        const swift::misc::audio::CAudioDeviceInfo &outputDevice);

            //! @{
            //! Volume
            void setMasterOutputVolume(int volume);
            void setComOutputVolume(swift::misc::aviation::CComSystem::ComUnit comUnit, int volume);
            int getMasterOutputVolume() const;
            int getComOutputVolume(swift::misc::aviation::CComSystem::ComUnit comUnit) const;
            void setOutputMute(bool muted);
            bool isOutputMuted() const;
            //! @}

            //! SELCAL
            void playSelcalTone(const swift::misc::aviation::CSelcal &selcal);

            //! Notification sounds
            void playNotification(swift::misc::audio::CNotificationSounds::NotificationFlag notification,
                                  bool considerSettings, int volume = -1);

            //! @{
            //! Loopback
            void enableAudioLoopback(bool enable = true);
            bool isAudioLoopbackEnabled() const;
            //! @}

            //! Info string about audio
            QString audioRunsWhereInfo() const;

            //! Audio runs where
            const swift::misc::CIdentifier &audioRunsWhere() const;

            //! Is COM unit enabled?
            bool isEnabledComUnit(swift::misc::aviation::CComSystem::ComUnit comUnit) const;

            //! Is COM unit transmitting?
            bool isTransmittingComUnit(swift::misc::aviation::CComSystem::ComUnit comUnit) const;

            //! Connect to audio with network credentials
            //! \remark if there is no network connection/credential this just returns
            bool connectAudioWithNetworkCredentials();

            //! Is audio connected?
            bool isAudioConnected() const;

            //! Is audio started?
            bool isAudioStarted() const;

            //! Is audio enabled?
            bool isAudioEnabled() const { return m_voiceClient != nullptr; }

            //! Integrated with COM unit?
            bool isComUnitIntegrated() const;

            //! \todo WORKAROUND to hide the "local signals"
            afv::clients::CAfvClient *afvClient() const { return m_voiceClient; }

            //! Cmd.line arguments
            static const QList<QCommandLineOption> &getCmdLineOptions();

            //! No audio?
            static bool isNoAudioSet();

            //! Register the commands
            static void registerHelp()
            {
                if (swift::misc::CSimpleCommandParser::registered("swift::core::context::CContextAudioBase"))
                {
                    return;
                }
                swift::misc::CSimpleCommandParser::registerCommand({ ".mute", "mute audio" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".unmute", "unmute audio" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".vol volume", "volume 0..100" });
            }

            // -------- parts which can run in core and GUI, referring to local voice client ------------

        public slots:
            // ------------- DBus ---------------

            //! \cond
            //! Parse command line
            //! \ingroup swiftdotcommands
            //! <pre>
            //! .mute                          mute             swift::core::context::CContextAudioBase
            //! .unmute                        unmute           swift::core::context::CContextAudioBase
            //! .vol .volume   volume 0..100   set volume       swift::core::context::CContextAudioBase
            //! </pre>
            bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override;
            //! \endcond

            // ------------- DBus ---------------

            // ------------ local signals -------
        signals:
            //! Audio volume changed
            //! \sa setVoiceOutputVolume
            void changedAudioVolume(int volume);

            //! Output mute changed
            void changedOutputMute(bool muted);

            //! Changed audio devices (e.g. device enabled/disable)
            void changedLocalAudioDevices(const swift::misc::audio::CAudioDeviceInfoList &devices);

            //! Audio started with devices
            void startedAudio(const swift::misc::audio::CAudioDeviceInfo &input,
                              const swift::misc::audio::CAudioDeviceInfo &output);

            //! Audio stopped
            void stoppedAudio();

            //! PTT in voice client received
            void ptt(bool active, const swift::misc::CIdentifier &identifier);

            // ------------ local signals -------

        private:
            //! Init the voice client
            void initVoiceClient();

            //! Terminate the voice client
            void terminateVoiceClient();

            //! Enable/disable voice transmission, normally used with hotkey
            void setVoiceTransmission(bool enable);

            //! Change the device settings
            void changeDeviceSettings();

            //! Changed audio settings
            void onChangedAudioSettings();

            //! @{
            //! Audio increase/decrease volume
            void audioIncreaseVolume(bool enabled);
            void audioDecreaseVolume(bool enabled);
            void audioIncreaseVolumeCom1(bool enabled);
            void audioDecreaseVolumeCom1(bool enabled);
            void audioIncreaseVolumeCom2(bool enabled);
            void audioDecreaseVolumeCom2(bool enabled);
            //! @}

            //! Network connection status
            void xCtxNetworkConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                                    const swift::misc::network::CConnectionStatus &to);

            //! AFV client connection status changed
            void onAfvConnectionStatusChanged(int status);

            //! AFV client authentication failed
            void onAfvConnectionFailure(const swift::misc::CStatusMessage &msg);

            CActionBind m_actionPtt { swift::misc::input::pttHotkeyAction(), swift::misc::input::pttHotkeyIcon(), this,
                                      &CContextAudioBase::setVoiceTransmission };
            CActionBind m_actionAudioVolumeIncrease { swift::misc::input::audioVolumeIncreaseHotkeyAction(),
                                                      swift::misc::input::audioVolumeIncreaseHotkeyIcon(), this,
                                                      &CContextAudioBase::audioIncreaseVolume };
            CActionBind m_actionAudioVolumeDecrease { swift::misc::input::audioVolumeDecreaseHotkeyAction(),
                                                      swift::misc::input::audioVolumeDecreaseHotkeyIcon(), this,
                                                      &CContextAudioBase::audioDecreaseVolume };
            CActionBind m_actionAudioVolumeIncreaseCom1 { swift::misc::input::audioVolumeIncreaseCom1HotkeyAction(),
                                                          swift::misc::input::audioVolumeIncreaseHotkeyIcon(), this,
                                                          &CContextAudioBase::audioIncreaseVolumeCom1 };
            CActionBind m_actionAudioVolumeDecreaseCom1 { swift::misc::input::audioVolumeDecreaseCom1HotkeyAction(),
                                                          swift::misc::input::audioVolumeDecreaseHotkeyIcon(), this,
                                                          &CContextAudioBase::audioDecreaseVolumeCom1 };
            CActionBind m_actionAudioVolumeIncreaseCom2 { swift::misc::input::audioVolumeIncreaseCom2HotkeyAction(),
                                                          swift::misc::input::audioVolumeIncreaseHotkeyIcon(), this,
                                                          &CContextAudioBase::audioIncreaseVolumeCom2 };
            CActionBind m_actionAudioVolumeDecreaseCom2 { swift::misc::input::audioVolumeDecreaseCom2HotkeyAction(),
                                                          swift::misc::input::audioVolumeDecreaseHotkeyIcon(), this,
                                                          &CContextAudioBase::audioDecreaseVolumeCom2 };

            int m_outMasterVolumeBeforeMute = 50;
            static constexpr int MinUnmuteVolume = 20; //!< minimum volume when unmuted

            //! Do we use a local core
            static bool isRunningWithLocalCore();

            // settings
            swift::misc::CSetting<swift::misc::audio::TSettings> m_audioSettings {
                this, &CContextAudioBase::onChangedAudioSettings
            };

            swift::misc::CSetting<audio::TInputDevice> m_inputDeviceSetting {
                this, &CContextAudioBase::changeDeviceSettings
            };
            swift::misc::CSetting<audio::TOutputDevice> m_outputDeviceSetting {
                this, &CContextAudioBase::changeDeviceSettings
            };

            // AFV
            afv::clients::CAfvClient *m_voiceClient = nullptr;
            bool m_winCoInitialized = false;
            swift::misc::audio::CAudioDeviceInfoList m_activeLocalDevices;

            // Players
            swift::sound::CSelcalPlayer *m_selcalPlayer = nullptr;
            swift::sound::CNotificationPlayer m_notificationPlayer;
        };
    } // namespace context
} // namespace swift::core

#endif // SWIFT_CORE_CONTEXT_CONTEXTAUDIO_H
