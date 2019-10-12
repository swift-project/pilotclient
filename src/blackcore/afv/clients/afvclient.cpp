/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "afvclient.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/application.h"
#include "blacksound/audioutilities.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/threadutils.h"

#ifdef _WIN32
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
            const CLogCategoryList &CAfvClient::getLogCategories()
            {
                static const CLogCategoryList cats { CLogCategory::audio(), CLogCategory::vatsimSpecific() };
                return cats;
            }

            CAfvClient::CAfvClient(const QString &apiServer, QObject *owner) :
                CContinuousWorker(owner, "CAfvClient"),
                m_connection(new CClientConnection(apiServer, this)),
                m_input(new CInput(SampleRate, this)),
                m_output(new Output(this)),
                m_voiceServerPositionTimer(new QTimer(this))
            {
                this->setObjectName("AFV client");
                m_connection->setReceiveAudio(false);

                connect(m_input, &CInput::opusDataAvailable, this, &CAfvClient::opusDataAvailable);
                connect(m_input, &CInput::inputVolumeStream, this, &CAfvClient::inputVolumeStream);

                connect(m_output,     &Output::outputVolumeStream,       this, &CAfvClient::outputVolumeStream);
                connect(m_connection, &CClientConnection::audioReceived, this, &CAfvClient::audioOutDataAvailable);
                connect(m_voiceServerPositionTimer, &QTimer::timeout,    this, &CAfvClient::onPositionUpdateTimer);

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
                this->onPositionUpdateTimer();
            }

            void CAfvClient::connectWithContexts()
            {
                if (m_connectedWithContext) { return; }
                if (!hasContext()) { return; }
                this->disconnect(sApp->getIContextOwnAircraft());
                sApp->getIContextOwnAircraft()->disconnect(this);
                connect(sApp->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CAfvClient::onUpdateTransceiversFromContext, Qt::QueuedConnection);
                m_connectedWithContext = true;
            }

            void CAfvClient::connectTo(const QString &cid, const QString &password, const QString &callsign)
            {
                if (QThread::currentThread() != thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(this, [ = ]() { if (myself) { connectTo(cid, password, callsign); }});
                    return;
                }

                // called in CAfvClient thread
                this->connectWithContexts();
                this->setCallsign(callsign);

                m_connection->connectTo(cid, password, callsign);
                m_aliasedStations = m_connection->getAllAliasedStations();

                this->onPositionUpdateTimer();

                if (m_connection->isConnected())
                {
                    // restart timer, normally it should be started already, paranoia
                    if (m_voiceServerPositionTimer) { m_voiceServerPositionTimer->start(PositionUpdatesMs); }
                    emit this->connectionStatusChanged(Connected);
                }
                else
                {
                    emit this->connectionStatusChanged(Disconnected);
                }
            }

            void CAfvClient::disconnectFrom()
            {
                if (QThread::currentThread() != thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(this, [ = ]() { if (myself) disconnectFrom(); });
                    return;
                }

                // we intentionally DO NOT STOP the timer here, but keep it for preset (own aircraft pos.)
                m_connection->disconnectFrom();
                emit connectionStatusChanged(Disconnected);
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
                QMutexLocker lock(&m_mutex);
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

            void CAfvClient::restartWithNewDevices(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
            {
                if (QThread::currentThread() != this->thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QPointer<CAfvClient> myself(this);
                    QMetaObject::invokeMethod(this, [ = ]() { if (myself) restartWithNewDevices(inputDevice, outputDevice); });
                }

                this->stopAudio();
                this->startAudio(inputDevice, outputDevice, allTransceiverIds());
            }

            void CAfvClient::startAudio(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice, const QVector<quint16> &transceiverIDs)
            {
                if (QThread::currentThread() != this->thread())
                {
                    // Method needs to be executed in the object thread since it will create new QObject children
                    QMetaObject::invokeMethod(this, [ = ]() { startAudio(inputDevice, outputDevice, transceiverIDs); });
                    return;
                }

                if (m_isStarted)
                {
                    CLogMessage(this).info(u"Client already started");
                    return;
                }

                this->initTransceivers();

                if (m_soundcardSampleProvider) { m_soundcardSampleProvider->deleteLater(); }
                m_soundcardSampleProvider = new CSoundcardSampleProvider(SampleRate, transceiverIDs, this);
                connect(m_soundcardSampleProvider, &CSoundcardSampleProvider::receivingCallsignsChanged, this, &CAfvClient::receivingCallsignsChanged);

                if (m_outputSampleProvider) { m_outputSampleProvider->deleteLater(); }
                m_outputSampleProvider = new CVolumeSampleProvider(m_soundcardSampleProvider, this);
                m_outputSampleProvider->setVolume(m_outputVolume);

                m_output->start(outputDevice, m_outputSampleProvider);
                m_input->start(inputDevice);

                m_startDateTimeUtc = QDateTime::currentDateTimeUtc();
                m_connection->setReceiveAudio(true);
                m_voiceServerPositionTimer->start(PositionUpdatesMs); // start for preset values

                this->onSettingsChanged(); // make sure all settings are applied
                m_isStarted = true;
                CLogMessage(this).info(u"Started [Input: %1] [Output: %2]") << inputDevice.getName() << outputDevice.getName();

                this->onPositionUpdateTimer(); // update values
            }

            void CAfvClient::startAudio(const QString &inputDeviceName, const QString &outputDeviceName)
            {
                const CAudioDeviceInfo i(CAudioDeviceInfo::InputDevice, inputDeviceName);
                const CAudioDeviceInfo o(CAudioDeviceInfo::OutputDevice, outputDeviceName);
                this->startAudio(i, o, allTransceiverIds());
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
                    CLogMessage(this).info(u"Client NOT started");
                    return;
                }

                m_isStarted = false;
                m_connection->setReceiveAudio(false);

                // stop input/output
                m_input->stop();
                m_output->stop();
                CLogMessage(this).info(u"Client stopped");
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

                if (hasContext())
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
                const QString callsign = this->getCallsign();

                // transceivers
                QVector<TransceiverDto> newEnabledTransceivers;
                for (const TransceiverDto &transceiver : transceivers)
                {
                    if (enabledTransceivers.contains(transceiver.id))
                    {
                        newEnabledTransceivers.push_back(transceiver);
                    }
                }

                // in connection and soundcard only use the enabled tarnsceivers
                QMutexLocker lock(&m_mutex);
                if (m_connection) { m_connection->updateTransceivers(callsign, newEnabledTransceivers); }
                if (m_soundcardSampleProvider) { m_soundcardSampleProvider->updateRadioTransceivers(newEnabledTransceivers); }
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

                {
                    QMutexLocker lock(&m_mutex);
                    if (m_soundcardSampleProvider)
                    {
                        m_soundcardSampleProvider->pttUpdate(active, m_transmittingTransceivers);
                    }

                    if (!active)
                    {
                        // AGC
                        // if (maxDbReadingInPTTInterval > -1) InputVolumeDb = InputVolumeDb - 1;
                        // if (maxDbReadingInPTTInterval < -4) InputVolumeDb = InputVolumeDb + 1;
                        m_maxDbReadingInPTTInterval = -100;
                    }
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
                if (valueDb > MaxDbIn) { valueDb = MaxDbIn; }
                else if (valueDb < MinDbIn) { valueDb = MinDbIn; }

                QMutexLocker lock(&m_mutex);
                bool changed = !qFuzzyCompare(m_inputVolumeDb, valueDb);
                m_inputVolumeDb = valueDb;
                if (m_input)
                {
                    changed = m_input->setVolume(qPow(10, valueDb / 20.0));
                }
                return changed;
            }

            double CAfvClient::getOutputVolumeDb() const
            {
                QMutexLocker lock(&m_mutex);
                return m_outputVolumeDb;
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
                const bool transmitHistory = m_transmitHistory;
                const auto transceivers = this->getTransceivers();

                if (loopback && transmit)
                {
                    IAudioDto audioData;
                    audioData.audio = QByteArray(args.audio.data(), args.audio.size());
                    audioData.callsign = "loopback";
                    audioData.lastPacket = false;
                    audioData.sequenceCounter = 0;

                    const RxTransceiverDto com1 = { 0, transceivers.size() > 0 ?  transceivers[0].frequencyHz : UniCom, 1.0 };
                    const RxTransceiverDto com2 = { 1, transceivers.size() > 1 ?  transceivers[1].frequencyHz : UniCom, 1.0 };

                    QMutexLocker lock(&m_mutex);
                    m_soundcardSampleProvider->addOpusSamples(audioData, { com1, com2 });
                    return;
                }

                {
                    QMutexLocker lock(&m_mutex);
                    if (!m_connection->isConnected()) { return; }
                }

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
                        dto.transceivers = transmittingTransceivers.toStdVector();
                        QMutexLocker lock(&m_mutex);
                        m_connection->sendToVoiceServer(dto);
                    }

                    if (!transmit && transmitHistory)
                    {
                        AudioTxOnTransceiversDto dto;
                        dto.callsign = callsign.toStdString();
                        dto.sequenceCounter = args.sequenceCounter;
                        dto.audio = std::vector<char>(args.audio.begin(), args.audio.end());
                        dto.lastPacket = true;
                        dto.transceivers = transmittingTransceivers.toStdVector();
                        QMutexLocker lock(&m_mutex);
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
                m_soundcardSampleProvider->addOpusSamples(audioData, QVector<RxTransceiverDto>::fromStdVector(dto.transceivers));
            }

            void CAfvClient::inputVolumeStream(const InputVolumeStreamArgs &args)
            {
                {
                    QMutexLocker lock(&m_mutexInputStream);
                    m_inputVolumeStream = args;
                }
                emit inputVolumePeakVU(args.PeakVU);
            }

            void CAfvClient::outputVolumeStream(const OutputVolumeStreamArgs &args)
            {
                {
                    QMutexLocker lock(&m_mutexOutputStream);
                    m_outputVolumeStream = args;
                }
                emit outputVolumePeakVU(args.PeakVU);
            }

            QString CAfvClient::getReceivingCallsignsCom1()
            {
                QMutexLocker lock(&m_mutex);
                if (!m_soundcardSampleProvider) return {};
                return m_soundcardSampleProvider->getReceivingCallsigns(0);
            }

            QString CAfvClient::getReceivingCallsignsCom2()
            {
                QMutexLocker lock(&m_mutex);
                if (!m_soundcardSampleProvider) return {};
                return m_soundcardSampleProvider->getReceivingCallsigns(1);
            }

            bool CAfvClient::updateVoiceServerUrl(const QString &url)
            {
                QMutexLocker lock(&m_mutex);
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
#ifdef _WIN32
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
#ifdef _WIN32
                if (m_winCoInitialized)
                {
                    CoUninitialize();
                    m_winCoInitialized = false;
                }
#endif
            }

            void CAfvClient::onPositionUpdateTimer()
            {
                if (hasContext())
                {
                    // for pilot client
                    this->updateFromOwnAircraft(sApp->getIContextOwnAircraft()->getOwnAircraft(), false);
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
                this->setNormalizedInputVolume(audioSettings.getInVolume());
                this->setNormalizedOutputVolume(audioSettings.getOutVolume());
                this->setBypassEffects(!audioSettings.isAudioEffectsEnabled());
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
                QSet<quint16> newEnabledTransceiverIds;
                QVector<TransceiverDto> newTransceivers { transceiverCom1, transceiverCom2 };
                QVector<TransceiverDto> newEnabledTransceivers;
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
                // in connection and soundcard only use the enabled tarnsceivers
                const QString callsign = this->getCallsign();
                {
                    QMutexLocker lock(&m_mutex);
                    if (m_connection) { m_connection->updateTransceivers(callsign, newEnabledTransceivers); }
                    if (m_soundcardSampleProvider) { m_soundcardSampleProvider->updateRadioTransceivers(newEnabledTransceivers); }
                }

                if (withSignals) { emit this->updatedFromOwnAircraftCockpit(); }
            }

            void CAfvClient::onUpdateTransceiversFromContext(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
            {
                if (originator == this->identifier()) { return; }
                this->updateFromOwnAircraft(aircraft);
            }

            quint32 CAfvClient::getAliasFrequencyHz(quint32 frequencyHz) const
            {
                // void rounding issues from float/double
                quint32 roundedFrequencyHz = static_cast<quint32>(qRound(frequencyHz / 1000.0)) * 1000;

                // change to aliased frequency if needed
                {
                    QMutexLocker lock(&m_mutex);
                    auto it = std::find_if(m_aliasedStations.begin(), m_aliasedStations.end(), [roundedFrequencyHz](const StationDto & d)
                    {
                        return d.frequencyAliasHz == roundedFrequencyHz;
                    });

                    if (it != m_aliasedStations.end())
                    {
                        CLogMessage(this).debug(u"Aliasing %1Hz [VHF] to %2Hz [HF]")  << frequencyHz << it->frequencyHz;
                        roundedFrequencyHz = it->frequencyHz;
                    }
                }
                return roundedFrequencyHz;
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

            bool CAfvClient::hasContext()
            {
                return sApp && !sApp->isShuttingDown() && sApp->getIContextOwnAircraft();
            }

            bool CAfvClient::setOutputVolumeDb(double valueDb)
            {
                if (valueDb > MaxDbOut) { valueDb = MaxDbOut; }
                else if (valueDb < MinDbOut) { valueDb = MinDbOut; }

                QMutexLocker lock(&m_mutex);
                bool changed = !qFuzzyCompare(m_outputVolumeDb, valueDb);
                m_outputVolumeDb = valueDb;
                m_outputVolume   = qPow(10, m_outputVolumeDb / 20.0);

                if (m_outputSampleProvider)
                {
                    changed = m_outputSampleProvider->setVolume(m_outputVolume);
                }
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

                return i.matchesNameTypeHostName(inputDevice) &&
                       o.matchesNameTypeHostName(outputDevice);
            }

            CAfvClient::ConnectionStatus CAfvClient::getConnectionStatus() const
            {
                return m_connection->isConnected() ? Connected : Disconnected;
            }
        } // ns
    } // ns
} // ns
