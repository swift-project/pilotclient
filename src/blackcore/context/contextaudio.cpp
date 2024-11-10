// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h" // for user login
#include "blackcore/context/contextownaircraft.h" // for COM integration
#include "blackcore/context/contextsimulator.h" // for COM intergration
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextaudioproxy.h"
#include "blackcore/afv/clients/afvclient.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"
#include "blackmisc/icons.h"
#include "config/buildconfig.h"

#ifdef Q_OS_WIN
#    include "comdef.h"
#endif

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace swift::sound;
using namespace BlackCore::Afv::Clients;

//! \cond

namespace BlackCore::Context
{
    IContextAudio::IContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime)
    {
        // void
    }

    void IContextAudio::onChangedLocalDevices(const CAudioDeviceInfoList &devices)
    {
        this->registerDevices(devices);
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
        // since CContextAudioBaseImpl provides audio on either side (core/GUI) we do not use ContextAudioEmpty
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
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Empty context not supported for audio (since AFV)");
            return nullptr;
        }
    }

    bool CContextAudioBase::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        Q_UNUSED(originator)
        if (commandLine.isEmpty()) { return false; }
        CSimpleCommandParser parser(
            { ".vol", ".volume", // output volume
              ".mute", // mute
              ".unmute", // unmute
              ".aliased" });
        parser.parse(commandLine);
        if (!parser.isKnownCommand()) { return false; }

        if (parser.matchesCommand(".mute"))
        {
            this->setOutputMute(true);
            return true;
        }
        else if (parser.matchesCommand(".unmute"))
        {
            this->setOutputMute(false);
            return true;
        }
        else if (parser.commandStartsWith("vol") && parser.countParts() > 1)
        {
            const int v = parser.toInt(1);
            this->setMasterOutputVolume(v);
            return true;
        }
        else if (afvClient() && parser.matchesCommand(".aliased") && parser.countParts() > 1)
        {
            const bool enable = parser.toBool(1, true);
            afvClient()->enableAliasedStations(enable);

            CLogMessage(this).info(u"Aliased stations are: %1") << boolToOnOff(enable);
            return true;
        }
        return false;
    }

    CContextAudioBase::CContextAudioBase(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextAudio(mode, runtime),
                                                                                                      CIdentifiable(this)
    {
        CContextAudioBase::registerHelp();

        if (CContextAudioBase::isNoAudioSet())
        {
            CLogMessage(this).info(u"Voice client disabled");
        }
        else
        {
            this->initVoiceClient();
        }

        // here we are in a base class of one context
        // the whole context/facade system is not initialized when this code here is executed

        QPointer<CContextAudioBase> myself(this);
        QTimer::singleShot(5000, this, [=] {
            if (!myself || !sApp || sApp->isShuttingDown()) { return; }

            const CSettings as = m_audioSettings.getThreadLocal();
            this->setMasterOutputVolume(as.getOutVolume());
            this->setComOutputVolume(CComSystem::Com1, as.getOutVolumeCom1());
            this->setComOutputVolume(CComSystem::Com2, as.getOutVolumeCom2());
            m_selcalPlayer = new CSelcalPlayer(CAudioDeviceInfo::getDefaultOutputDevice(), this);

            myself->changeDeviceSettings();
            myself->onChangedAudioSettings();
            myself->onChangedLocalDevices(m_activeLocalDevices);
        });
    }

    CContextAudioBase::~CContextAudioBase()
    {
        this->gracefulShutdown();
    }

    void CContextAudioBase::initVoiceClient()
    {
        if (m_voiceClient || !sApp) { return; }

        const CAudioDeviceInfoList devices = CAudioDeviceInfoList::allDevices();
        if (devices != m_activeLocalDevices)
        {
            m_activeLocalDevices = devices;
            emit this->changedLocalAudioDevices(devices);
        }

#ifdef Q_OS_WIN
        if (!m_winCoInitialized)
        {
            HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

            // RPC_E_CHANGED_MODE: CoInitializeEx was already called by someone else in this thread with a different mode.
            if (hr == RPC_E_CHANGED_MODE)
            {
                CLogMessage(this).debug(u"CoInitializeEx was already called with a different mode. Trying again.");
                hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            }

            // S_OK: The COM library was initialized successfully on this thread.
            // S_FALSE: The COM library is already initialized on this thread. Reference count was incremented. This is not an error.
            if (hr == S_OK || hr == S_FALSE) { m_winCoInitialized = true; }
        }
#endif

        m_voiceClient = new CAfvClient(sApp->getGlobalSetup().getAfvApiServerUrl().toQString(), this);

        Q_ASSERT_X(m_voiceClient->thread() == qApp->thread(), Q_FUNC_INFO, "Should be in main thread");
        m_voiceClient->start(); // thread
        Q_ASSERT_X(m_voiceClient->owner() == this, Q_FUNC_INFO, "Wrong owner");
        Q_ASSERT_X(m_voiceClient->thread() != qApp->thread(), Q_FUNC_INFO, "Must NOT be in main thread");

        // connect(m_voiceClient, &CAfvClient::outputVolumePeakVU,            this, &CContextAudioBase::outputVolumePeakVU, Qt::QueuedConnection);
        // connect(m_voiceClient, &CAfvClient::inputVolumePeakVU,             this, &CContextAudioBase::inputVolumePeakVU,  Qt::QueuedConnection);
        // connect(m_voiceClient, &CAfvClient::receivingCallsignsChanged,     this, &CContextAudioBase::receivingCallsignsChanged,     Qt::QueuedConnection);
        // connect(m_voiceClient, &CAfvClient::updatedFromOwnAircraftCockpit, this, &CContextAudioBase::updatedFromOwnAircraftCockpit, Qt::QueuedConnection);
        connect(m_voiceClient, &CAfvClient::startedAudio, this, &CContextAudioBase::startedAudio, Qt::QueuedConnection);
        connect(m_voiceClient, &CAfvClient::stoppedAudio, this, &CContextAudioBase::stoppedAudio, Qt::QueuedConnection);
        connect(m_voiceClient, &CAfvClient::ptt, this, &CContextAudioBase::ptt, Qt::QueuedConnection);
        connect(m_voiceClient, &CAfvClient::changedOutputMute, this, &CContextAudioBase::changedOutputMute, Qt::QueuedConnection);
        connect(m_voiceClient, &CAfvClient::connectionStatusChanged, this, &CContextAudioBase::onAfvConnectionStatusChanged, Qt::QueuedConnection);
        connect(m_voiceClient, &CAfvClient::afvConnectionFailure, this, &CContextAudioBase::onAfvConnectionFailure, Qt::QueuedConnection);
    }

    void CContextAudioBase::terminateVoiceClient()
    {
        if (m_voiceClient)
        {
            m_voiceClient->gracefulShutdown();
            Q_ASSERT_X(CThreadUtils::isInThisThread(m_voiceClient), Q_FUNC_INFO, "Needs to be back in current thread");
            m_voiceClient->deleteLater();
            m_voiceClient = nullptr;
#ifdef Q_OS_WIN
            if (m_winCoInitialized)
            {
                CoUninitialize();
                m_winCoInitialized = false;
            }
#endif
        }
    }

    void CContextAudioBase::gracefulShutdown()
    {
        this->terminateVoiceClient();
        if (m_selcalPlayer)
        {
            m_selcalPlayer->gracefulShutdown();
            m_selcalPlayer = nullptr;
        }
        QObject::disconnect(this);
    }

    void CContextAudioBase::setRxTx(bool rx1, bool tx1, bool rx2, bool tx2)
    {
        if (m_voiceClient)
        {
            m_voiceClient->setRxTx(rx1, tx1, rx2, tx2);
        }
    }

    void CContextAudioBase::getRxTx(bool &rx1, bool &tx1, bool &rx2, bool &tx2) const
    {
        if (m_voiceClient)
        {
            m_voiceClient->setRxTx(rx1, tx1, rx2, tx2);
        }
    }

    const CIdentifier &CContextAudioBase::audioRunsWhere() const
    {
        static const CIdentifier i("CContextAudioBaseImpl");
        return i;
    }

    bool CContextAudioBase::isEnabledComUnit(CComSystem::ComUnit comUnit) const
    {
        if (!m_voiceClient) { return false; }
        return m_voiceClient->isEnabledComUnit(comUnit);
    }

    bool CContextAudioBase::isTransmittingComUnit(CComSystem::ComUnit comUnit) const
    {
        if (!m_voiceClient) { return false; }
        return m_voiceClient->isTransmittingComUnit(comUnit);
    }

    bool CContextAudioBase::connectAudioWithNetworkCredentials()
    {
        if (!m_voiceClient) { return false; }
        if (!sApp || sApp->isShuttingDown() || !sApp->getIContextNetwork()) { return false; }

        const CEcosystem ecoSystem = this->getIContextNetwork()->getConnectedServer().getEcosystem();
        if (ecoSystem != CEcosystem::vatsim())
        {
            CLogMessage(this).info(u"Will not use AFV as ecosystem is '%1'") << ecoSystem.toQString(true);
            return false;
        }

        const CUser connectedUser = this->getIContextNetwork()->getConnectedServer().getUser();
        const QString client = "swift " % swift::config::CBuildConfig::getShortVersionString();
        CCallsign cs = connectedUser.getCallsign();
        this->unRegisterAudioCallsign(cs, this->identifier()); // un-register "myself"
        if (this->hasRegisteredAudioCallsign(cs)) // anybody else using that callsign
        {
            //! \todo KB 2019-11 would need a better algorithm to really find a cs
            cs = CCallsign(cs.asString() + "2");
        }
        CLogMessage(this).info(u"About to connect to voice as '%1' '%2'") << connectedUser.getId() << cs;
        m_voiceClient->connectTo(connectedUser.getId(), connectedUser.getPassword(), cs.asString(), client);
        this->registerAudioCallsign(cs, this->identifier()); // login can still fail, but we "block" this callsign
        return true;
    }

    bool CContextAudioBase::isAudioConnected() const
    {
        return m_voiceClient && m_voiceClient->isConnected();
    }

    bool CContextAudioBase::isAudioStarted() const
    {
        return m_voiceClient && m_voiceClient->isStarted();
    }

    bool CContextAudioBase::isComUnitIntegrated() const
    {
        return m_voiceClient && m_voiceClient->isComUnitIntegrated();
    }

    const QList<QCommandLineOption> &CContextAudioBase::getCmdLineOptions()
    {
        static const QList<QCommandLineOption> opts {
            QCommandLineOption({ { "n", "noaudio" }, QCoreApplication::translate("CContextAudioBase", "No audio for GUI or core.", "noaudio") })
        };
        return opts;
    }

    bool CContextAudioBase::isNoAudioSet()
    {
        if (!sApp) { return false; }
        return sApp->isParserOptionSet("noaudio");
    }

    QString CContextAudioBase::audioRunsWhereInfo() const
    {
        const QString s = QStringLiteral("[%1] Audio on '%2', '%3'.").arg(boolToEnabledDisabled(this->isAudioStarted()), audioRunsWhere().getMachineName(), audioRunsWhere().getProcessName());
        return s;
    }

    CAudioDeviceInfoList CContextAudioBase::getAudioDevices() const
    {
        return CAudioDeviceInfoList::allDevices();
    }

    CAudioDeviceInfoList CContextAudioBase::getAudioInputDevices() const
    {
        return this->getAudioDevices().getInputDevices();
    }

    CAudioDeviceInfoList CContextAudioBase::getAudioOutputDevices() const
    {
        return this->getAudioDevices().getOutputDevices();
    }

    CAudioDeviceInfoList CContextAudioBase::getAudioDevicesPlusDefault() const
    {
        return CAudioDeviceInfoList::allDevicesPlusDefault();
    }

    CAudioDeviceInfoList CContextAudioBase::getAudioInputDevicesPlusDefault() const
    {
        return this->getAudioDevicesPlusDefault().getInputDevices();
    }

    CAudioDeviceInfoList CContextAudioBase::getAudioOutputDevicesPlusDefault() const
    {
        return this->getAudioDevicesPlusDefault().getOutputDevices();
    }

    CAudioDeviceInfoList CContextAudioBase::getCurrentAudioDevices() const
    {
        const QString inputDeviceName = m_inputDeviceSetting.get();
        const CAudioDeviceInfo inputDevice = this->getAudioInputDevicesPlusDefault().findByNameOrDefault(inputDeviceName, CAudioDeviceInfo::getDefaultInputDevice());

        const QString outputDeviceName = m_outputDeviceSetting.get();
        const CAudioDeviceInfo outputDevice = this->getAudioOutputDevicesPlusDefault().findByNameOrDefault(outputDeviceName, CAudioDeviceInfo::getDefaultOutputDevice());

        CAudioDeviceInfoList devices;
        devices.push_back(inputDevice);
        devices.push_back(outputDevice);
        return devices;
    }

    void CContextAudioBase::setCurrentAudioDevices(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
    {
        if (!m_voiceClient) { return; }
        if (!sApp) { return; }

        if (!inputDevice.getName().isEmpty() && inputDevice.getName() != m_inputDeviceSetting.get())
        {
            Q_ASSERT_X(inputDevice.isInputDevice(), Q_FUNC_INFO, "Need input device");
            const CStatusMessage m = m_inputDeviceSetting.setAndSave(inputDevice.getName());
            CLogMessage::preformatted(m);
        }
        if (!outputDevice.getName().isEmpty() && outputDevice.getName() != m_outputDeviceSetting.get())
        {
            Q_ASSERT_X(outputDevice.isOutputDevice(), Q_FUNC_INFO, "Need output device");
            const CStatusMessage m = m_outputDeviceSetting.setAndSave(outputDevice.getName());
            CLogMessage::preformatted(m);
        }

        m_voiceClient->startAudio(inputDevice, outputDevice);
    }

    void CContextAudioBase::setMasterOutputVolume(int volume)
    {
        if (!m_voiceClient) { return; }

        const bool wasMuted = this->isOutputMuted();
        volume = CSettings::fixOutVolume(volume);

        const int currentVolume = m_voiceClient->getNormalizedMasterOutputVolume();
        const bool changedVoiceOutput = (currentVolume != volume);
        if (changedVoiceOutput)
        {
            // TODO: KB 2020-05 the mute handling should entirely go to AFV client!
            m_voiceClient->setNormalizedMasterOutputVolume(volume);
            m_outMasterVolumeBeforeMute = volume;

            emit this->changedAudioVolume(volume);
            if ((volume > 0 && wasMuted) || (volume < 1 && !wasMuted))
            {
                // inform about muted
                emit this->changedOutputMute(volume < 1);
            }
        }

        CSettings as(m_audioSettings.getThreadLocal());
        if (as.getOutVolume() != volume)
        {
            as.setOutVolume(volume);
            m_audioSettings.set(as);
        }
    }

    void CContextAudioBase::setComOutputVolume(CComSystem::ComUnit comUnit, int volume)
    {
        if (comUnit != CComSystem::Com1 && comUnit != CComSystem::Com2) { return; }
        if (!m_voiceClient) { return; }

        volume = CSettings::fixOutVolume(volume);

        const int currentVolume = m_voiceClient->getNormalizedComOutputVolume(comUnit);
        const bool changedVoiceOutput = (currentVolume != volume);
        if (changedVoiceOutput)
        {
            m_voiceClient->setNormalizedComOutputVolume(comUnit, volume);
            emit this->changedAudioVolume(volume);
        }

        CSettings as(m_audioSettings.getThreadLocal());
        if (comUnit == CComSystem::Com1 && as.getOutVolumeCom1() != volume)
        {
            as.setOutVolumeCom1(volume);
            m_audioSettings.set(as);
        }
        else if (comUnit == CComSystem::Com2 && as.getOutVolumeCom2() != volume)
        {
            as.setOutVolumeCom2(volume);
            m_audioSettings.set(as);
        }
    }

    int CContextAudioBase::getMasterOutputVolume() const
    {
        if (!m_voiceClient) { return 0; }
        return m_voiceClient->getNormalizedMasterOutputVolume();
    }

    int CContextAudioBase::getComOutputVolume(CComSystem::ComUnit comUnit) const
    {
        if (!m_voiceClient) { return 0; }
        return m_voiceClient->getNormalizedComOutputVolume(comUnit);
    }

    void CContextAudioBase::setOutputMute(bool muted)
    {
        if (!m_voiceClient) { return; }
        if (this->isOutputMuted() == muted) { return; } // avoid roundtrips / unnecessary signals

        if (muted) { m_outMasterVolumeBeforeMute = m_voiceClient->getNormalizedMasterOutputVolume(); }

        m_voiceClient->setOutputMuted(muted);
        if (!muted) { m_voiceClient->setNormalizedMasterOutputVolume(m_outMasterVolumeBeforeMute); }

        // signal no longer need, signaled by m_voiceClient->setMuted
        // emit this->changedMute(muted);
    }

    bool CContextAudioBase::isOutputMuted() const
    {
        if (!m_voiceClient) { return false; }
        return m_voiceClient->isOutputMuted();
    }

    void CContextAudioBase::playSelcalTone(const CSelcal &selcal)
    {
        const CTime t = m_selcalPlayer->play(90, selcal);
        const int ms = t.toMs();
        if (ms > 10)
        {
            // As of https://dev.swift-project.org/T558 play additional notification
            const QPointer<const CContextAudioBase> myself(this);
            QTimer::singleShot(ms, this, [=] {
                if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                this->playNotification(CNotificationSounds::NotificationTextMessageSupervisor, true);
            });
        }
    }

    void CContextAudioBase::playNotification(CNotificationSounds::NotificationFlag notification, bool considerSettings, int volume)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << notification; }

        const CSettings settings = m_audioSettings.getThreadLocal();
        const bool play = !considerSettings || settings.isNotificationFlagSet(notification);
        if (!play) { return; }

        if (volume < 0 || volume > 100)
        {
            volume = 90;
            if (considerSettings) { volume = qMax(25, settings.getNotificationVolume()); }
        }
        m_notificationPlayer.play(notification, volume);
    }

    void CContextAudioBase::enableAudioLoopback(bool enable)
    {
        if (!m_voiceClient) { return; }
        m_voiceClient->setLoopBack(enable);
    }

    bool CContextAudioBase::isAudioLoopbackEnabled() const
    {
        if (!m_voiceClient) { return false; }
        return m_voiceClient->isLoopback();
    }

    void CContextAudioBase::setVoiceTransmission(bool enable)
    {
        if (!m_voiceClient) { return; }
        m_voiceClient->setPtt(enable);
    }

    void CContextAudioBase::changeDeviceSettings()
    {
        const CAudioDeviceInfoList devices = this->getCurrentAudioDevices();
        Q_ASSERT_X(devices.size() == 2, Q_FUNC_INFO, "Expect INPUT and OUTPUT device");

        const CAudioDeviceInfo input = devices.front();
        const CAudioDeviceInfo output = devices.back();
        this->setCurrentAudioDevices(input, output);
    }

    void CContextAudioBase::onChangedAudioSettings()
    {
        const CSettings s = m_audioSettings.get();
        const QString dir = s.getNotificationSoundDirectory();
        m_notificationPlayer.updateDirectory(dir);
        this->setMasterOutputVolume(s.getOutVolume());
        this->setComOutputVolume(CComSystem::Com1, s.getOutVolumeCom1());
        this->setComOutputVolume(CComSystem::Com2, s.getOutVolumeCom2());
    }

    void CContextAudioBase::audioIncreaseVolume(bool enabled)
    {
        if (!enabled) { return; }
        const int v = qRound(this->getMasterOutputVolume() * 1.05);
        this->setMasterOutputVolume(v);
    }

    void CContextAudioBase::audioDecreaseVolume(bool enabled)
    {
        if (!enabled) { return; }
        const int v = qRound(this->getMasterOutputVolume() / 1.05);
        this->setMasterOutputVolume(v);
    }

    void CContextAudioBase::audioIncreaseVolumeCom1(bool enabled)
    {
        if (!enabled) { return; }
        if (isComUnitIntegrated()) { return; }
        const int v = qRound(this->getComOutputVolume(CComSystem::Com1) * 1.05);
        this->setComOutputVolume(CComSystem::Com1, v);
    }

    void CContextAudioBase::audioDecreaseVolumeCom1(bool enabled)
    {
        if (!enabled) { return; }
        if (isComUnitIntegrated()) { return; }
        const int v = qRound(this->getComOutputVolume(CComSystem::Com1) / 1.05);
        this->setComOutputVolume(CComSystem::Com1, v);
    }

    void CContextAudioBase::audioIncreaseVolumeCom2(bool enabled)
    {
        if (!enabled) { return; }
        if (isComUnitIntegrated()) { return; }
        const int v = qRound(this->getComOutputVolume(CComSystem::Com2) * 1.05);
        this->setComOutputVolume(CComSystem::Com2, v);
    }

    void CContextAudioBase::audioDecreaseVolumeCom2(bool enabled)
    {
        if (!enabled) { return; }
        if (isComUnitIntegrated()) { return; }
        const int v = qRound(this->getComOutputVolume(CComSystem::Com2) / 1.05);
        this->setComOutputVolume(CComSystem::Com2, v);
    }

    void CContextAudioBase::xCtxNetworkConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        if (!m_voiceClient) { return; }

        Q_UNUSED(from)
        SWIFT_VERIFY_X(this->getIContextNetwork(), Q_FUNC_INFO, "Missing network context");

        // we only change network connection of AFV client here
        if (to.isConnected() && this->getIContextNetwork())
        {
            const bool connected = this->connectAudioWithNetworkCredentials();
            Q_UNUSED(connected)

            // one reason for not connecting is NOT using the VATSIM ecosystem
        }
        else if (to.isDisconnected())
        {
            m_voiceClient->disconnectFrom();
        }
    }

    void CContextAudioBase::onAfvConnectionStatusChanged(int status)
    {
        if (!m_voiceClient) { return; }

        const CCallsign cs = m_voiceClient->getCallsign();
        const CAfvClient::ConnectionStatus s = static_cast<CAfvClient::ConnectionStatus>(status);

        switch (s)
        {
        case CAfvClient::Connected:
            this->registerAudioCallsign(cs, this->identifier());
            break;
        case CAfvClient::Disconnected:
            this->unRegisterAudioCallsign(cs, this->identifier());
            break;
        }
    }

    void CContextAudioBase::onAfvConnectionFailure(const CStatusMessage &msg)
    {
        if (!m_voiceClient) { return; }
        emit this->voiceClientFailure(msg);
    }

    bool CContextAudioBase::isRunningWithLocalCore()
    {
        return sApp && sApp->isLocalContext();
    }

} // ns

//! \endcond
