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
#include "blackcore/blackcoreexport.h"

#include "blackcore/context/contextownaircraft.h"

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
            class BLACKCORE_EXPORT AFVClient final : public QObject
            {
                Q_OBJECT
                Q_PROPERTY(float inputVolumePeakVU READ getInputVolumePeakVU NOTIFY inputVolumePeakVU)
                Q_PROPERTY(float outputVolumePeakVU READ getOutputVolumePeakVU NOTIFY outputVolumePeakVU)

            public:
                //! Ctor
                AFVClient(const QString &apiServer, QObject *parent = nullptr);

                //! Dtor
                virtual ~AFVClient()
                {
                    stop();
                }

                void setContextOwnAircraft(const BlackCore::Context::IContextOwnAircraft *contextOwnAircraft);

                QString callsign() const { return m_callsign; }

                bool isConnected() const { return m_connection->isConnected(); }

                Q_INVOKABLE void connectTo(const QString &cid, const QString &password, const QString &callsign);
                Q_INVOKABLE void disconnectFrom();

                Q_INVOKABLE QStringList availableInputDevices() const;
                Q_INVOKABLE QStringList availableOutputDevices() const;

                void setBypassEffects(bool value);

                bool isStarted() const { return m_isStarted; }
                QDateTime getStartDateTimeUt() const { return m_startDateTimeUtc; }

                void start(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice, const QVector<quint16> &transceiverIDs);
                Q_INVOKABLE void start(const QString &inputDeviceName, const QString &outputDeviceName);
                void stop();

                Q_INVOKABLE void updateComFrequency(quint16 id, quint32 frequency);
                Q_INVOKABLE void updatePosition(double latitude, double longitude, double height);

                void setTransmittingTransceivers(quint16 transceiverID);
                void setTransmittingTransceivers(const QVector<TxTransceiverDto> &transceivers);

                void setPtt(bool active);

                void setLoopBack(bool on) { m_loopbackOn = on; }

                float inputVolumeDb() const
                {
                    return m_inputVolumeDb;
                }

                void setInputVolumeDb(float value);

                float getOutputVolume() const;
                void setOutputVolume(float outputVolume);

                float getInputVolumePeakVU() const { return m_inputVolumeStream.PeakVU; }
                float getOutputVolumePeakVU() const { return m_outputVolumeStream.PeakVU; }

            signals:
                void receivingCallsignsChanged(const Audio::TransceiverReceivingCallsignsChangedArgs &args);
                void inputVolumePeakVU(float value);
                void outputVolumePeakVU(float value);

            private:
                void opusDataAvailable(const Audio::OpusDataAvailableArgs &args);
                void audioOutDataAvailable(const AudioRxOnTransceiversDto &dto);
                void inputVolumeStream(const Audio::InputVolumeStreamArgs &args);
                void outputVolumeStream(const Audio::OutputVolumeStreamArgs &args);

                void input_OpusDataAvailable();

                void updateTransceivers();
                void updateTransceiversFromContext(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

                static constexpr int c_sampleRate = 48000;
                static constexpr int frameSize = 960; //20ms

                // Connection
                Connection::ClientConnection *m_connection = nullptr;

                // Properties
                QString m_callsign;

                Audio::Input  *m_input = nullptr;
                Audio::Output *m_output = nullptr;

                Audio::SoundcardSampleProvider *soundcardSampleProvider = nullptr;
                // TODO VolumeSampleProvider outputSampleProvider;

                bool m_transmit = false;
                bool m_transmitHistory = false;
                QVector<TxTransceiverDto> m_transmittingTransceivers;

                bool m_isStarted = false;
                QDateTime m_startDateTimeUtc;

                float m_inputVolumeDb;
                float m_outputVolume = 1;

                double m_maxDbReadingInPTTInterval = -100;

                bool m_loopbackOn = false;

                QTimer m_voiceServerPositionTimer;
                QVector<TransceiverDto> m_transceivers;

                Audio::InputVolumeStreamArgs m_inputVolumeStream;
                Audio::OutputVolumeStreamArgs m_outputVolumeStream;

                BlackCore::Context::IContextOwnAircraft const *m_contextOwnAircraft = nullptr;
            };
        } // ns
    } // ns
} // ns

#endif
