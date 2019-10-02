/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAUDIO_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTAUDIO_IMPL_H

#include "blackcore/context/contextaudio.h"
#include "blackcore/audio/audiosettings.h"
#include "blackcore/actionbind.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blackcore/afv/clients/afvclient.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/audio/ptt.h"
#include "blackmisc/input/actionhotkeydefs.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/macos/microphoneaccess.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/icons.h"
#include "blackmisc/network/connectionstatus.h"
#include "blacksound/selcalplayer.h"
#include "blacksound/notificationplayer.h"

#include <QHash>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <memory>

// clazy:excludeall=const-signal-or-slot

namespace BlackMisc
{
    class CDBusServer;
    namespace Audio    { class CAudioDeviceInfo; }
    namespace Aviation { class CCallsign; }
}

namespace BlackCore
{
    class CCoreFacade;

    namespace Context
    {
        //! Audio context implementation
        class BLACKCORE_EXPORT CContextAudio :
            public IContextAudio,
            public BlackMisc::CIdentifiable
        {
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)
            Q_OBJECT

            friend class BlackCore::CCoreFacade;
            friend class IContextAudio;

        public:
            //! Destructor
            virtual ~CContextAudio() override;

            //! Reference to voice client
            BlackCore::Afv::Clients::CAfvClient &voiceClient() { return m_voiceClient; }

        public slots:
            // Interface implementations
            //! \publicsection
            //! @{
            virtual BlackMisc::CIdentifier audioRunsWhere() const override;
            virtual BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const override;
            virtual BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const override;
            virtual void setCurrentAudioDevices(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice, const BlackMisc::Audio::CAudioDeviceInfo &outputDevice) override;
            virtual void setVoiceOutputVolume(int volume) override;
            virtual int  getVoiceOutputVolume() const override;
            virtual void setMute(bool muted) override;
            virtual bool isMuted() const override;
            virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) override;
            virtual void playNotification(BlackMisc::Audio::CNotificationSounds::NotificationFlag notification, bool considerSettings, int volume = -1) override;
            virtual void enableAudioLoopback(bool enable = true) override;
            virtual bool isAudioLoopbackEnabled() const override;
            virtual BlackMisc::Audio::CVoiceSetup getVoiceSetup() const override;
            virtual void setVoiceSetup(const BlackMisc::Audio::CVoiceSetup &setup) override;
            //! @}

            //! \addtogroup swiftdotcommands
            //! @{
            //! <pre>
            //! .mute                          mute             BlackCore::Context::CContextAudio
            //! .unmute                        unmute           BlackCore::Context::CContextAudio
            //! .vol .volume   volume 0..100   set volume       BlackCore::Context::CContextAudio
            //! </pre>
            //! @}
            //! \copydoc IContextAudio::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

        protected:
            //! Constructor
            CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextAudio *registerWithDBus(BlackMisc::CDBusServer *server);

        private:
            //! Enable/disable voice transmission, nornally used with hotkey @{
            void setVoiceTransmission(bool enable, BlackMisc::Audio::PTTCOM com);
            void setVoiceTransmissionCom1(bool enabled);
            void setVoiceTransmissionCom2(bool enabled);
            void setVoiceTransmissionComActive(bool enabled);
            //! @}

            //! Connection in transition
            bool inTransitionState() const;

            //! Change the device settings
            void changeDeviceSettings();

            //! Changed audio settings
            void onChangedAudioSettings();

            //! Audio increase/decrease volume @{
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

            CActionBind m_actionPtt     { BlackMisc::Input::pttHotkeyAction(),     BlackMisc::Input::pttHotkeyIcon(), this, &CContextAudio::setVoiceTransmissionComActive };
            CActionBind m_actionPttCom1 { BlackMisc::Input::pttCom1HotkeyAction(), BlackMisc::Input::pttHotkeyIcon(), this, &CContextAudio::setVoiceTransmissionCom1 };
            CActionBind m_actionPttCom2 { BlackMisc::Input::pttCom2HotkeyAction(), BlackMisc::Input::pttHotkeyIcon(), this, &CContextAudio::setVoiceTransmissionCom2 };
            CActionBind m_actionAudioVolumeIncrease { BlackMisc::Input::audioVolumeIncreaseHotkeyAction(), BlackMisc::Input::audioVolumeIncreaseHotkeyIcon(), this, &CContextAudio::audioIncreaseVolume };
            CActionBind m_actionAudioVolumeDecrease { BlackMisc::Input::audioVolumeDecreaseHotkeyAction(), BlackMisc::Input::audioVolumeDecreaseHotkeyIcon(), this, &CContextAudio::audioDecreaseVolume };

            int m_outVolumeBeforeMute = 90;
            static constexpr int MinUnmuteVolume = 20; //!< minimum volume when unmuted

            /**
            #ifdef Q_OS_MAC
                        BlackMisc::CMacOSMicrophoneAccess m_micAccess;
                        void delayedInitMicrophone();
            #endif
            **/

            BlackSound::CSelcalPlayer      *m_selcalPlayer = nullptr;
            BlackSound::CNotificationPlayer m_notificationPlayer;

            // settings
            BlackMisc::CSetting<BlackMisc::Audio::TSettings>     m_audioSettings       { this, &CContextAudio::onChangedAudioSettings };
            BlackMisc::CSetting<BlackCore::Audio::TInputDevice>  m_inputDeviceSetting  { this, &CContextAudio::changeDeviceSettings };
            BlackMisc::CSetting<BlackCore::Audio::TOutputDevice> m_outputDeviceSetting { this, &CContextAudio::changeDeviceSettings };

            // AFV
            Afv::Clients::CAfvClient m_voiceClient;
        };
    } // namespace
} // namespace

#endif // guard
