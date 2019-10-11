/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "contextaudio.h"

#include "blackcore/afv/clients/afvclient.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"      // for user login
#include "blackcore/context/contextownaircraft.h"  // for COM integration
#include "blackcore/context/contextsimulator.h"    // for COM intergration
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextaudioproxy.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/verify.h"
#include "blackmisc/icons.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackSound;
using namespace BlackCore::Afv::Clients;

namespace BlackCore
{
    namespace Context
    {
        IContextAudio::IContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContext(mode, runtime),
            CIdentifiable(this),
            m_voiceClient(new CAfvClient(CVoiceSetup().getAfvVoiceServerUrl(), this))
        {
            const CVoiceSetup vs = m_voiceSettings.getThreadLocal();
            m_voiceClient->updateVoiceServerUrl(vs.getAfvVoiceServerUrl());

            Q_ASSERT_X(CThreadUtils::isApplicationThread(m_voiceClient->thread()), Q_FUNC_INFO, "Should be in main thread");
            m_voiceClient->start();
            Q_ASSERT_X(m_voiceClient->owner() == this, Q_FUNC_INFO, "Wrong owner");
            Q_ASSERT_X(!CThreadUtils::isApplicationThread(m_voiceClient->thread()), Q_FUNC_INFO, "Must NOT be in main thread");

            connect(m_voiceClient, &CAfvClient::outputVolumePeakVU,               this, &IContextAudio::outputVolumePeakVU);
            connect(m_voiceClient, &CAfvClient::inputVolumePeakVU,                this, &IContextAudio::inputVolumePeakVU);
            connect(m_voiceClient, &CAfvClient::receivingCallsignsChanged,        this, &IContextAudio::receivingCallsignsChanged);
            connect(m_voiceClient, &CAfvClient::updatedFromOwnAircraftCockpit,    this, &IContextAudio::updatedFromOwnAircraftCockpit);
            connect(m_voiceClient, &CAfvClient::ptt,                              this, &IContextAudio::ptt);

            const CSettings as = m_audioSettings.getThreadLocal();
            this->setVoiceOutputVolume(as.getOutVolume());
            m_selcalPlayer = new CSelcalPlayer(CAudioDeviceInfo::getDefaultOutputDevice(), this);

            this->changeDeviceSettings();
            QPointer<IContextAudio> myself(this);
            QTimer::singleShot(5000, this, [ = ]
            {
                if (!myself || !sApp || sApp->isShuttingDown()) { return; }
                myself->onChangedAudioSettings();
            });
        }

        const QString &IContextAudio::InterfaceName()
        {
            static const QString s(BLACKCORE_CONTEXTAUDIO_INTERFACENAME);
            return s;
        }

        const QString &IContextAudio::ObjectPath()
        {
            static const QString s(BLACKCORE_CONTEXTAUDIO_OBJECTPATH);
            return s;
        }

        IContextAudio *IContextAudio::create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection)
        {
            // for audio no empty context is available
            // since IContextAudio provides audio on either side (core/GUI) we do not use ContextAudioEmpty
            // ContextAudioEmpty would cause issue, as it is initializing "common parts" during shutdown
            switch (mode)
            {
            case CCoreFacadeConfig::Local:
            case CCoreFacadeConfig::LocalInDBusServer:
            default:
                return (new CContextAudio(mode, runtime))->registerWithDBus(server);
            case CCoreFacadeConfig::Remote:
                return new CContextAudioProxy(CDBusServer::coreServiceName(connection), connection, mode, runtime);
            case CCoreFacadeConfig::NotUsed:
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Empty context not supported for audio (since AFV)");
                return nullptr;
            }
        }

        IContextAudio::~IContextAudio()
        {
            gracefulShutdown();
        }

