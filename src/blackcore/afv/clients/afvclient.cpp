/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "afvclient.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/application.h"
#include "blacksound/audioutilities.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"

#ifdef Q_OS_WIN
#include "comdef.h"
#endif

#include <QDebug>

using namespace BlackCore::Context;
using namespace BlackCore::Afv::Audio;
using namespace BlackCore::Afv::Connection;
using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackSound;
using namespace BlackSound::SampleProvider;

namespace BlackCore
{
    namespace Afv
    {
        namespace Clients
        {
            constexpr int CAfvClient::PositionUpdatesMs;
            constexpr int CAfvClient::SampleRate;
            constexpr int CAfvClient::FrameSize;
            constexpr double CAfvClient::MinDbIn;
            constexpr double CAfvClient::MaxDbIn;
            constexpr double CAfvClient::MinDbOut;
            constexpr double CAfvClient::MaxDbOut;
            constexpr quint32 CAfvClient::UniCom;

            const CLogCategoryList &CAfvClient::getLogCategories()
            {
                static const CLogCategoryList cats { CLogCategory::audio(), CLogCategory::vatsimSpecific() };
                return cats;
            }

            CAfvClient::CAfvClient(const QString &apiServer, QObject *owner) :
                CContinuousWorker(owner, "CAfvClient"),
                m_connection(new CClientConnection(apiServer, this)),
                m_input(new CInput(SampleRate, this)),
                m_output(new COutput(this)),
                m_voiceServerTimer(new QTimer(this))
            {
                this->setObjectName("AFV client: " + apiServer);
                m_connection->setReceiveAudio(false);

                connect(m_input, &CInput::opusDataAvailable, this, &CAfvClient::opusDataAvailable);
                connect(m_input, &CInput::inputVolumeStream, this, &CAfvClient::inputVolumeStream);

                connect(m_output,     &COutput::outputVolumeStream,      this, &CAfvClient::outputVolumeStream);
                connect(m_connection, &CClientConnection::audioReceived, this, &CAfvClient::audioOutDataAvailable);
                connect(m_voiceServerTimer, &QTimer::timeout,            this, &CAfvClient::onTimerUpdate);

                m_updateTimer.stop(); // not used

                // deferred init - use BlackMisc:: singleShot to call in correct thread, "myself" NOT needed
                BlackMisc::singleShot(1000, this, [ = ]
                {
                    this->deferredInit();
                });
            }

            QString CAfvClient::getCallsign() const
            {
                QMutexLocker lock(&m_mutexCallsign);
                return m_callsign;
            }

            void CAfvClient::setCallsign(const QString &callsign)
            {
                QMutexLocker lock(&m_mutexCallsign);
                m_callsign = callsign;
            }

            bool CAfvClient::isConnected() const
            {
                QMutexLocker lock(&m_mutexConnection);
                return m_connection->isConnected();
            }

            void CAfvClient::initTransceivers()
            {
                {
                    QMutexLocker lock(&m_mutexTransceivers);
                    m_transceivers =
                    {
                        { 0, UniCom, 48.5, 11.5, 1000.0, 1000.0 },
                        { 1, UniCom, 48.5, 11.5, 1000.0, 1000.0 }
                    };

                    m_enabledTransceivers = { 0, 1 };
                    m_transmittingTransceivers = { { 0 } }; // TxTransceiverDto
                }

                // init with context values
                this->connectWithContexts();

                // update from context
                this->onTimerUpdate();
            }

            void CAfvClient::connectWithContexts()
            {
                if (m_connectedWithContext) { return; }
                if (!hasContexts())         { return; }
                this->disconnect(sApp->getIContextOwnAircraft());
                sApp->getIContextOwnAircraft()->disconnect(this);
                connect(sApp->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CAfvClient::onUpdateTransceiversFromContext, Qt::QueuedConnection);
                m_connectedWithContext = true;
            }

            void CAfvClient::fetchSimulatorSettings()
            {
                // call that in correct thread
                if (!hasContexts()) { return; }

                if (QThread::currentThread() != sApp->getIContextSimulator()->thread())
                {
                    // Method needs to be executed in the context thread
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(sApp->getIContextSimulator(), [ = ]() { if (myself) { this->fetchSimulatorSettings(); }});
                    return;
                }

                const bool integrated = sApp->getIContextSimulator()->getSimulatorSettings().isComIntegrated();
                const bool changed    = integrated != m_integratedComUnit;

                m_integratedComUnit = integrated;
                if (changed)
                {
                    emit this->updatedFromOwnAircraftCockpit();
                }
            }

            void CAfvClient::connectTo(const QString &cid, const QString &password, const QString &callsign, const QString &client)
            {
                if (QThread::currentThread() != thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(this, [ = ]() { if (myself) { connectTo(cid, password, callsign, client); }});
                    return;
                }

                // called in CAfvClient thread
                this->connectWithContexts();
                this->setCallsign(callsign);

                QPointer<CAfvClient> myself(this);
                if (!this->isConnected() && m_retryConnectAttempt == 0)
                {
                    // check if connect simply did NOT receive an answer
                    QTimer::singleShot(20 * 1000, this, [ = ]
                    {
                        if (!myself) { return; }
                        if (m_retryConnectAttempt > 0) { return; } // already handled

                        // this will reconnect ONLY if not already connected
                        this->retryConnectTo(cid, password, callsign, client, QStringLiteral("No connection afer 20secs"));
                    });
                }
                // thread safe connect
                {
                    QMutexLocker lock(&m_mutexConnection);

                    // async connection
                    m_connection->connectTo(cid, password, callsign, client,
                    {
                        // this is the callback when the connection has been established
                        this, [ = ](bool authenticated)
                        {
                            if (!myself) { return; }

                            // HF stations aliased
                            const QVector<StationDto> aliasedStations = m_connection->getAllAliasedStations();
                            this->setAliasedStations(aliasedStations); // threadsafe
                            this->onTimerUpdate();

                            // const bool isConnected = this->isConnected(); // threadsafe
                            if (authenticated)
                            {
                                // restart timer, normally it should be started already, paranoia
                                // as I run in "my thread" starting timer should be OK
                                {
                                    QMutexLocker lock(&m_mutex);
                                    if (m_voiceServerTimer) { m_voiceServerTimer->start(PositionUpdatesMs); }
                                }
                                m_retryConnectAttempt = 0;
                                emit this->connectionStatusChanged(Connected);
                            }
                            else
                            {
                                myself->retryConnectTo(cid, password, callsign, client, QStringLiteral("AFV authentication failed for '%1' callsign '%2'").arg(cid, callsign));
                                emit this->connectionStatusChanged(Disconnected);
                            }
                        }
                    });
                }
            }

