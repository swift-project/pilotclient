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

            CAfvClient::CAfvClient(const QString &apiServer, QObject *parent) :
                QObject(parent), CIdentifiable(this)
            {
                m_connection = new CClientConnection(apiServer, this);
                m_connection->setReceiveAudio(false);

                m_input = new CInput(SampleRate, this);
                connect(m_input, &CInput::opusDataAvailable, this, &CAfvClient::opusDataAvailable);
                connect(m_input, &CInput::inputVolumeStream, this, &CAfvClient::inputVolumeStream);

                m_output = new Output(this);
                connect(m_output, &Output::outputVolumeStream, this, &CAfvClient::outputVolumeStream);
                connect(m_connection, &CClientConnection::audioReceived, this, &CAfvClient::audioOutDataAvailable);
                connect(&m_voiceServerPositionTimer, &QTimer::timeout, this, &CAfvClient::onPositionUpdateTimer);

                // transceivers
                this->initTransceivers();

                // init by settings
                this->onSettingsChanged();

                CLogMessage(this).info(u"UserClient instantiated");
            }

            void CAfvClient::initTransceivers()
            {
                m_transceivers =
                {
                    { 0, UniCom, 48.5, 11.5, 1000.0, 1000.0 },
                    { 1, UniCom, 48.5, 11.5, 1000.0, 1000.0 }
                };

                m_enabledTransceivers = { 0, 1 };
                m_transmittingTransceivers = { { 0 } }; // TxTransceiverDto

                // init with context values
                this->initWithContext();
            }

            void CAfvClient::initWithContext()
            {
                if (!hasContext()) { return; }
                this->disconnect(sApp->getIContextOwnAircraft());
                sApp->getIContextOwnAircraft()->disconnect(this);
                connect(sApp->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CAfvClient::updateTransceiversFromContext);
            }

            void CAfvClient::connectTo(const QString &cid, const QString &password, const QString &callsign)
            {
                this->initWithContext();
                m_callsign = callsign;
                m_connection->connectTo(cid, password, callsign);
                this->updateTransceivers(); // uses context if available

                if (m_connection->isConnected()) { emit this->connectionStatusChanged(Connected); }
                else { emit this->connectionStatusChanged(Disconnected); }

                m_aliasedStations = m_connection->getAllAliasedStations();
            }

            void CAfvClient::disconnectFrom()
            {
                m_connection->disconnectFrom();
                emit connectionStatusChanged(Disconnected);
            }

            QStringList CAfvClient::availableInputDevices() const
            {
                return CAudioDeviceInfoList::allQtInputDevices().getDeviceNames();
            }

            QStringList CAfvClient::availableOutputDevices() const
            {
                return CAudioDeviceInfoList::allQtOutputDevices().getDeviceNames();
            }

            void CAfvClient::setBypassEffects(bool value)
            {
                if (soundcardSampleProvider)
                {
                    soundcardSampleProvider->setBypassEffects(value);
                }
            }

            bool CAfvClient::isMuted() const
            {
                return !this->isStarted();
            }

            void CAfvClient::setMuted(bool mute)
            {
                Q_UNUSED(mute)
            }

            bool CAfvClient::restartWithNewDevices(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice)
            {
                this->stop();
                this->start(inputDevice, outputDevice, allTransceiverIds());
                return true;
            }

            void CAfvClient::start(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice, const QVector<quint16> &transceiverIDs)
            {
                if (m_isStarted)
                {
                    CLogMessage(this).info(u"Client already started");
                    return;
                }

                this->initTransceivers();

                soundcardSampleProvider = new CSoundcardSampleProvider(SampleRate, transceiverIDs, this);
                connect(soundcardSampleProvider, &CSoundcardSampleProvider::receivingCallsignsChanged, this, &CAfvClient::receivingCallsignsChanged);
                outputSampleProvider = new CVolumeSampleProvider(soundcardSampleProvider, this);
                outputSampleProvider->setVolume(m_outputVolume);

                m_output->start(outputDevice.isNull() ? QAudioDeviceInfo::defaultOutputDevice() : outputDevice, outputSampleProvider);
                m_input->start(inputDevice.isNull()   ? QAudioDeviceInfo::defaultInputDevice()  : inputDevice);

                m_startDateTimeUtc = QDateTime::currentDateTimeUtc();
                m_connection->setReceiveAudio(true);
                m_voiceServerPositionTimer.start(5000);
                this->onSettingsChanged(); // make sure all settings are applied
                m_isStarted = true;
                CLogMessage(this).info(u"Started [Input: %1] [Output: %2]") << inputDevice.deviceName() << outputDevice.deviceName();
            }

            void CAfvClient::start(const QString &inputDeviceName, const QString &outputDeviceName)
            {
                const QAudioDeviceInfo i = CAudioDeviceInfoList::allQtInputDevices().findByName(inputDeviceName).toAudioDeviceInfo();
                const QAudioDeviceInfo o = CAudioDeviceInfoList::allQtOutputDevices().findByName(outputDeviceName).toAudioDeviceInfo();
                this->start(i, o, allTransceiverIds());
            }

            void CAfvClient::stop()
            {
                if (!m_isStarted)
                {
                    CLogMessage(this).info(u"Client NOT started");
                    return;
                }

                m_isStarted = false;
                m_connection->setReceiveAudio(false);

                m_transceivers.clear();
                this->updateTransceivers(false);

                m_input->stop();
                m_output->stop();
                CLogMessage(this).info(u"Client stopped");
            }

            void CAfvClient::enableTransceiver(quint16 id, bool enable)
            {
                if (enable) { m_enabledTransceivers.insert(id); }
                else { m_enabledTransceivers.remove(id); }

                this->updateTransceivers();
            }

            void CAfvClient::enableComUnit(CComSystem::ComUnit comUnit, bool enable)
            {
                this->enableTransceiver(comUnitToTransceiverId(comUnit), enable);
            }

            bool CAfvClient::isEnabledTransceiver(quint16 id) const
            {
                // we double check, enabled and exist!
                if (!m_enabledTransceivers.contains(id)) { return false; }
                for (const TransceiverDto &dto : m_transceivers)
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

                auto it = std::find_if(m_aliasedStations.begin(), m_aliasedStations.end(), [roundedFrequencyHz](const StationDto & d)
                {
                    return d.frequencyAlias == roundedFrequencyHz;
                });

                if (it != m_aliasedStations.end())
                {
                    qDebug() << "Aliasing" << frequencyHz << "Hz [VHF] to" << it->frequency << "Hz [HF]";
                    roundedFrequencyHz = it->frequency;
                }

                if (m_transceivers.size() >= id + 1)
                {
                    if (m_transceivers[id].frequencyHz != roundedFrequencyHz)
                    {
                        m_transceivers[id].frequencyHz = roundedFrequencyHz;
                        this->updateTransceivers(false); // no frequency update
                    }
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
                if (!m_connection->isConnected()) { return; }
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

                QVector<TransceiverDto> enabledTransceivers;
                for (const TransceiverDto &transceiver : m_transceivers)
                {
                    if (m_enabledTransceivers.contains(transceiver.id))
                    {
                        enabledTransceivers.push_back(transceiver);
                    }
                }
                m_connection->updateTransceivers(m_callsign, enabledTransceivers);

                if (soundcardSampleProvider)
                {
                    soundcardSampleProvider->updateRadioTransceivers(m_transceivers);
                }
            }

            void CAfvClient::setTransmittingTransceiver(quint16 transceiverID)
            {
                TxTransceiverDto tx = { transceiverID };
                setTransmittingTransceivers({ tx });
            }

            void CAfvClient::setTransmittingComUnit(CComSystem::ComUnit comUnit)
            {
                this->setTransmittingTransceiver(comUnitToTransceiverId(comUnit));
            }

            void CAfvClient::setTransmittingTransceivers(const QVector<TxTransceiverDto> &transceivers)
            {
                m_transmittingTransceivers = transceivers;
            }

            bool CAfvClient::isTransmittingTransceiver(quint16 id) const
            {
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

                if (soundcardSampleProvider)
                {
                    soundcardSampleProvider->pttUpdate(active, m_transmittingTransceivers);
                }

                if (!active)
                {
                    //AGC
                    //if (maxDbReadingInPTTInterval > -1)
                    //    InputVolumeDb = InputVolumeDb - 1;
                    //if(maxDbReadingInPTTInterval < -4)
                    //    InputVolumeDb = InputVolumeDb + 1;
                    m_maxDbReadingInPTTInterval = -100;
                }

                emit this->ptt(active, com, this->identifier());
                // qDebug() << "PTT:" << active;
            }

            void CAfvClient::setInputVolumeDb(double valueDb)
            {
                if (valueDb > MaxDbIn) { valueDb = MaxDbIn; }
                if (valueDb < MinDbIn) { valueDb = MinDbIn; }
                m_inputVolumeDb = valueDb;
                if (m_input)
                {
                    m_input->setVolume(qPow(10, valueDb / 20.0));
                }
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
                const double db = this->getOutputVolumeDb();
                const double range = MaxDbIn - MinDbIn;
                const int i = qRound((db - MinDbIn) / range * 100);
                return i;
            }

            void CAfvClient::setNormalizedInputVolume(int volume)
            {
                if (volume < 0) { volume = 0; }
                else if (volume > 100) { volume = 100; }
                const double range = MaxDbIn - MinDbIn;
                const double dB = MinDbIn + (volume * range / 100.0);
                this->setInputVolumeDb(dB);
            }

            void CAfvClient::setNormalizedOutputVolume(int volume)
            {
                if (volume < 0) { volume = 0; }
                else if (volume > 100) { volume = 100; }
                const double range = MaxDbIn - MinDbIn;
                const double dB = MinDbIn + (volume * range / 100.0);
                this->setOutputVolumeDb(dB);
            }

            void CAfvClient::opusDataAvailable(const OpusDataAvailableArgs &args)
            {
                if (m_loopbackOn && m_transmit)
                {
                    IAudioDto audioData;
                    audioData.audio = QByteArray(args.audio.data(), args.audio.size());
                    audioData.callsign = "loopback";
                    audioData.lastPacket = false;
                    audioData.sequenceCounter = 0;

                    RxTransceiverDto com1 = { 0, m_transceivers.size() > 0 ?  m_transceivers[0].frequencyHz : UniCom, 1.0 };
                    RxTransceiverDto com2 = { 1, m_transceivers.size() > 1 ?  m_transceivers[1].frequencyHz : UniCom, 1.0 };

                    soundcardSampleProvider->addOpusSamples(audioData, { com1, com2 });
                    return;
                }

                if (m_transmittingTransceivers.size() > 0)
                {
                    if (m_transmit)
                    {
                        if (m_connection->isConnected())
                        {
                            AudioTxOnTransceiversDto dto;
                            dto.callsign = m_callsign.toStdString();
                            dto.sequenceCounter = args.sequenceCounter;
                            dto.audio = std::vector<char>(args.audio.begin(), args.audio.end());
                            dto.lastPacket = false;
                            dto.transceivers = m_transmittingTransceivers.toStdVector();
                            m_connection->sendToVoiceServer(dto);
                        }
                    }

                    if (!m_transmit && m_transmitHistory)
                    {
                        if (m_connection->isConnected())
                        {
                            AudioTxOnTransceiversDto dto;
                            dto.callsign = m_callsign.toStdString();
                            dto.sequenceCounter = args.sequenceCounter;
                            dto.audio = std::vector<char>(args.audio.begin(), args.audio.end());
                            dto.lastPacket = true;
                            dto.transceivers = m_transmittingTransceivers.toStdVector();
                            m_connection->sendToVoiceServer(dto);
                        }
                    }
                    m_transmitHistory = m_transmit;
                }
            }

            void CAfvClient::audioOutDataAvailable(const AudioRxOnTransceiversDto &dto)
            {
                IAudioDto audioData;
                audioData.audio = QByteArray(dto.audio.data(), static_cast<int>(dto.audio.size()));
                audioData.callsign = QString::fromStdString(dto.callsign);
                audioData.lastPacket = dto.lastPacket;
                audioData.sequenceCounter = dto.sequenceCounter;
                soundcardSampleProvider->addOpusSamples(audioData, QVector<RxTransceiverDto>::fromStdVector(dto.transceivers));
            }

            void CAfvClient::inputVolumeStream(const InputVolumeStreamArgs &args)
            {
                m_inputVolumeStream = args;
                emit inputVolumePeakVU(m_inputVolumeStream.PeakVU);
            }

            void CAfvClient::outputVolumeStream(const OutputVolumeStreamArgs &args)
            {
                m_outputVolumeStream = args;
                emit outputVolumePeakVU(m_outputVolumeStream.PeakVU);
            }

            QString CAfvClient::getReceivingCallsignsCom1()
            {
                if (soundcardSampleProvider)
                {
                    return soundcardSampleProvider->getReceivingCallsigns(0);
                }
                return {};
            }

            QString CAfvClient::getReceivingCallsignsCom2()
            {
                if (soundcardSampleProvider)
                {
                    return soundcardSampleProvider->getReceivingCallsigns(1);
                }
                return {};
            }

            void CAfvClient::onPositionUpdateTimer()
            {
                this->updateTransceivers();
            }

            void CAfvClient::onSettingsChanged()
            {
                const CSettings audioSettings = m_audioSettings.get();
                this->setNormalizedInputVolume(audioSettings.getInVolume());
                this->setNormalizedOutputVolume(audioSettings.getOutVolume());
                this->setBypassEffects(!audioSettings.isAudioEffectsEnabled());
            }

            void CAfvClient::updateTransceiversFromContext(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
            {
                Q_UNUSED(originator)
                this->updatePosition(aircraft.latitude().value(CAngleUnit::deg()),
                                     aircraft.longitude().value(CAngleUnit::deg()),
                                     aircraft.getAltitude().value(CLengthUnit::ft()));

                const CComSystem com1 = aircraft.getCom1System();
                const CComSystem com2 = aircraft.getCom2System();
                this->updateComFrequency(CComSystem::Com1, com1);
                this->updateComFrequency(CComSystem::Com2, com2);

                const bool tx1 = com1.isTransmitEnabled();
                const bool rx1 = com1.isReceiveEnabled();
                const bool tx2 = com2.isTransmitEnabled();
                const bool rx2 = com2.isReceiveEnabled();

                this->enableComUnit(CComSystem::Com1, tx1 || rx1);
                this->enableComUnit(CComSystem::Com2, tx2 || rx2);
                this->setTransmittingComUnit(CComSystem::Com1);
                this->setTransmittingComUnit(CComSystem::Com2);

                this->updateTransceivers();
                emit this->updatedFromOwnAircraftCockpit();
            }

            quint16 CAfvClient::comUnitToTransceiverId(CComSystem::ComUnit comUnit)
            {
                switch (comUnit)
                {
                case CComSystem::Com1: return 0;
                case CComSystem::Com2: return 1;
                default:
                    break;
                }
                return 0;
            }

            CComSystem::ComUnit CAfvClient::transceiverIdToComUnit(quint16 id)
            {
                if (comUnitToTransceiverId(CComSystem::Com1) == id) { return CComSystem::Com1; }
                if (comUnitToTransceiverId(CComSystem::Com2) == id) { return CComSystem::Com2; }
                return CComSystem::Com1;
            }

            bool CAfvClient::hasContext()
            {
                return sApp && !sApp->isShuttingDown() && sApp->getIContextOwnAircraft();
            }

            void CAfvClient::setOutputVolumeDb(double valueDb)
            {
                if (valueDb > MaxDbOut) { valueDb = MaxDbOut; }
                if (valueDb < MinDbOut) { valueDb = MinDbOut; }
                m_outputVolumeDb = valueDb;

                m_outputVolume = qPow(10, m_outputVolumeDb / 20.0);
                if (outputSampleProvider)
                {
                    outputSampleProvider->setVolume(m_outputVolume);
                }
            }

            const QAudioDeviceInfo &CAfvClient::getInputDevice() const
            {
                if (m_input) { return m_input->device(); }
                static const QAudioDeviceInfo null = QAudioDeviceInfo();
                return null;
            }

            const QAudioDeviceInfo &CAfvClient::getOutputDevice() const
            {
                if (m_output) { return m_output->device(); }
                static const QAudioDeviceInfo nullDevice = QAudioDeviceInfo();
                return nullDevice;
            }

            CAfvClient::ConnectionStatus CAfvClient::getConnectionStatus() const
            {
                return m_connection->isConnected() ? Connected : Disconnected;
            }
        } // ns
    } // ns
} // ns