        void IContextAudio::gracefulShutdown()
        {
            if (m_voiceClient)
            {
                m_voiceClient->stopAudio();
                m_voiceClient->quitAndWait();
                Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(m_voiceClient), Q_FUNC_INFO, "Needs to be back in current thread");
                m_voiceClient = nullptr;
            }
            QObject::disconnect(this);
        }

        const CIdentifier &IContextAudio::audioRunsWhere() const
        {
            static const CIdentifier i("IContextAudio");
            return i;
        }

        bool IContextAudio::isEnabledComUnit(CComSystem::ComUnit comUnit) const
        {
            return m_voiceClient->isEnabledComUnit(comUnit);
        }

        bool IContextAudio::isTransmittingComUnit(CComSystem::ComUnit comUnit) const
        {
            return m_voiceClient->isTransmittingdComUnit(comUnit);
        }

        QString IContextAudio::audioRunsWhereInfo() const
        {
            static const QString s = QStringLiteral("Audio on '%1', '%2'.").arg(audioRunsWhere().getMachineName(), audioRunsWhere().getProcessName());
            return s;
        }

        bool IContextAudio::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
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

        CAudioDeviceInfoList IContextAudio::getAudioDevices() const
        {
            return CAudioDeviceInfoList::allDevices();
        }

        CAudioDeviceInfoList IContextAudio::getCurrentAudioDevices() const
        {
            const QString inputDeviceName = m_inputDeviceSetting.get();
            const CAudioDeviceInfo inputDevice = this->getAudioInputDevices().findByNameOrDefault(inputDeviceName, CAudioDeviceInfo::getDefaultInputDevice());

            const QString outputDeviceName = m_outputDeviceSetting.get();
            const CAudioDeviceInfo outputDevice = this->getAudioOutputDevices().findByNameOrDefault(outputDeviceName, CAudioDeviceInfo::getDefaultOutputDevice());

            CAudioDeviceInfoList devices;
            devices.push_back(inputDevice);
            devices.push_back(outputDevice);
            return devices;
        }

        void IContextAudio::setCurrentAudioDevices(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
        {
            if (!m_voiceClient) { return; }
            if (!inputDevice.getName().isEmpty())  { m_inputDeviceSetting.setAndSave(inputDevice.getName()); }
            if (!outputDevice.getName().isEmpty()) { m_outputDeviceSetting.setAndSave(outputDevice.getName()); }

            m_voiceClient->restartWithNewDevices(inputDevice, outputDevice);
            emit this->changedSelectedAudioDevices(this->getCurrentAudioDevices());
        }

        void IContextAudio::setVoiceOutputVolume(int volume)
        {
            if (!m_voiceClient) { return; }

            const bool wasMuted = this->isMuted();
            volume = CSettings::fixOutVolume(volume);

            const int  currentVolume = m_voiceClient->getNormalizedOutputVolume();
            const bool changedVoiceOutput = (currentVolume != volume);
            if (changedVoiceOutput)
            {
                m_voiceClient->setNormalizedOutputVolume(volume);
                m_outVolumeBeforeMute = volume;

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

        int IContextAudio::getVoiceOutputVolume() const
        {
            if (!m_voiceClient) { return 0; }
            return m_voiceClient->getNormalizedOutputVolume();
        }

        void IContextAudio::setMute(bool muted)
        {
            if (!m_voiceClient) { return; }
            if (this->isMuted() == muted) { return; } // avoid roundtrips / unnecessary signals

            if (muted)
            {
                const int nv = m_voiceClient->getNormalizedOutputVolume();
                m_outVolumeBeforeMute = nv;
            }

            m_voiceClient->setMuted(muted);
            if (!muted) { m_voiceClient->setNormalizedOutputVolume(m_outVolumeBeforeMute); }

            // signal
            emit this->changedMute(muted);
        }

        bool IContextAudio::isMuted() const
        {
            if (!m_voiceClient) { return false; }
            return m_voiceClient->isMuted();
        }

        void IContextAudio::playSelcalTone(const CSelcal &selcal)
        {
            const CTime t = m_selcalPlayer->play(90, selcal);
            const int ms = t.toMs();
            if (ms > 10)
            {
                // As of https://dev.swift-project.org/T558 play additional notification
                const QPointer<const IContextAudio> myself(this);
                QTimer::singleShot(ms, this, [ = ]
                {
                    if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                    this->playNotification(CNotificationSounds::NotificationTextMessageSupervisor, true);
                });
            }
        }

        void IContextAudio::playNotification(CNotificationSounds::NotificationFlag notification, bool considerSettings, int volume)
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

        void IContextAudio::enableAudioLoopback(bool enable)
        {
            if (!m_voiceClient) { return; }
            m_voiceClient->setLoopBack(enable);
        }

        bool IContextAudio::isAudioLoopbackEnabled() const
        {
            if (!m_voiceClient) { return false; }
            return m_voiceClient->isLoopback();
        }

        void IContextAudio::setVoiceSetup(const CVoiceSetup &setup)
        {
            // could be recycled for some AFV setup
            Q_UNUSED(setup)
        }

        CVoiceSetup IContextAudio::getVoiceSetup() const
        {
            return CVoiceSetup();
        }

        void IContextAudio::setVoiceTransmission(bool enable, PTTCOM com)
        {
            if (!m_voiceClient) { return; }
            m_voiceClient->setPttForCom(enable, com);
        }

        void IContextAudio::setVoiceTransmissionCom1(bool enabled)
        {
            this->setVoiceTransmission(enabled, COM1);
        }

        void IContextAudio::setVoiceTransmissionCom2(bool enabled)
        {
            this->setVoiceTransmission(enabled, COM2);
        }

        void IContextAudio::setVoiceTransmissionComActive(bool enabled)
        {
            this->setVoiceTransmission(enabled, COMActive);
        }

        void IContextAudio::changeDeviceSettings()
        {
            const QString inputDeviceName = m_inputDeviceSetting.get();
            const CAudioDeviceInfo input = this->getAudioInputDevices().findByNameOrDefault(inputDeviceName, CAudioDeviceInfo::getDefaultInputDevice());

            const QString outputDeviceName = m_outputDeviceSetting.get();
            const CAudioDeviceInfo output = this->getAudioOutputDevices().findByNameOrDefault(outputDeviceName, CAudioDeviceInfo::getDefaultOutputDevice());

            this->setCurrentAudioDevices(input, output);
        }

        void IContextAudio::onChangedAudioSettings()
        {
            const CSettings s = m_audioSettings.get();
            const QString dir = s.getNotificationSoundDirectory();
            m_notificationPlayer.updateDirectory(dir);
            this->setVoiceOutputVolume(s.getOutVolume());
        }

        void IContextAudio::onChangedVoiceSettings()
        {
            const CVoiceSetup vs = m_voiceSettings.getThreadLocal();
            m_voiceClient->updateVoiceServerUrl(vs.getAfvVoiceServerUrl());
        }

        void IContextAudio::audioIncreaseVolume(bool enabled)
        {
            if (!enabled) { return; }
            const int v = qRound(this->getVoiceOutputVolume() * 1.2);
            this->setVoiceOutputVolume(v);
        }

        void IContextAudio::audioDecreaseVolume(bool enabled)
        {
            if (!enabled) { return; }
            const int v = qRound(this->getVoiceOutputVolume() / 1.2);
            this->setVoiceOutputVolume(v);
        }

        CComSystem IContextAudio::xCtxGetOwnComSystem(CComSystem::ComUnit unit) const
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

        bool IContextAudio::xCtxIsComIntegratedWithSimulator() const
        {
            if (!this->getIContextSimulator()) { return false; }
            return this->getIContextSimulator()->getSimulatorSettings().isComIntegrated();
        }

        void IContextAudio::xCtxChangedAircraftCockpit(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            Q_UNUSED(aircraft)
            Q_UNUSED(originator)

            /** NOT NEEDED as CAfvClient is directly "tracking changes" **/
        }

        void IContextAudio::xCtxNetworkConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
        {
            if (!m_voiceClient) { return; }

            Q_UNUSED(from)
            BLACK_VERIFY_X(this->getIContextNetwork(), Q_FUNC_INFO, "Missing network context");

            if (to.isConnected() && this->getIContextNetwork())
            {
                const CVoiceSetup vs = m_voiceSettings.getThreadLocal();
                m_voiceClient->updateVoiceServerUrl(vs.getAfvVoiceServerUrl());

                const CUser connectedUser = this->getIContextNetwork()->getConnectedServer().getUser();
                m_voiceClient->connectTo(connectedUser.getId(), connectedUser.getPassword(), connectedUser.getCallsign().asString());
            }
            else if (to.isDisconnected())
            {
                m_voiceClient->stopAudio();
                m_voiceClient->disconnectFrom();
            }
        }
    } // ns
} // ns