            void CAfvClient::disconnectFrom(bool stop)
            {
                if (QThread::currentThread() != thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(this, [ = ]() { if (myself) { disconnectFrom(stop); }});
                    return;
                }

                // we intentionally DO NOT STOP the timer here, but keep it for preset (own aircraft pos.)
                // threadsafe
                {
                    QMutexLocker lock(&m_mutexConnection);
                    m_connection->disconnectFrom();
                }
                emit connectionStatusChanged(Disconnected);

                if (stop) { this->stopAudio(); }
            }

            QStringList CAfvClient::availableInputDevices() const
            {
                return CAudioDeviceInfoList::allInputDevices().getDeviceNames();
            }

            QStringList CAfvClient::availableOutputDevices() const
            {
                return CAudioDeviceInfoList::allOutputDevices().getDeviceNames();
            }

            void CAfvClient::setBypassEffects(bool value)
            {
                QMutexLocker lock(&m_mutexSampleProviders);
                if (m_soundcardSampleProvider)
                {
                    m_soundcardSampleProvider->setBypassEffects(value);
                }
            }

            bool CAfvClient::isMuted() const
            {
                const int v = this->getNormalizedOutputVolume();
                return v < 1;
            }

            void CAfvClient::setMuted(bool mute)
            {
                this->setNormalizedOutputVolume(mute ? 0 : 50);
            }

            void CAfvClient::startAudio()
            {
                const CAudioDeviceInfo inputDevice  = this->getInputDevice();
                const CAudioDeviceInfo outputDevice = this->getOutputDevice();
                this->startAudio(inputDevice, outputDevice);
            }

