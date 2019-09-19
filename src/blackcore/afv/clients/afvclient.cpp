/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "afvclient.h"
#include "blacksound/audioutilities.h"
#include <QDebug>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore::Context;
using namespace BlackCore::Afv::Audio;
using namespace BlackCore::Afv::Connection;

namespace BlackCore
{
    namespace Afv
    {
        namespace Clients
        {
            AFVClient::AFVClient(const QString &apiServer, QObject *parent) :
                QObject(parent)
            {
                m_connection = new ClientConnection(apiServer, this);
                m_connection->setReceiveAudio(false);

                m_input = new Input(c_sampleRate, this);
                connect(m_input, &Input::opusDataAvailable, this, &AFVClient::opusDataAvailable);
                connect(m_input, &Input::inputVolumeStream, this, &AFVClient::inputVolumeStream);

                m_output = new Output(this);
                connect(m_output, &Output::outputVolumeStream, this, &AFVClient::outputVolumeStream);

                connect(m_connection, &ClientConnection::audioReceived, this, &AFVClient::audioOutDataAvailable);

                connect(&m_voiceServerPositionTimer, &QTimer::timeout, this, qOverload<>(&AFVClient::updateTransceivers));

                m_transceivers =
                {
                    { 0, 122800000, 48.5, 11.5, 1000.0, 1000.0 },
                    { 1, 122800000, 48.5, 11.5, 1000.0, 1000.0 }
                };

                qDebug() << "UserClient instantiated";
            }

            void AFVClient::setContextOwnAircraft(const IContextOwnAircraft *contextOwnAircraft)
            {
                m_contextOwnAircraft = contextOwnAircraft;
                if (m_contextOwnAircraft)
                {
                    connect(m_contextOwnAircraft, &IContextOwnAircraft::changedAircraftCockpit, this, &AFVClient::updateTransceiversFromContext);
                }
            }

            void AFVClient::connectTo(const QString &cid, const QString &password, const QString &callsign)
            {
                m_callsign = callsign;
                m_connection->connectTo(cid, password, callsign);
                updateTransceivers();
            }

            void AFVClient::disconnectFrom()
            {
                m_connection->disconnectFrom();
            }

            QStringList AFVClient::availableInputDevices() const
            {
                const QList<QAudioDeviceInfo> inputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

                QStringList deviceNames;
                for (const QAudioDeviceInfo &inputDevice : inputDevices)
                {
                    deviceNames << inputDevice.deviceName();
                }
                return deviceNames;
            }

            QStringList AFVClient::availableOutputDevices() const
            {
                const QList<QAudioDeviceInfo> outputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

                QStringList deviceNames;
                for (const QAudioDeviceInfo &outputDevice : outputDevices)
                {
                    deviceNames << outputDevice.deviceName();
                }
                return deviceNames;
            }

            void AFVClient::setBypassEffects(bool value)
            {
                if (soundcardSampleProvider)
                {
                    soundcardSampleProvider->setBypassEffects(value);
                }
            }

            void AFVClient::start(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice, const QVector<quint16> &transceiverIDs)
            {
                if (m_isStarted)
                {
                    qDebug() << "Client already started";
                    return;
                }

                soundcardSampleProvider = new SoundcardSampleProvider(c_sampleRate, transceiverIDs, this);
                connect(soundcardSampleProvider, &SoundcardSampleProvider::receivingCallsignsChanged, this, &AFVClient::receivingCallsignsChanged);
                // TODO outputSampleProvider = new VolumeSampleProvider(soundcardSampleProvider);

                m_output->start(outputDevice, soundcardSampleProvider);
                m_input->start(inputDevice);

                m_startDateTimeUtc = QDateTime::currentDateTimeUtc();
                m_connection->setReceiveAudio(true);
                m_voiceServerPositionTimer.start(5000);
                m_isStarted = true;
                qDebug() << ("Started [Input: " + inputDevice.deviceName() + "] [Output: " + outputDevice.deviceName() + "]");
            }

