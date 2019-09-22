/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CLIENT_AFVCLIENT_H
#define BLACKCORE_AFV_CLIENT_AFVCLIENT_H

#include "blackcore/afv/connection/clientconnection.h"
#include "blackcore/afv/audio/input.h"
#include "blackcore/afv/audio/output.h"
#include "blackcore/afv/audio/soundcardsampleprovider.h"
#include "blackcore/afv/dto.h"

#include "blackcore/context/contextownaircraft.h"
#include "blackcore/blackcoreexport.h"

#include "blackmisc/audio/ptt.h"
#include "blacksound/sampleprovider/volumesampleprovider.h"

#include <QAudioDeviceInfo>
#include <QDateTime>
#include <QAudioInput>
#include <QAudioOutput>
#include <QObject>
#include <QString>
#include <QVector>

namespace BlackCore
{
    namespace Afv
    {
        namespace Clients
        {
            //! AFV client
            class BLACKCORE_EXPORT CAfvClient final : public QObject
            {
                Q_OBJECT
                Q_PROPERTY(double inputVolumePeakVU READ getInputVolumePeakVU NOTIFY inputVolumePeakVU)
                Q_PROPERTY(double outputVolumePeakVU READ getOutputVolumePeakVU NOTIFY outputVolumePeakVU)
                Q_PROPERTY(ConnectionStatus connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)
                Q_PROPERTY(QString receivingCallsignsCom1 READ getReceivingCallsignsCom1 NOTIFY receivingCallsignsChanged)
                Q_PROPERTY(QString receivingCallsignsCom2 READ getReceivingCallsignsCom2 NOTIFY receivingCallsignsChanged)

            public:
                enum ConnectionStatus { Disconnected, Connected };
                Q_ENUM(ConnectionStatus)

                //! Ctor
                CAfvClient(const QString &apiServer, QObject *parent = nullptr);

                //! Dtor
                virtual ~CAfvClient() override
                {
                    this->stop();
                }

                // void setContextOwnAircraft(const BlackCore::Context::IContextOwnAircraft *contextOwnAircraft);

                QString callsign() const { return m_callsign; }

                bool isConnected() const { return m_connection->isConnected(); }
                ConnectionStatus getConnectionStatus() const;

                Q_INVOKABLE void connectTo(const QString &cid, const QString &password, const QString &callsign);
                Q_INVOKABLE void disconnectFrom();

                Q_INVOKABLE QStringList availableInputDevices() const;
                Q_INVOKABLE QStringList availableOutputDevices() const;

                Q_INVOKABLE void setBypassEffects(bool value);

                bool isStarted() const { return m_isStarted; }
                QDateTime getStartDateTimeUt() const { return m_startDateTimeUtc; }

                void start(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice, const QVector<quint16> &transceiverIDs);
                Q_INVOKABLE void start(const QString &inputDeviceName, const QString &outputDeviceName);
                void stop();

                Q_INVOKABLE void enableTransceiver(quint16 id, bool enable);
                Q_INVOKABLE void updateComFrequency(quint16 id, quint32 frequencyHz);
                Q_INVOKABLE void updatePosition(double latitudeDeg, double longitudeDeg, double heightMeters);

                void setTransmittingTransceivers(quint16 transceiverID);
                void setTransmittingTransceivers(const QVector<TxTransceiverDto> &transceivers);

                //! Push to talk @{
                Q_INVOKABLE void setPtt(bool active);
                void setPttForCom(bool active, BlackMisc::Audio::PTTCOM com);
                //! @}


                Q_INVOKABLE void setLoopBack(bool on) { m_loopbackOn = on; }

                //! Input volume in DB, +-18DB @{
                double getInputVolumeDb() const { return m_inputVolumeDb; }
                Q_INVOKABLE void setInputVolumeDb(double value);
                //! @}

                //! Output volume in DB, +-18DB @{
                double getOutputVolumeDb() const;
                Q_INVOKABLE void setOutputVolumeDb(double outputVolume);
                //! @}

                //! VU values, 0..1 @{
                double getInputVolumePeakVU() const { return m_inputVolumeStream.PeakVU; }
                double getOutputVolumePeakVU() const { return m_outputVolumeStream.PeakVU; }
                //! @}

            signals:
                void receivingCallsignsChanged(const Audio::TransceiverReceivingCallsignsChangedArgs &args);
                void inputVolumePeakVU(double value);
                void outputVolumePeakVU(double value);
                void connectionStatusChanged(ConnectionStatus status);

            private:
                void opusDataAvailable(const Audio::OpusDataAvailableArgs &args);
                void audioOutDataAvailable(const AudioRxOnTransceiversDto &dto);
                void inputVolumeStream(const Audio::InputVolumeStreamArgs &args);
                void outputVolumeStream(const Audio::OutputVolumeStreamArgs &args);
                QString getReceivingCallsignsCom1();
                QString getReceivingCallsignsCom2();

                void input_OpusDataAvailable();

                void updateTransceivers();
                void updateTransceiversFromContext(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

                static constexpr int SampleRate = 48000;
                static constexpr int FrameSize  = 960; // 20ms

                // Connection
                Connection::CClientConnection *m_connection = nullptr;

                // Properties
                QString m_callsign;

                Audio::CInput *m_input  = nullptr;
                Audio::Output *m_output = nullptr;

                Audio::CSoundcardSampleProvider *soundcardSampleProvider = nullptr;
                BlackSound::SampleProvider::CVolumeSampleProvider *outputSampleProvider = nullptr;

                bool m_transmit = false;
                bool m_transmitHistory = false;
                QVector<TxTransceiverDto> m_transmittingTransceivers;

                bool m_isStarted = false;
                QDateTime m_startDateTimeUtc;

                double m_inputVolumeDb;
                double m_outputVolume = 1.0;

                double m_maxDbReadingInPTTInterval = -100;

                bool m_loopbackOn = false;

                QTimer m_voiceServerPositionTimer;
                QVector<TransceiverDto> m_transceivers;
                QSet<quint16> m_enabledTransceivers;

                Audio::InputVolumeStreamArgs m_inputVolumeStream;
                Audio::OutputVolumeStreamArgs m_outputVolumeStream;

                void initWithContext();
                static bool hasContext();

            };
        } // ns
    } // ns
} // ns

#endif