            void CAfvClient::startAudio(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
            {
                if (QThread::currentThread() != this->thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(this, [ = ]() { if (myself) { startAudio(inputDevice, outputDevice); }});
                    return;
                }

                const CAudioDeviceInfo useInputDevice  = inputDevice.isValid()  ? inputDevice  : CAudioDeviceInfo::getDefaultInputDevice();
                const CAudioDeviceInfo useOutputDevice = outputDevice.isValid() ? outputDevice : CAudioDeviceInfo::getDefaultOutputDevice();

                BLACK_VERIFY_X(useInputDevice.isValid()  && useInputDevice.isInputDevice(),   Q_FUNC_INFO, "Wrong input device");
                BLACK_VERIFY_X(useOutputDevice.isValid() && useOutputDevice.isOutputDevice(), Q_FUNC_INFO, "Wrong output device");

                if (m_isStarted)
                {
                    if (this->usesSameDevices(useInputDevice, useOutputDevice))
                    {
                        CLogMessage(this).info(u"Client already started for '%1'/'%2'") << useInputDevice.getName() << useOutputDevice.getName();
                        return;
                    }
                    this->stopAudio();
                }

                this->initTransceivers();

                // threadsafe block
                const double outputVolume = this->getOutputGainRatio();
                {
                    // lock block 1
                    {
                        QMutexLocker lock{&m_mutexSampleProviders};
                        if (m_soundcardSampleProvider)
                        {
                            m_soundcardSampleProvider->disconnect();
                            m_soundcardSampleProvider->deleteLater();
                        }
                        m_soundcardSampleProvider = new CSoundcardSampleProvider(SampleRate, allTransceiverIds(), this);
                        connect(m_soundcardSampleProvider, &CSoundcardSampleProvider::receivingCallsignsChanged, this, &CAfvClient::onReceivingCallsignsChanged);

                        if (m_outputSampleProvider) { m_outputSampleProvider->deleteLater(); }
                        m_outputSampleProvider = new CVolumeSampleProvider(m_soundcardSampleProvider, this);
                        m_outputSampleProvider->setGainRatio(outputVolume);
                    }

                    // lock block 2
                    {
                        QMutexLocker lock(&m_mutex);

                        m_output->start(useOutputDevice, m_outputSampleProvider);
                        m_input->start(useInputDevice);
                        m_startDateTimeUtc = QDateTime::currentDateTimeUtc();

                        // runs in correct thread
                        m_voiceServerTimer->start(PositionUpdatesMs); // start for preset values
                    }
                }

                this->setReceiveAudio(true); // threadsafe

                this->onSettingsChanged(); // make sure all settings are applied
                m_isStarted = true;
                CLogMessage(this).info(u"Started [Input: %1] [Output: %2]") << useInputDevice.getName() << useOutputDevice.getName();

                this->onTimerUpdate(); // update values

                emit this->startedAudio(useInputDevice, useOutputDevice);
            }

            void CAfvClient::startAudio(const QString &inputDeviceName, const QString &outputDeviceName)
            {
                const CAudioDeviceInfo i = CAudioDeviceInfoList::allInputDevices().findByName(inputDeviceName);
                const CAudioDeviceInfo o = CAudioDeviceInfoList::allOutputDevices().findByName(outputDeviceName);
                this->startAudio(i, o);
            }

            void CAfvClient::stopAudio()
            {
                if (QThread::currentThread() != this->thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(this, [ = ]() { if (myself) stopAudio(); });
                    return;
                }

                if (!m_isStarted)
                {
                    CLogMessage(this).info(u"Client was NOT started, not stopping!");
                    return;
                }

                m_isStarted = false;
                this->setReceiveAudio(false); // threadsafe

                // stop input/output
                {
                    QMutexLocker lock{&m_mutex};
                    m_input->stop();
                    m_output->stop();
                }
                CLogMessage(this).info(u"AFV Client stopped");

                emit this->inputVolumePeakVU(0.0);
                emit this->outputVolumePeakVU(0.0);
                emit this->stoppedAudio();
            }

            void CAfvClient::restartAudio()
            {
                if (!m_isStarted)
                {
                    // just need to start
                    this->startAudio();
                    return;
                }

                this->stopAudio();
                QPointer<CAfvClient> myself(this);
                QTimer::singleShot(1000, this, [ = ]
                {
                    if (myself) { myself->startAudio(); }
                });
            }

            /* disabled because NOT used
            double CAfvClient::getDeviceInputVolume() const
            {
                if (m_input) { return m_input->getDeviceInputVolume(); }
                return 0;
            }

            bool CAfvClient::setDeviceInputVolume(double volume)
            {
                if (m_input) { return m_input->setDeviceInputVolume(volume); }
                return false;
            }

            double CAfvClient::getDeviceOutputVolume() const
            {
                if (m_output) { return m_output->getDeviceOutputVolume(); }
                return 0;
            }

            bool CAfvClient::setDeviceOutputVolume(double volume)
            {
                if (m_output) { return m_output->setDeviceOutputVolume(volume); }
                return false;
            }
            */

            void CAfvClient::setReceiveAudio(bool receive)
            {
                QMutexLocker lock(&m_mutexConnection);
                if (!m_connection) { return; }
                m_connection->setReceiveAudio(receive);
            }

            void CAfvClient::enableTransceiver(quint16 id, bool enable)
            {
                {
                    QMutexLocker lock(&m_mutexTransceivers);
                    if (enable) { m_enabledTransceivers.insert(id); }
                    else        { m_enabledTransceivers.remove(id); }
                }

                this->updateTransceivers();
            }

            void CAfvClient::enableComUnit(CComSystem::ComUnit comUnit, bool enable)
            {
                this->enableTransceiver(comUnitToTransceiverId(comUnit), enable);
            }

            bool CAfvClient::isEnabledTransceiver(quint16 id) const
            {
                // we double check, enabled and exist!
                const auto enabledTransceivers = this->getEnabledTransceivers(); // threadsafe
                if (!enabledTransceivers.contains(id)) { return false; }

                const auto transceivers = this->getTransceivers(); // threadsafe
                for (const TransceiverDto &dto : transceivers)
                {
                    if (dto.id == id) { return true; }
                }
                return false;
            }

            bool CAfvClient::isEnabledComUnit(CComSystem::ComUnit comUnit) const
            {
                return this->isEnabledTransceiver(comUnitToTransceiverId(comUnit));
            }

            void CAfvClient::updateComFrequency(quint16 id, quint32 frequencyHz)
            {
                if (id != 0 && id != 1) { return; }

                // Fix rounding issues like 128074999 Hz -> 128075000 Hz
                quint32 roundedFrequencyHz = static_cast<quint32>(qRound(frequencyHz / 1000.0)) * 1000;
                roundedFrequencyHz = this->getAliasFrequencyHz(roundedFrequencyHz);

                bool updateTransceivers = false;
                {
                    QMutexLocker lockTransceivers(&m_mutexTransceivers);
                    if (m_transceivers.size() >= id + 1)
                    {
                        if (m_transceivers[id].frequencyHz != roundedFrequencyHz)
                        {
                            updateTransceivers = true;
                            m_transceivers[id].frequencyHz = roundedFrequencyHz;
                        }
                    }
                }

                // outside lock to avoid deadlock
                if (updateTransceivers)
                {
                    this->updateTransceivers(false); // no frequency update
                }
            }

            void CAfvClient::updateComFrequency(CComSystem::ComUnit comUnit, const CFrequency &comFrequency)
            {
                const quint32 freqHz = static_cast<quint32>(comFrequency.valueInteger(CFrequencyUnit::Hz()));
                this->updateComFrequency(comUnitToTransceiverId(comUnit), freqHz);
            }

            void CAfvClient::updateComFrequency(CComSystem::ComUnit comUnit, const CComSystem &comSystem)
            {
                this->updateComFrequency(comUnit, comSystem.getFrequencyActive());
            }

            void CAfvClient::updatePosition(double latitudeDeg, double longitudeDeg, double heightMeters)
            {
                QMutexLocker lock(&m_mutexTransceivers);
                for (TransceiverDto &transceiver : m_transceivers)
                {
                    transceiver.LatDeg = latitudeDeg;
                    transceiver.LonDeg = longitudeDeg;
                    transceiver.HeightAglM = heightMeters;
                    transceiver.HeightMslM = heightMeters;
                }
            }

            void CAfvClient::updateTransceivers(bool updateFrequencies)
            {
                // also update if NOT connected, values will be preset

                if (hasContexts())
                {
                    const CSimulatedAircraft ownAircraft = sApp->getIContextOwnAircraft()->getOwnAircraft();
                    this->updatePosition(ownAircraft.latitude().value(CAngleUnit::deg()),
                                         ownAircraft.longitude().value(CAngleUnit::deg()),
                                         ownAircraft.getAltitude().value(CLengthUnit::ft()));

                    if (updateFrequencies)
                    {
                        this->updateComFrequency(CComSystem::Com1, ownAircraft.getCom1System());
                        this->updateComFrequency(CComSystem::Com2, ownAircraft.getCom2System());
                    }
                }

                // threadsafe copies
                const auto transceivers = this->getTransceivers();
                const auto enabledTransceivers = this->getEnabledTransceivers();
                const QString callsign = this->getCallsign(); // threadsafe

                // transceivers
                QVector<TransceiverDto> newEnabledTransceivers;
                for (const TransceiverDto &transceiver : transceivers)
                {
                    if (enabledTransceivers.contains(transceiver.id))
                    {
                        newEnabledTransceivers.push_back(transceiver);
                    }
                }

                // in connection and soundcard only use the enabled transceivers
                {
                    QMutexLocker lock(&m_mutexConnection);
                    if (m_connection) { m_connection->updateTransceivers(callsign, newEnabledTransceivers); }
                }
                {
                    QMutexLocker lock(&m_mutexSampleProviders);
                    if (m_soundcardSampleProvider) { m_soundcardSampleProvider->updateRadioTransceivers(newEnabledTransceivers); }
                }
            }

            void CAfvClient::setTransmittingTransceiver(quint16 transceiverID)
            {
                const TxTransceiverDto tx = { transceiverID };
                this->setTransmittingTransceivers({ tx });
            }

            void CAfvClient::setTransmittingComUnit(CComSystem::ComUnit comUnit)
            {
                this->setTransmittingTransceiver(comUnitToTransceiverId(comUnit));
            }

            void CAfvClient::setTransmittingTransceivers(const QVector<TxTransceiverDto> &transceivers)
            {
                QMutexLocker lock(&m_mutexTransceivers);
                m_transmittingTransceivers = transceivers;
            }

            bool CAfvClient::isTransmittingTransceiver(quint16 id) const
            {
                QMutexLocker lock(&m_mutexTransceivers);
                for (const TxTransceiverDto &dto : m_transmittingTransceivers)
                {
                    if (dto.id == id) { return true; }
                }
                return false;
            }

            bool CAfvClient::isTransmittingdComUnit(CComSystem::ComUnit comUnit) const
            {
                return this->isTransmittingTransceiver(comUnitToTransceiverId(comUnit));
            }

            void CAfvClient::setRxTx(bool rx1, bool tx1, bool rx2, bool tx2)
            {
                QVector<TxTransceiverDto> txs;
                if (tx1)
                {
                    const TxTransceiverDto tx = { comUnitToTransceiverId(CComSystem::Com1) };
                    txs.push_back(tx);
                }
                if (tx2)
                {
                    const TxTransceiverDto tx = { comUnitToTransceiverId(CComSystem::Com2) };
                    txs.push_back(tx);
                }
                this->setTransmittingTransceivers(txs);

                QSet<quint16> enabledTransceivers;
                if (rx1 || tx1)
                {
                    enabledTransceivers.insert(comUnitToTransceiverId(CComSystem::Com1));
                }

                if (rx2 || tx2)
                {
                    enabledTransceivers.insert(comUnitToTransceiverId(CComSystem::Com2));
                }

                {
                    QMutexLocker lock(&m_mutexTransceivers);
                    m_enabledTransceivers = enabledTransceivers;
                }

                // force update
                this->onTimerUpdate();
            }

            void CAfvClient::getRxTx(bool &rx1, bool &tx1, bool &rx2, bool &tx2) const
            {
                rx1 = false;
                rx2 = false;
                tx1 = false;
                tx2 = false;

                const QSet<quint16> enabled = getEnabledTransceivers();
                rx1 = enabled.contains(comUnitToTransceiverId(CComSystem::Com1));
                rx2 = enabled.contains(comUnitToTransceiverId(CComSystem::Com2));

                const QVector<TxTransceiverDto> transmits = getTransmittingTransceivers();
                for (const TxTransceiverDto &dto : transmits)
                {
                    if (dto.id == comUnitToTransceiverId(CComSystem::Com1)) { tx1 = true; }
                    if (dto.id == comUnitToTransceiverId(CComSystem::Com2)) { tx2 = true; }
                }
            }

            QVector<TransceiverDto> CAfvClient::getTransceivers() const
            {
                QMutexLocker lock(&m_mutexTransceivers);
                return m_transceivers;
            }

            QSet<quint16> CAfvClient::getEnabledTransceivers() const
            {
                QMutexLocker lock(&m_mutexTransceivers);
                return m_enabledTransceivers;
            }

            QVector<TxTransceiverDto> CAfvClient::getTransmittingTransceivers() const
            {
                QMutexLocker lock(&m_mutexTransceivers);
                return m_transmittingTransceivers;
            }

            void CAfvClient::setPtt(bool active)
            {
                this->setPttForCom(active, COMUnspecified);
            }

            void CAfvClient::setPttForCom(bool active, PTTCOM com)
            {
                Q_UNUSED(com)

                if (!m_isStarted)
                {
                    CLogMessage(this).info(u"Voice client not started");
                    return;
                }

                if (m_transmit == active) { return; }
                m_transmit = active;

                // thread safe block
                {
                    QMutexLocker lock(&m_mutexSampleProviders);
                    if (m_soundcardSampleProvider)
                    {
                        m_soundcardSampleProvider->pttUpdate(active, m_transmittingTransceivers);
                    }

                    /** TODO: RR 2019-10 as discussed https://discordapp.com/channels/539048679160676382/623947987822837779/633320595978846208
                     *  disabled for the moment as not needed
                    if (!active)
                    {
                        // AGC
                        // if (maxDbReadingInPTTInterval > -1) InputVolumeDb = InputVolumeDb - 1;
                        // if (maxDbReadingInPTTInterval < -4) InputVolumeDb = InputVolumeDb + 1;
                        m_maxDbReadingInPTTInterval = -100;
                    }
                    **/
                }

                emit this->ptt(active, com, this->identifier());
            }

            double CAfvClient::getInputVolumeDb() const
            {
                QMutexLocker lock(&m_mutex);
                return m_inputVolumeDb;
            }

            bool CAfvClient::setInputVolumeDb(double valueDb)
            {
                if (!CThreadUtils::isCurrentThreadObjectThread(this))
                {
                    // call in background thread of AFVClient to avoid lock issues
                    QPointer<CAfvClient> myself(this);
                    QTimer::singleShot(0, this, [ = ]
                    {
                        if (!myself || !CAfvClient::hasContexts()) { return; }
                        myself->setInputVolumeDb(valueDb);
                    });
                    return true; // not exactly "true" as we do it async
                }

                if (valueDb > MaxDbIn)      { valueDb = MaxDbIn; }
                else if (valueDb < MinDbIn) { valueDb = MinDbIn; }

                QMutexLocker lock(&m_mutex);
                bool changed = !qFuzzyCompare(m_inputVolumeDb, valueDb);
                if (changed)
                {
                    m_inputVolumeDb = valueDb;
                    if (m_input)
                    {
                        const double gainRatio = qPow(10, valueDb / 20.0);
                        changed = m_input->setGainRatio(gainRatio);
                    }
                }
                return changed;
            }

            double CAfvClient::getOutputVolumeDb() const
            {
                QMutexLocker lock(&m_mutexVolume);
                return m_outputVolumeDb;
            }

            double CAfvClient::getOutputGainRatio() const
            {
                QMutexLocker lock(&m_mutexVolume);
                return m_outputGainRatio;
            }

            int CAfvClient::getNormalizedInputVolume() const
            {
                const double db = this->getInputVolumeDb();
                const double range = MaxDbIn - MinDbIn;
                const int i = qRound((db - MinDbIn) / range * 100);
                return i;
            }

            int CAfvClient::getNormalizedOutputVolume() const
            {
                double db = this->getOutputVolumeDb();
                double range = MaxDbOut;
                int v = 50;
                if (db < 0)
                {
                    v = 0;
                    db -= MinDbOut;
                    range = qAbs(MinDbOut);
                }
                v += qRound(db * 50 / range);
                return v;
            }

            bool CAfvClient::setNormalizedInputVolume(int volume)
            {
                if (volume < 0) { volume = 0; }
                else if (volume > 100) { volume = 100; }
                const double range = MaxDbIn - MinDbIn;
                const double dB = MinDbIn + (volume * range / 100.0);

                // converted to MinDbIn-MaxDbIn
                return this->setInputVolumeDb(dB);
            }

            void CAfvClient::setNormalizedOutputVolume(int volume)
            {
                if (volume < 0) { volume = 0; }
                else if (volume > 100) { volume = 100; }

                // Asymetric
                double range = MaxDbOut;
                double dB = 0;
                if (volume >= 50)
                {
                    volume -= 50;
                }
                else
                {
                    dB = MinDbOut;
                    range = qAbs(MinDbOut);
                }
                dB += (volume * range / 50.0);

                // converted to MinDbOut-MaxDbOut
                this->setOutputVolumeDb(dB);
            }

            double CAfvClient::getInputVolumePeakVU() const
            {
                QMutexLocker lock(&m_mutexInputStream);
                return m_inputVolumeStream.PeakVU;
            }

            double CAfvClient::getOutputVolumePeakVU() const
            {
                QMutexLocker lock(&m_mutexOutputStream);
                return m_outputVolumeStream.PeakVU;
            }

            void CAfvClient::opusDataAvailable(const OpusDataAvailableArgs &args)
            {
                const bool transmit = m_transmit;
                const bool loopback = m_loopbackOn;
                const bool transmitHistory = m_transmitHistory; // threadsafe
                const auto transceivers = this->getTransceivers();

                if (loopback && transmit)
                {
                    IAudioDto audioData;
                    audioData.audio = QByteArray(args.audio.data(), args.audio.size());
                    audioData.callsign = QStringLiteral("loopback");
                    audioData.lastPacket = false;
                    audioData.sequenceCounter = 0;

                    const RxTransceiverDto com1 = { 0, transceivers.size() > 0 ?  transceivers[0].frequencyHz : UniCom, 1.0 };
                    const RxTransceiverDto com2 = { 1, transceivers.size() > 1 ?  transceivers[1].frequencyHz : UniCom, 1.0 };

                    QMutexLocker lock(&m_mutexSampleProviders);
                    m_soundcardSampleProvider->addOpusSamples(audioData, { com1, com2 });
                    return;
                }

                if (!this->isConnected()) { return; } // threadsafe

                const QString callsign = this->getCallsign(); // threadsafe
                const auto transmittingTransceivers = this->getTransmittingTransceivers(); // threadsafe
                if (transmittingTransceivers.size() > 0)
                {
                    if (transmit)
                    {
                        AudioTxOnTransceiversDto dto;
                        dto.callsign = callsign.toStdString();
                        dto.sequenceCounter = args.sequenceCounter;
                        dto.audio = std::vector<char>(args.audio.begin(), args.audio.end());
                        dto.lastPacket = false;
                        dto.transceivers = std::vector<TxTransceiverDto>(transmittingTransceivers.begin(), transmittingTransceivers.end());
                        QMutexLocker lock(&m_mutexConnection);
                        m_connection->sendToVoiceServer(dto);
                    }

                    if (!transmit && transmitHistory)
                    {
                        AudioTxOnTransceiversDto dto;
                        dto.callsign = callsign.toStdString();
                        dto.sequenceCounter = args.sequenceCounter;
                        dto.audio = std::vector<char>(args.audio.begin(), args.audio.end());
                        dto.lastPacket = true;
                        dto.transceivers = std::vector<TxTransceiverDto>(transmittingTransceivers.begin(), transmittingTransceivers.end());
                        QMutexLocker lock(&m_mutexConnection);
                        m_connection->sendToVoiceServer(dto);
                    }
                    m_transmitHistory = transmit; // threadsafe
                }
            }

            void CAfvClient::audioOutDataAvailable(const AudioRxOnTransceiversDto &dto)
            {
                IAudioDto audioData;
                audioData.audio           = QByteArray(dto.audio.data(), static_cast<int>(dto.audio.size()));
                audioData.callsign        = QString::fromStdString(dto.callsign);
                audioData.lastPacket      = dto.lastPacket;
                audioData.sequenceCounter = dto.sequenceCounter;

                QMutexLocker lock(&m_mutexSampleProviders);
                m_soundcardSampleProvider->addOpusSamples(audioData, QVector<RxTransceiverDto>(dto.transceivers.begin(), dto.transceivers.end()));
            }

            void CAfvClient::inputVolumeStream(const InputVolumeStreamArgs &args)
            {
                // thread safe block
                {
                    QMutexLocker lock(&m_mutexInputStream);
                    m_inputVolumeStream = args;
                }
                emit inputVolumePeakVU(args.PeakVU);
            }

            void CAfvClient::outputVolumeStream(const OutputVolumeStreamArgs &args)
            {
                // thread safe block
                {
                    QMutexLocker lock(&m_mutexOutputStream);
                    m_outputVolumeStream = args;
                }
                emit outputVolumePeakVU(args.PeakVU);
            }

            QString CAfvClient::getReceivingCallsignsStringCom1() const
            {
                QMutexLocker lock(&m_mutex);
                if (!m_soundcardSampleProvider) return {};
                return m_soundcardSampleProvider->getReceivingCallsignsString(comUnitToTransceiverId(CComSystem::Com1));
            }

            QString CAfvClient::getReceivingCallsignsStringCom2() const
            {
                QMutexLocker lock(&m_mutexSampleProviders);
                if (!m_soundcardSampleProvider) return {};
                return m_soundcardSampleProvider->getReceivingCallsignsString(comUnitToTransceiverId(CComSystem::Com2));
            }

            CCallsignSet CAfvClient::getReceivingCallsignsCom1() const
            {
                QMutexLocker lock(&m_mutexSampleProviders);
                if (!m_soundcardSampleProvider) return {};
                return m_soundcardSampleProvider->getReceivingCallsigns(comUnitToTransceiverId(CComSystem::Com1));
            }

            CCallsignSet CAfvClient::getReceivingCallsignsCom2() const
            {
                QMutexLocker lock(&m_mutexSampleProviders);
                if (!m_soundcardSampleProvider) return {};
                return m_soundcardSampleProvider->getReceivingCallsigns(comUnitToTransceiverId(CComSystem::Com2));
            }

            QStringList CAfvClient::getReceivingCallsignsStringCom1Com2() const
            {
                QStringList coms;
                QMutexLocker lock(&m_mutexSampleProviders);
                if (!m_soundcardSampleProvider) { return {{ QString(), QString() }}; }
                coms << m_soundcardSampleProvider->getReceivingCallsignsString(comUnitToTransceiverId(CComSystem::Com1));
                coms << m_soundcardSampleProvider->getReceivingCallsignsString(comUnitToTransceiverId(CComSystem::Com2));
                return coms;
            }

            bool CAfvClient::updateVoiceServerUrl(const QString &url)
            {
                QMutexLocker lock(&m_mutexConnection);
                if (!m_connection) { return false; }
                return m_connection->updateVoiceServerUrl(url);
            }

            void CAfvClient::gracefulShutdown()
            {
                this->stopAudio();
                this->disconnectFrom();
                this->quitAndWait();
                Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Needs to be back in current thread");
            }

            void CAfvClient::initialize()
            {
#ifdef Q_OS_WIN
                if (!m_winCoInitialized)
                {
                    HRESULT hr = CoInitializeEx(nullptr,  COINIT_MULTITHREADED);

                    // RPC_E_CHANGED_MODE: CoInitializeEx was already called by someone else in this thread with a different mode.
                    if (hr == RPC_E_CHANGED_MODE)
                    {
                        CLogMessage(this).debug(u"CoInitializeEx was already called with a different mode. Trying again.");
                        hr = CoInitializeEx(nullptr,  COINIT_APARTMENTTHREADED);
                    }

                    // S_OK: The COM library was initialized successfully on this thread.
                    // S_FALSE: The COM library is already initialized on this thread. Reference count was incremented. This is not an error.
                    if (hr == S_OK || hr == S_FALSE) { m_winCoInitialized = true; }
                }
#endif
                CLogMessage(this).info(u"Initialize AFV client in thread: %1") << CThreadUtils::threadInfo(this->thread());
            }

            void CAfvClient::cleanup()
            {
#ifdef Q_OS_WIN
                if (m_winCoInitialized)
                {
                    CoUninitialize();
                    m_winCoInitialized = false;
                }
#endif
            }

            void CAfvClient::onTimerUpdate()
            {
                if (hasContexts())
                {
                    // for pilot client
                    const CSimulatedAircraft aircraft = sApp->getIContextOwnAircraft()->getOwnAircraft();
                    this->updateFromOwnAircraft(aircraft, false);

                    // disconnect if NOT connected
                    this->autoLogoffWithoutFsdNetwork();

                    // get the settings in correct thread
                    this->fetchSimulatorSettings();
                }
                else
                {
                    // for AFV sample client
                    this->updateTransceivers();
                }
            }

            void CAfvClient::onSettingsChanged()
            {
                const CSettings audioSettings = m_audioSettings.get();
                const int iv = audioSettings.getInVolume();
                const int ov = audioSettings.getOutVolume();

                this->setNormalizedInputVolume(iv);
                this->setNormalizedOutputVolume(ov);
                this->setBypassEffects(!audioSettings.isAudioEffectsEnabled());
            }

            void CAfvClient::autoLogoffWithoutFsdNetwork()
            {
                if (!hasContexts())       { return; }
                if (!this->isConnected()) { m_fsdConnectMismatches = 0; return; }

                // AFV is connected
                if (sApp->getIContextNetwork()->isConnected()) { m_fsdConnectMismatches = 0; return; }
                if (++m_fsdConnectMismatches < 2) { return; } // avoid a single issue causing logoff

                CLogMessage(this).warning(u"Auto logoff AFV client because FSD no longer connected");
                this->disconnectFrom();
            }

            void CAfvClient::updateFromOwnAircraft(const CSimulatedAircraft &aircraft, bool withSignals)
            {
                if (!sApp || sApp->isShuttingDown()) { return; }

                TransceiverDto transceiverCom1;
                TransceiverDto transceiverCom2;
                transceiverCom1.id = comUnitToTransceiverId(CComSystem::Com1);
                transceiverCom2.id = comUnitToTransceiverId(CComSystem::Com2);

                // position
                const double latDeg = aircraft.latitude().value(CAngleUnit::deg());
                const double lngDeg = aircraft.longitude().value(CAngleUnit::deg());
                const double altM   = aircraft.getAltitude().value(CLengthUnit::m());

                transceiverCom1.LatDeg     = transceiverCom2.LatDeg     = latDeg;
                transceiverCom1.LonDeg     = transceiverCom2.LonDeg     = lngDeg;
                transceiverCom1.HeightAglM = transceiverCom2.HeightAglM = altM;
                transceiverCom1.HeightMslM = transceiverCom2.HeightMslM = altM;

                // enabled, rx/tx, frequency
                const CComSystem com1 = aircraft.getCom1System();
                const CComSystem com2 = aircraft.getCom2System();
                const quint32 f1 = static_cast<quint32>(com1.getFrequencyActive().valueInteger(CFrequencyUnit::Hz()));
                const quint32 f2 = static_cast<quint32>(com2.getFrequencyActive().valueInteger(CFrequencyUnit::Hz()));

                transceiverCom1.frequencyHz = this->getAliasFrequencyHz(f1);
                transceiverCom2.frequencyHz = this->getAliasFrequencyHz(f2);

                QVector<TransceiverDto> newEnabledTransceivers;
                if (m_integratedComUnit)
                {
                    const bool tx1 = com1.isTransmitEnabled();
                    const bool rx1 = com1.isReceiveEnabled();
                    const bool tx2 = com2.isTransmitEnabled(); // we only allow one (1) transmit
                    const bool rx2 = com2.isReceiveEnabled();

                    // enable, we currently treat receive as enable
                    // flight sim cockpits normally use rx and tx
                    // AFV uses tx and enable
                    const bool e1 = rx1;
                    const bool e2 = rx2;

                    // transceivers
                    const QVector<TransceiverDto> newTransceivers { transceiverCom1, transceiverCom2 };
                    QSet<quint16> newEnabledTransceiverIds;
                    QVector<TxTransceiverDto> newTransmittingTransceivers;
                    if (e1) { newEnabledTransceivers.push_back(transceiverCom1); newEnabledTransceiverIds.insert(transceiverCom1.id); }
                    if (e2) { newEnabledTransceivers.push_back(transceiverCom2); newEnabledTransceiverIds.insert(transceiverCom2.id); }

                    // Transmitting transceivers, currently ALLOW ONLY ONE
                    if (tx1 && e1) { newTransmittingTransceivers.push_back(transceiverCom1); }
                    else if (tx2 && e2) { newTransmittingTransceivers.push_back(transceiverCom2); }

                    // lock and update
                    {
                        QMutexLocker lock(&m_mutexTransceivers);
                        m_transceivers = newTransceivers;
                        m_enabledTransceivers = newEnabledTransceiverIds;
                        m_transmittingTransceivers = newTransmittingTransceivers;
                    }
                }
                else
                {
                    // update position and frequencies, but keep enabled as it was
                    const QSet<quint16> ids = getEnabledTransceivers();
                    if (ids.contains(comUnitToTransceiverId(CComSystem::Com1)))
                    {
                        newEnabledTransceivers.push_back(transceiverCom1);
                    }

                    if (ids.contains(comUnitToTransceiverId(CComSystem::Com2)))
                    {
                        newEnabledTransceivers.push_back(transceiverCom2);
                    }
                }

                // in connection and soundcard only use the enabled tarnsceivers
                const QString callsign = this->getCallsign(); // threadsafe
                {
                    {
                        QMutexLocker lock(&m_mutexConnection);
                        if (m_connection)
                        {
                            // fire to network and forget
                            m_connection->updateTransceivers(callsign, newEnabledTransceivers);
                        }
                    }

                    {
                        QMutexLocker lock(&m_mutexSampleProviders);
                        if (m_soundcardSampleProvider) { m_soundcardSampleProvider->updateRadioTransceivers(newEnabledTransceivers); }
                    }
                }

                if (withSignals) { emit this->updatedFromOwnAircraftCockpit(); }
            }

            void CAfvClient::onUpdateTransceiversFromContext(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
            {
                if (originator == this->identifier()) { return; }
                this->updateFromOwnAircraft(aircraft);
            }

            void CAfvClient::onReceivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args)
            {
                const CComSystem::ComUnit unit = transceiverIdToComUnit(args.transceiverID);
                CCallsignSet callsignsCom1;
                CCallsignSet callsignsCom2;
                switch (unit)
                {
                case CComSystem::Com1:
                default:
                    callsignsCom1 = CCallsignSet(args.receivingCallsigns);
                    callsignsCom2 = this->getReceivingCallsignsCom2();
                    break;

                case CComSystem::Com2:
                    callsignsCom2 = CCallsignSet(args.receivingCallsigns);
                    callsignsCom1 = this->getReceivingCallsignsCom1();
                    break;
                }

                emit this->receivedCallsignsChanged(callsignsCom1, callsignsCom2);
                emit this->receivingCallsignsChanged(args);
            }

            void CAfvClient::retryConnectTo(const QString &cid, const QString &password, const QString &callsign, const QString &client, const QString &reason)
            {
                if (this->isConnected()) { return; }
                m_retryConnectAttempt++;

                const int retrySecs = qMin(3 * 60, m_retryConnectAttempt * 30);
                const CStatusMessage msg = CStatusMessage(this).validationError(reason + ". Retry in %1secs. Attempt %2.") << retrySecs << m_retryConnectAttempt;
                this->reconnectTo(cid, password, callsign, client, retrySecs * 1000, msg);
            }

            void CAfvClient::reconnectTo(const QString &cid, const QString &password, const QString &callsign, const QString &client, int delayMs, const CStatusMessage &msg)
            {
                if (msg.isFailure())
                {
                    emit this->afvConnectionFailure(msg);
                }

                QPointer<CAfvClient> myself(this);
                QTimer::singleShot(delayMs, this, [ = ]
                {
                    if (!myself) { return; }
                    if (myself->isConnected()) { return; }
                    this->connectTo(cid, password, callsign, client);
                });
            }

            QVector<StationDto> CAfvClient::getAliasedStations() const
            {
                QMutexLocker lock(&m_mutex);
                return m_aliasedStations;
            }

            void CAfvClient::setAliasedStations(const QVector<StationDto> &stations)
            {
                QMutexLocker lock(&m_mutex);
                m_aliasedStations = stations;
            }

            quint32 CAfvClient::getAliasFrequencyHz(quint32 frequencyHz) const
            {
                // void rounding issues from float/double
                quint32 roundedFrequencyHz = static_cast<quint32>(qRound(frequencyHz / 1000.0)) * 1000;

                // disabled?
                if (!m_enableAliased) { return roundedFrequencyHz; }

                // change to aliased frequency if needed
                {
                    QMutexLocker lock(&m_mutex);
                    const auto it = std::find_if(m_aliasedStations.constBegin(), m_aliasedStations.constEnd(), [roundedFrequencyHz](const StationDto & d)
                    {
                        return d.frequencyAliasHz == roundedFrequencyHz;
                    });

                    if (it != m_aliasedStations.constEnd())
                    {
                        if (sApp && sApp->getIContextNetwork())
                        {
                            // Get the callsign for this frequency and fuzzy compare with our alias station
                            // !\todo KB 2019-10 replace by COM unit channel spacing
                            const CComSystem::ChannelSpacing spacing = CComSystem::ChannelSpacing25KHz;
                            const CFrequency f(static_cast<int>(roundedFrequencyHz), CFrequencyUnit::Hz());
                            const CAtcStationList matchingAtcStations = sApp->getIContextNetwork()->getOnlineStationsForFrequency(f, spacing);
                            const CAtcStation closest = matchingAtcStations.findClosest(1, sApp->getIContextOwnAircraft()->getOwnAircraftSituation().getPosition()).frontOrDefault();

                            if (fuzzyMatchCallsign(it->name, closest.getCallsign().asString()))
                            {
                                // this is how it should be
                                roundedFrequencyHz = it->frequencyHz;
                                CLogMessage(this).debug(u"Aliasing '%1' %2Hz [VHF] to %3Hz [HF]")  << closest.getCallsign() << frequencyHz << it->frequencyHz;
                            }
                            else
                            {
                                // Ups!
                                CLogMessage(this).debug(u"Station '%1' NOT found! Using original frequency %2Hz")  << it->name << roundedFrequencyHz;
                            }
                        }
                        else
                        {
                            // without contexts always use HF frequency if found
                            roundedFrequencyHz = it->frequencyHz; // we use this frequency
                            CLogMessage(this).debug(u"Aliasing %1Hz [VHF] to %2Hz [HF] (no context)")  << frequencyHz << it->frequencyHz;
                        }
                    }
                }
                return roundedFrequencyHz;
            }

            bool CAfvClient::fuzzyMatchCallsign(const QString &callsign, const QString &compareTo) const
            {
                if (callsign.isEmpty() || compareTo.isEmpty()) { return false; } // empty callsigns should NOT match

                QString prefixA;
                QString suffixA;
                QString prefixB;
                QString suffixB;
                this->getPrefixSuffix(callsign, prefixA, suffixA);
                this->getPrefixSuffix(compareTo, prefixB, suffixB);
                return (prefixA == prefixB) && (suffixA == suffixB);
            }

            void CAfvClient::getPrefixSuffix(const QString &callsign, QString &prefix, QString &suffix) const
            {
                thread_local const QRegularExpression separator("[(\\-|_)]");
                const QStringList parts = callsign.split(separator);

                // avoid issues if there are no parts, or only one
                prefix = parts.size() > 0 ? parts.first() : QString();
                suffix = parts.size() > 1 ? parts.last() :  QString();
            }

            quint16 CAfvClient::comUnitToTransceiverId(CComSystem::ComUnit comUnit)
            {
                switch (comUnit)
                {
                case CComSystem::Com1: return 0;
                case CComSystem::Com2: return 1;
                default:               break;
                }
                return 0;
            }

            CComSystem::ComUnit CAfvClient::transceiverIdToComUnit(quint16 id)
            {
                if (comUnitToTransceiverId(CComSystem::Com1) == id) { return CComSystem::Com1; }
                if (comUnitToTransceiverId(CComSystem::Com2) == id) { return CComSystem::Com2; }
                return CComSystem::Com1;
            }

            void CAfvClient::deferredInit()
            {
                // transceivers
                this->initTransceivers();

                // init by settings
                this->onSettingsChanged();

                // info
                CLogMessage(this).info(u"UserClient instantiated (deferred init)");
            }

            bool CAfvClient::hasContexts()
            {
                return sApp && !sApp->isShuttingDown() && sApp->getIContextOwnAircraft() && sApp->getIContextNetwork() && sApp->getIContextSimulator();
            }

            bool CAfvClient::setOutputVolumeDb(double valueDb)
            {
                if (!CThreadUtils::isCurrentThreadObjectThread(this))
                {
                    // call in background thread of AFVClient to avoid lock issues
                    QPointer<CAfvClient> myself(this);
                    QTimer::singleShot(0, this, [ = ]
                    {
                        if (!myself || !CAfvClient::hasContexts()) { return; }
                        myself->setOutputVolumeDb(valueDb);
                    });
                    return true; // not exactly "true" as we do it async
                }

                if (valueDb > MaxDbOut)      { valueDb = MaxDbOut; }
                else if (valueDb < MinDbOut) { valueDb = MinDbOut; }

                const double gainRatio = qPow(10, valueDb / 20.0);
                bool changed = false;
                {
                    QMutexLocker lock(&m_mutexVolume);
                    changed = !qFuzzyCompare(m_outputVolumeDb, valueDb);
                    if (changed)
                    {
                        m_outputVolumeDb  = valueDb;
                        m_outputGainRatio = gainRatio;
                    }
                }

                // do NOT check on "changed", can be false, but "m_outputSampleProvider" is initialized
                // HINT: I do this tryLock here because I had deadlocks here, and I need to further investigate
                // As deadlocks mean (for the user) he needs to terminate the client I keep "trylock" that for now
                if (!m_mutexSampleProviders.tryLock(1000))
                {
                    return false;
                }

                if (m_outputSampleProvider)
                {
                    changed = m_outputSampleProvider->setGainRatio(gainRatio);
                }
                m_mutexSampleProviders.unlock();
                return changed;
            }

            const CAudioDeviceInfo &CAfvClient::getInputDevice() const
            {
                QMutexLocker lock(&m_mutex);
                if (m_input) { return m_input->device(); }
                static const CAudioDeviceInfo nullDevice;
                return nullDevice;
            }

            const CAudioDeviceInfo &CAfvClient::getOutputDevice() const
            {
                QMutexLocker lock(&m_mutex);
                if (m_output) { return m_output->device(); }
                static const CAudioDeviceInfo nullDevice;
                return nullDevice;
            }

            bool CAfvClient::usesSameDevices(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
            {
                QMutexLocker lock(&m_mutex);
                if (!m_output || !m_input) { return false; }
                const CAudioDeviceInfo i = m_input->device();
                const CAudioDeviceInfo o = m_output->device();
                lock.unlock();

                return i.matchesNameTypeMachineName(inputDevice) &&
                       o.matchesNameTypeMachineName(outputDevice);
            }

            CAfvClient::ConnectionStatus CAfvClient::getConnectionStatus() const
            {
                return this->isConnected() ? Connected : Disconnected;
            }
        } // ns
    } // ns
} // ns