            void AFVClient::start(const QString &inputDeviceName, const QString &outputDeviceName)
            {
                if (m_isStarted) { return; }

                soundcardSampleProvider = new SoundcardSampleProvider(c_sampleRate, { 0, 1 }, this);
                connect(soundcardSampleProvider, &SoundcardSampleProvider::receivingCallsignsChanged, this, &AFVClient::receivingCallsignsChanged);
                // TODO outputSampleProvider = new VolumeSampleProvider(soundcardSampleProvider);

                QAudioDeviceInfo inputDevice = QAudioDeviceInfo::defaultInputDevice();
                for (const auto &device : QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
                {
                    if (device.deviceName().startsWith(inputDeviceName))
                    {
                        inputDevice = device;
                        break;
                    }
                }

                QAudioDeviceInfo outputDevice = QAudioDeviceInfo::defaultOutputDevice();
                for (const auto &device : QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
                {
                    if (device.deviceName().startsWith(outputDeviceName))
                    {
                        outputDevice = device;
                        break;
                    }
                }

                m_output->start(outputDevice, soundcardSampleProvider);
                m_input->start(inputDevice);

                m_startDateTimeUtc = QDateTime::currentDateTimeUtc();
                m_connection->setReceiveAudio(true);
                m_voiceServerPositionTimer.start(5000);
                m_isStarted = true;
            }

            void AFVClient::stop()
            {
                if (! m_isStarted)
                {
                    qDebug() << "Client not started";
                    return;
                }

                m_isStarted = false;
                m_connection->setReceiveAudio(false);

                m_transceivers.clear();
                updateTransceivers();

                m_input->stop();
                m_output->stop();
            }

            void AFVClient::updateComFrequency(quint16 id, quint32 frequency)
            {
                if (id != 0 && id != 1) { return; }

                // Fix rounding issues like 128074999 Hz -> 128075000 Hz
                quint32 roundedFrequency = qRound(frequency / 1000.0) * 1000;

                if (m_transceivers.size() >= id + 1)
                {
                    if (m_transceivers[id].frequency != roundedFrequency)
                    {
                        m_transceivers[id].frequency = roundedFrequency;
                        updateTransceivers();
                    }
                }
            }

            void AFVClient::updatePosition(double latitude, double longitude, double height)
            {
                for (TransceiverDto &transceiver : m_transceivers)
                {
                    transceiver.LatDeg = latitude;
                    transceiver.LonDeg = longitude;
                    transceiver.HeightAglM = height;
                    transceiver.HeightMslM = height;
                }
            }

            void AFVClient::updateTransceivers()
            {
                if (! m_connection->isConnected()) { return; }

                if (m_contextOwnAircraft)
                {
                    BlackMisc::Simulation::CSimulatedAircraft ownAircraft = m_contextOwnAircraft->getOwnAircraft();
                    updatePosition(ownAircraft.latitude().value(CAngleUnit::deg()),
                                   ownAircraft.longitude().value(CAngleUnit::deg()),
                                   ownAircraft.getAltitude().value(CLengthUnit::ft()));
                    updateComFrequency(0, ownAircraft.getCom1System().getFrequencyActive().value(CFrequencyUnit::Hz()));
                    updateComFrequency(1, ownAircraft.getCom2System().getFrequencyActive().value(CFrequencyUnit::Hz()));
                }

                m_connection->updateTransceivers(m_callsign, m_transceivers);

                if (soundcardSampleProvider)
                {
                    soundcardSampleProvider->updateRadioTransceivers(m_transceivers);
                }
            }

            void AFVClient::setTransmittingTransceivers(quint16 transceiverID)
            {
                TxTransceiverDto tx = { transceiverID };
                setTransmittingTransceivers({ tx });
            }

            void AFVClient::setTransmittingTransceivers(const QVector<TxTransceiverDto> &transceivers)
            {
                m_transmittingTransceivers = transceivers;
            }

            void AFVClient::setPtt(bool active)
            {
                if (! m_isStarted)
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

                qDebug() << "PTT:" << active;
            }

            void AFVClient::setInputVolumeDb(float value)
            {
                if (value > 18) { value = 18; }
                if (value < -18) { value = -18; }
                m_inputVolumeDb = value;
                // TODO input.Volume = (float)System.Math.Pow(10, value / 20);
            }

            void AFVClient::opusDataAvailable(const OpusDataAvailableArgs &args)
            {
                if (m_loopbackOn && m_transmit)
                {
                    IAudioDto audioData;
                    audioData.audio = QByteArray(args.audio.data(), args.audio.size());
                    audioData.callsign = "loopback";
                    audioData.lastPacket = false;
                    audioData.sequenceCounter = 0;
                    RxTransceiverDto com1 = { 0, m_transceivers[0].frequency, 0.0 };
                    RxTransceiverDto com2 = { 1, m_transceivers[1].frequency, 0.0 };

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

            void AFVClient::audioOutDataAvailable(const AudioRxOnTransceiversDto &dto)
            {
                IAudioDto audioData;
                audioData.audio = QByteArray(dto.audio.data(), static_cast<int>(dto.audio.size()));
                audioData.callsign = QString::fromStdString(dto.callsign);
                audioData.lastPacket = dto.lastPacket;
                audioData.sequenceCounter = dto.sequenceCounter;
                soundcardSampleProvider->addOpusSamples(audioData, QVector<RxTransceiverDto>::fromStdVector(dto.transceivers));
            }

            void AFVClient::inputVolumeStream(const InputVolumeStreamArgs &args)
            {
                m_inputVolumeStream = args;
                emit inputVolumePeakVU(m_inputVolumeStream.PeakVU);
            }

            void AFVClient::outputVolumeStream(const OutputVolumeStreamArgs &args)
            {
                m_outputVolumeStream = args;
                emit outputVolumePeakVU(m_outputVolumeStream.PeakVU);
            }

            void AFVClient::updateTransceiversFromContext(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator)
            {
                Q_UNUSED(originator);
                updatePosition(aircraft.latitude().value(CAngleUnit::deg()),
                               aircraft.longitude().value(CAngleUnit::deg()),
                               aircraft.getAltitude().value(CLengthUnit::ft()));
                updateComFrequency(0, aircraft.getCom1System().getFrequencyActive().value(CFrequencyUnit::Hz()));
                updateComFrequency(1, aircraft.getCom2System().getFrequencyActive().value(CFrequencyUnit::Hz()));
                updateTransceivers();
            }

            float AFVClient::getOutputVolume() const
            {
                return m_outputVolume;
            }

            void AFVClient::setOutputVolume(float outputVolume)
            {
                if (outputVolume > 18) { m_outputVolume = 18; }
                if (outputVolume < -60) { m_outputVolume = -60; }
                // m_outputVolume = (float)System.Math.Pow(10, value / 20);
                // TODO outputSampleProvider.Volume = outputVolume;
            }
        } // ns
    } // ns
} // ns
