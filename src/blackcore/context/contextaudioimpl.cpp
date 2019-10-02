/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */


#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"  // for COM integration
#include "blackcore/context/contextsimulator.h"    // for COM intergration
#include "blackcore/application.h"
#include "blackcore/corefacade.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"

#include <QTimer>
#include <QtGlobal>
#include <QPointer>

#include <algorithm>
#include <stdbool.h>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Input;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackSound;
using namespace BlackCore::Vatsim;
using namespace BlackCore::Afv::Clients;

namespace BlackCore
{
    namespace Context
    {
        CContextAudio::CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextAudio(mode, runtime),
            CIdentifiable(this),
            m_voiceClient(("https://voice1.vatsim.uk"))
        {
            const CSettings as = m_audioSettings.getThreadLocal();
            this->setVoiceOutputVolume(as.getOutVolume());
            m_selcalPlayer = new CSelcalPlayer(CAudioDeviceInfo::getDefaultOutputDevice(), this);

            connect(&m_voiceClient, &CAfvClient::ptt, this, &CContextAudio::ptt);

            this->changeDeviceSettings();
            QPointer<CContextAudio> myself(this);
            QTimer::singleShot(5000, this, [ = ]
            {
                if (!myself) { return; }
                if (!sApp || sApp->isShuttingDown()) { return; }
                myself->onChangedAudioSettings();
            });
        }

        CContextAudio *CContextAudio::registerWithDBus(CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) { return this; }
            server->addObject(IContextAudio::ObjectPath(), this);
            return this;
        }

        CContextAudio::~CContextAudio()
        {
            m_voiceClient.stop();
        }

