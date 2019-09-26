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
                connect(&m_voiceServerPositionTimer, &QTimer::timeout, this, qOverload<>(&CAfvClient::updateTransceivers));

                m_transceivers =
                {
                    { 0, 122800000, 48.5, 11.5, 1000.0, 1000.0 },
                    { 1, 122800000, 48.5, 11.5, 1000.0, 1000.0 }
                };

                m_enabledTransceivers = { 0, 1 };
                m_transmittingTransceivers = { { 0 } }; // TxTransceiverDto

                // init by settings
                this->onSettingsChanged();

                CLogMessage(this).info(u"UserClient instantiated");
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
                this->updateTransceivers();

                if (m_connection->isConnected()) { emit this->connectionStatusChanged(Connected); }
                else { emit this->connectionStatusChanged(Disconnected); }

                m_connection->getAllAliasedStations();
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

                soundcardSampleProvider = new CSoundcardSampleProvider(SampleRate, transceiverIDs, this);
                connect(soundcardSampleProvider, &CSoundcardSampleProvider::receivingCallsignsChanged, this, &CAfvClient::receivingCallsignsChanged);
                outputSampleProvider = new CVolumeSampleProvider(soundcardSampleProvider, this);
                outputSampleProvider->setVolume(m_outputVolume);

                m_output->start(outputDevice.isNull() ? QAudioDeviceInfo::defaultOutputDevice() : outputDevice, outputSampleProvider);
                m_input->start(inputDevice.isNull() ? QAudioDeviceInfo::defaultInputDevice() : inputDevice);

                m_startDateTimeUtc = QDateTime::currentDateTimeUtc();
                m_connection->setReceiveAudio(true);
                m_voiceServerPositionTimer.start(5000);
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
                updateTransceivers();

                m_input->stop();
                m_output->stop();
                CLogMessage(this).info(u"Client NOT stopped");
            }

            void CAfvClient::enableTransceiver(quint16 id, bool enable)
            {
                if (enable) { m_enabledTransceivers.insert(id); }
                else { m_enabledTransceivers.remove(id); }

                updateTransceivers();
            }

            void CAfvClient::updateComFrequency(quint16 id, quint32 frequencyHz)
            {
                if (id != 0 && id != 1) { return; }

                // Fix rounding issues like 128074999 Hz -> 128075000 Hz
                quint32 roundedFrequencyHz = static_cast<quint32>(qRound(frequencyHz / 1000.0)) * 1000;

                if (m_transceivers.size() >= id + 1)
                {
                    if (m_transceivers[id].frequencyHz != roundedFrequencyHz)
                    {
                        m_transceivers[id].frequencyHz = roundedFrequencyHz;
                        updateTransceivers();
                    }
                }
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

            void CAfvClient::updateTransceivers()
            {
                if (!m_connection->isConnected()) { return; }
                if (hasContext())
                {
                    const CSimulatedAircraft ownAircraft = sApp->getIContextOwnAircraft()->getOwnAircraft();
                    updatePosition(ownAircraft.latitude().value(CAngleUnit::deg()),
                                   ownAircraft.longitude().value(CAngleUnit::deg()),
                                   ownAircraft.getAltitude().value(CLengthUnit::ft()));

                    const quint16 com1Hz = static_cast<quint16>(ownAircraft.getCom1System().getFrequencyActive().valueInteger(CFrequencyUnit::Hz()));
                    const quint16 com2Hz = static_cast<quint16>(ownAircraft.getCom2System().getFrequencyActive().valueInteger(CFrequencyUnit::Hz()));
                    updateComFrequency(0, com1Hz);
                    updateComFrequency(1, com2Hz);
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

            void CAfvClient::setTransmittingTransceivers(quint16 transceiverID)
            {
                TxTransceiverDto tx = { transceiverID };
                setTransmittingTransceivers({ tx });
            }

            void CAfvClient::setTransmittingTransceivers(const QVector<TxTransceiverDto> &transceivers)
            {
                m_transmittingTransceivers = transceivers;
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
                    qDebug() << "Client not started";
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
                qDebug() << "PTT:" << active;
            }

            void CAfvClient::setInputVolumeDb(double value)
            {
                if (value > MaxDb) { value = MaxDb; }
                if (value < MinDb) { value = MinDb; }
                m_inputVolumeDb = value;
                m_input->setVolume(qPow(10, value / 20.0));
            }

            int CAfvClient::getNormalizedInputVolume() const
            {
                const double db = this->getInputVolumeDb();
                const double range = MaxDb - MinDb;
                const int i = qRound((db - MinDb) / range * 100);
                return i;
            }

            int CAfvClient::getNormalizedOutputVolume() const
            {
                const double db = this->getOutputVolumeDb();
                const double range = MaxDb - MinDb;
                const int i = qRound((db - MinDb) / range * 100);
                return i;
            }

            void CAfvClient::setNormalizedInputVolume(int volume)
            {
                if (volume < 0) { volume = 0; }
                else if (volume > 100) { volume = 100; }
                const double range = MaxDb - MinDb;
                const double dB = MinDb + (volume * range / 100.0);
                this->setInputVolumeDb(dB);
            }

            void CAfvClient::setNormalizedOutputVolume(int volume)
            {
                if (volume < 0) { volume = 0; }
                else if (volume > 100) { volume = 100; }
                const double range = MaxDb - MinDb;
                const double dB = MinDb + (volume * range / 100.0);
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

                    RxTransceiverDto com1 = { 0, m_transceivers.size() > 0 ?  m_transceivers[0].frequencyHz : UniCom, 0.0 };
                    RxTransceiverDto com2 = { 1, m_transceivers.size() > 1 ?  m_transceivers[1].frequencyHz : UniCom, 0.0 };

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
                updatePosition(aircraft.latitude().value(CAngleUnit::deg()),
                               aircraft.longitude().value(CAngleUnit::deg()),
                               aircraft.getAltitude().value(CLengthUnit::ft()));

                const quint16 com1Hz = static_cast<quint16>(aircraft.getCom1System().getFrequencyActive().valueInteger(CFrequencyUnit::Hz()));
                const quint16 com2Hz = static_cast<quint16>(aircraft.getCom2System().getFrequencyActive().valueInteger(CFrequencyUnit::Hz()));

                updateComFrequency(0, com1Hz);
                updateComFrequency(1, com2Hz);
                updateTransceivers();
            }

            bool CAfvClient::hasContext()
            {
                return sApp && !sApp->isShuttingDown() && sApp->getIContextOwnAircraft();
            }

            double CAfvClient::getOutputVolumeDb() const
            {
                return m_outputVolume;
            }

            void CAfvClient::setOutputVolumeDb(double outputVolume)
            {
                if (outputVolume > 18) { m_outputVolume = 18; }
                if (outputVolume < -60) { m_outputVolume = -60; }
                m_outputVolume = qPow(10, m_outputVolume / 20);
                if (outputSampleProvider)
                {
                    outputSampleProvider->setVolume(outputVolume);
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