        CIdentifier CContextAudio::audioRunsWhere() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            static const CIdentifier i("CContextAudio");
            return i;
        }

        CAudioDeviceInfoList CContextAudio::getAudioDevices() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return CAudioDeviceInfoList::allDevices();
        }

        CAudioDeviceInfoList CContextAudio::getCurrentAudioDevices() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }

            // either the devices really used, or settings
            CAudioDeviceInfo inputDevice = m_voiceClient.getInputDevice();
            if (!inputDevice.isValid()) { inputDevice = CAudioDeviceInfo::getDefaultInputDevice(); }

            CAudioDeviceInfo outputDevice = m_voiceClient.getOutputDevice();
            if (!outputDevice.isValid()) { outputDevice = CAudioDeviceInfo::getDefaultOutputDevice(); }

            CAudioDeviceInfoList devices;
            devices.push_back(inputDevice);
            devices.push_back(outputDevice);
            return devices;
        }

        void CContextAudio::setCurrentAudioDevices(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << inputDevice << outputDevice; }
            if (!inputDevice.getName().isEmpty())  { m_inputDeviceSetting.setAndSave(inputDevice.getName()); }
            if (!outputDevice.getName().isEmpty()) { m_outputDeviceSetting.setAndSave(outputDevice.getName()); }
            const bool changed = m_voiceClient.restartWithNewDevices(inputDevice, outputDevice);
            if (changed)
            {
                emit this->changedSelectedAudioDevices(this->getCurrentAudioDevices());
            }
        }

        void CContextAudio::setVoiceOutputVolume(int volume)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << volume; }

            const bool wasMuted = this->isMuted();
            volume = CSettings::fixOutVolume(volume);

            const int currentVolume = m_voiceClient.getNormalizedOutputVolume();
            const bool changedVoiceOutput = (currentVolume != volume);
            if (changedVoiceOutput)
            {
                m_voiceClient.setNormalizedOutputVolume(volume);
                m_outVolumeBeforeMute = currentVolume;

                emit this->changedAudioVolume(volume);
                if ((volume > 0 && wasMuted) || (volume < 1 && !wasMuted))
                {
                    // inform about muted
                    emit this->changedMute(volume < 1);
                }
            }

            CSettings as(m_audioSettings.getThreadLocal());
            if (as.getOutVolume() != volume)
            {
                as.setOutVolume(volume);
                m_audioSettings.set(as);
            }
        }

        int CContextAudio::getVoiceOutputVolume() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_voiceClient.getNormalizedOutputVolume();
        }

        void CContextAudio::setMute(bool muted)
        {
            if (this->isMuted() == muted) { return; } // avoid roundtrips / unnecessary signals
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << muted; }

            if (m_voiceClient.isMuted() == muted) { return; }
            m_voiceClient.setMuted(muted);

            // signal
            emit this->changedMute(muted);
        }

        bool CContextAudio::isMuted() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_voiceClient.isMuted();
        }

        void CContextAudio::playSelcalTone(const CSelcal &selcal)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << selcal; }
            const CTime t = m_selcalPlayer->play(90, selcal);
            const int ms = t.toMs();
            if (ms > 10)
            {
                // As of https://dev.swift-project.org/T558 play additional notification
                const QPointer<const CContextAudio> myself(this);
                QTimer::singleShot(ms, this, [ = ]
                {
                    if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                    this->playNotification(CNotificationSounds::NotificationTextMessageSupervisor, true);
                });
            }
        }

        void CContextAudio::playNotification(CNotificationSounds::NotificationFlag notification, bool considerSettings, int volume)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << notification; }

            const CSettings settings = m_audioSettings.getThreadLocal();
            const bool play = !considerSettings || settings.isNotificationFlagSet(notification);
            if (!play) { return; }
            if (notification == CNotificationSounds::PTTClickKeyDown && (considerSettings && settings.noAudioTransmission()))
            {
                /**
                if (!this->canTalk())
                {
                    // warning sound
                    notification = CNotificationSounds::NotificationNoAudioTransmission;
                }
                **/
            }

            if (volume < 0 || volume > 100)
            {
                volume = 90;
                if (considerSettings) { volume = qMax(25, settings.getNotificationVolume()); }
            }
            m_notificationPlayer.play(notification, volume);
        }

        void CContextAudio::enableAudioLoopback(bool enable)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            m_voiceClient.setLoopBack(enable);
        }

        bool CContextAudio::isAudioLoopbackEnabled() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_voiceClient.isLoopback();
        }

        void CContextAudio::setVoiceSetup(const CVoiceSetup &setup)
        {
            // could be recycled for some AFV setup
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            Q_UNUSED(setup)
        }

        CVoiceSetup CContextAudio::getVoiceSetup() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return CVoiceSetup();
        }

        bool CContextAudio::parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator)
        {
            Q_UNUSED(originator)
            if (commandLine.isEmpty()) { return false; }
            CSimpleCommandParser parser(
            {
                ".vol", ".volume",    // output volume
                ".mute",              // mute
                ".unmute"             // unmute
            });
            parser.parse(commandLine);
            if (!parser.isKnownCommand()) { return false; }

            if (parser.matchesCommand(".mute"))
            {
                this->setMute(true);
                return true;
            }
            else if (parser.matchesCommand(".unmute"))
            {
                this->setMute(false);
                return true;
            }
            else if (parser.commandStartsWith("vol") && parser.countParts() > 1)
            {
                int v = parser.toInt(1);
                this->setVoiceOutputVolume(v);
            }
            return false;
        }

        void CContextAudio::setVoiceTransmission(bool enable, PTTCOM com)
        {
            m_voiceClient.setPttForCom(enable, com);
        }

        void CContextAudio::setVoiceTransmissionCom1(bool enabled)
        {
            this->setVoiceTransmission(enabled, COM1);
        }

        void CContextAudio::setVoiceTransmissionCom2(bool enabled)
        {
            this->setVoiceTransmission(enabled, COM2);
        }

        void CContextAudio::setVoiceTransmissionComActive(bool enabled)
        {
            this->setVoiceTransmission(enabled, COMActive);
        }

        void CContextAudio::changeDeviceSettings()
        {
            const QString inputDeviceName = m_inputDeviceSetting.get();
            CAudioDeviceInfo input;
            if (!inputDeviceName.isEmpty())
            {
                const CAudioDeviceInfoList inputDevs = this->getAudioInputDevices();
                input = inputDevs.findByName(inputDeviceName);
            }

            const QString outputDeviceName = m_outputDeviceSetting.get();
            CAudioDeviceInfo output;
            if (!outputDeviceName.isEmpty())
            {
                const CAudioDeviceInfoList outputDevs = this->getAudioOutputDevices();
                output = outputDevs.findByName(outputDeviceName);
            }

            this->setCurrentAudioDevices(input, output);
        }

        void CContextAudio::onChangedAudioSettings()
        {
            const CSettings s = m_audioSettings.get();
            const QString dir = s.getNotificationSoundDirectory();
            m_notificationPlayer.updateDirectory(dir);
            this->setVoiceOutputVolume(s.getOutVolume());
        }

        void CContextAudio::audioIncreaseVolume(bool enabled)
        {
            if (!enabled) { return; }
            const int v = qRound(this->getVoiceOutputVolume() * 1.2);
            this->setVoiceOutputVolume(v);
        }

        void CContextAudio::audioDecreaseVolume(bool enabled)
        {
            if (!enabled) { return; }
            const int v = qRound(this->getVoiceOutputVolume() / 1.2);
            this->setVoiceOutputVolume(v);
        }

        CComSystem CContextAudio::getOwnComSystem(CComSystem::ComUnit unit) const
        {
            if (!this->getIContextOwnAircraft())
            {
                // context not available
                const double defFreq = 122.8;
                switch (unit)
                {
                case CComSystem::Com1: return CComSystem::getCom1System(defFreq, defFreq);
                case CComSystem::Com2: return CComSystem::getCom2System(defFreq, defFreq);
                default: break;
                }
                return CComSystem::getCom1System(defFreq, defFreq);
            }
            return this->getIContextOwnAircraft()->getOwnComSystem(unit);
        }

        bool CContextAudio::isComIntegratedWithSimulator() const
        {
            if (!this->getIContextSimulator()) { return false; }
            return this->getIContextSimulator()->getSimulatorSettings().isComIntegrated();
        }

        void CContextAudio::xCtxChangedAircraftCockpit(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            Q_UNUSED(aircraft)
            Q_UNUSED(originator)

            /**
            if (CIdentifiable::isMyIdentifier(originator)) { return; }
            const bool integrated = this->isComIntegratedWithSimulator();
            if (integrated)
            {
                // set as in cockpit
                const bool com1Rec = aircraft.getCom1System().isReceiveEnabled();
                const bool com2Rec = aircraft.getCom2System().isReceiveEnabled();
            }
            **/
        }

        void CContextAudio::xCtxNetworkConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
        {
            Q_UNUSED(from)
            BLACK_VERIFY_X(this->getIContextNetwork(), Q_FUNC_INFO, "Missing network context");
            if (to.isConnected() && this->getIContextNetwork())
            {
                const CUser connectedUser = this->getIContextNetwork()->getConnectedServer().getUser();
                m_voiceClient.connectTo(connectedUser.getId(), connectedUser.getPassword(), connectedUser.getCallsign().asString());
                m_voiceClient.start(CAudioDeviceInfo::getDefaultInputDevice(), CAudioDeviceInfo::getDefaultOutputDevice(), {0, 1});
            }
            else if (to.isDisconnected())
            {
                m_voiceClient.stop();
                m_voiceClient.disconnectFrom();
            }
        }

        /**
        #ifdef Q_OS_MAC
                void CContextAudio::delayedInitMicrophone()
                {
                    m_voiceInputDevice = m_voice->createInputDevice();
                    m_voice->connectVoice(m_voiceInputDevice.get(), m_audioMixer.get(), IAudioMixer::InputMicrophone);
                    CLogMessage(this).info(u"MacOS delayed input device init");
                }
        #endif
        **/

    } // namespace
} // namespace
