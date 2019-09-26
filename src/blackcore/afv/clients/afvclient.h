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

#include "blackcore/context/contextownaircraft.h"
#include "blackcore/afv/connection/clientconnection.h"
#include "blackcore/afv/audio/input.h"
#include "blackcore/afv/audio/output.h"
#include "blackcore/afv/audio/soundcardsampleprovider.h"
#include "blackcore/afv/dto.h"
#include "blackcore/blackcoreexport.h"

#include "blacksound/sampleprovider/volumesampleprovider.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/ptt.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/settingscache.h"

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
            class BLACKCORE_EXPORT CAfvClient final : public QObject, public BlackMisc::CIdentifiable
            {
                Q_OBJECT
                Q_PROPERTY(double inputVolumePeakVU  READ getInputVolumePeakVU  NOTIFY inputVolumePeakVU)
                Q_PROPERTY(double outputVolumePeakVU READ getOutputVolumePeakVU NOTIFY outputVolumePeakVU)
                Q_PROPERTY(ConnectionStatus connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)
                Q_PROPERTY(QString receivingCallsignsCom1 READ getReceivingCallsignsCom1 NOTIFY receivingCallsignsChanged)
                Q_PROPERTY(QString receivingCallsignsCom2 READ getReceivingCallsignsCom2 NOTIFY receivingCallsignsChanged)

            public:
                //! Categories
                static const BlackMisc::CLogCategoryList &getLogCategories();

                //! Connection status
                enum ConnectionStatus { Disconnected, Connected };
                Q_ENUM(ConnectionStatus)

                //! Ctor
                CAfvClient(const QString &apiServer, QObject *parent = nullptr);

                //! Dtor
                virtual ~CAfvClient() override { this->stop(); }

                //! Corresponding callsign
                QString callsign() const { return m_callsign; }

                bool isConnected() const { return m_connection->isConnected(); }
                ConnectionStatus getConnectionStatus() const;

                Q_INVOKABLE void connectTo(const QString &cid, const QString &password, const QString &callsign);
                Q_INVOKABLE void disconnectFrom();

                //! Audio devices @{
                Q_INVOKABLE QStringList availableInputDevices() const;
                Q_INVOKABLE QStringList availableOutputDevices() const;
                //! @}

                //! Enable/disable VHF simulation, true means effects are NOT used
                Q_INVOKABLE void setBypassEffects(bool value);

                bool isStarted() const { return m_isStarted; }
                QDateTime getStartDateTimeUt() const { return m_startDateTimeUtc; }

                //! Muted @{
                bool isMuted() const;
                void setMuted(bool mute);
                //! @}

                bool restartWithNewDevices(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice);
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

                //! Loopback @{
                Q_INVOKABLE void setLoopBack(bool on) { m_loopbackOn = on; }
                Q_INVOKABLE bool isLoopback() const { return m_loopbackOn; }
                //! @}

                //! Input volume in dB, +-18dB @{
                double getInputVolumeDb() const { return m_inputVolumeDb; }
                Q_INVOKABLE void setInputVolumeDb(double value);
                //! @}

                //! Output volume in dB, +-18dB @{
                double getOutputVolumeDb() const;
                Q_INVOKABLE void setOutputVolumeDb(double outputVolume);
                //! @}

                //! Normalized volumes 0..100 @{
                int getNormalizedInputVolume() const;
                int getNormalizedOutputVolume() const;
                void setNormalizedInputVolume(int volume);
                void setNormalizedOutputVolume(int volume);
                //! @}

                //! VU values, 0..1 @{
                double getInputVolumePeakVU() const { return m_inputVolumeStream.PeakVU; }
                double getOutputVolumePeakVU() const { return m_outputVolumeStream.PeakVU; }
                //! @}

                //! Recently used device @{
                const QAudioDeviceInfo &getInputDevice() const;
                const QAudioDeviceInfo &getOutputDevice() const;
                //! @}

            signals:
                //! Receiving callsigns have been changed
                //! \remark callsigns I do receive
                void receivingCallsignsChanged(const Audio::TransceiverReceivingCallsignsChangedArgs &args);

                //! Connection status has been changed
                void connectionStatusChanged(ConnectionStatus status);

                //! PTT status in this particular AFV client
                void ptt(bool active, BlackMisc::Audio::PTTCOM pttcom, const BlackMisc::CIdentifier &identifier);

                //! VU levels @{
                void inputVolumePeakVU(double value);
                void outputVolumePeakVU(double value);
                //! @}

            private:
                void opusDataAvailable(const Audio::OpusDataAvailableArgs &args);
                void audioOutDataAvailable(const AudioRxOnTransceiversDto &dto);
                void inputVolumeStream(const Audio::InputVolumeStreamArgs &args);
                void outputVolumeStream(const Audio::OutputVolumeStreamArgs &args);
                QString getReceivingCallsignsCom1();
                QString getReceivingCallsignsCom2();

                void input_OpusDataAvailable();
                void onSettingsChanged();

                void updateTransceivers();
                void updateTransceiversFromContext(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

                static constexpr int SampleRate = 48000;
                static constexpr int FrameSize  =   960; // 20ms
                static constexpr double MinDb   = -18.0;
                static constexpr double MaxDb   =  18.0;
                static constexpr quint32 UniCom = 122800000;

                Connection::CClientConnection *m_connection = nullptr;
                BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this, &CAfvClient::onSettingsChanged };
                QString m_callsign;

                Audio::CInput *m_input  = nullptr;
                Audio::Output *m_output = nullptr;

                Audio::CSoundcardSampleProvider *soundcardSampleProvider = nullptr;
                BlackSound::SampleProvider::CVolumeSampleProvider *outputSampleProvider = nullptr;

                bool m_transmit = false;
                bool m_transmitHistory = false;
                QVector<TxTransceiverDto> m_transmittingTransceivers;
                static const QVector<quint16> &allTransceiverIds() { static const QVector<quint16> transceiverIds{0, 1}; return transceiverIds; }

                bool m_isStarted = false;
                bool m_loopbackOn = false;
                QDateTime m_startDateTimeUtc;

                double m_inputVolumeDb;
                double m_outputVolume = 1.0;
                double m_maxDbReadingInPTTInterval = -100;

                QTimer m_voiceServerPositionTimer;
                QVector<TransceiverDto> m_transceivers;
                QSet<quint16> m_enabledTransceivers;

                Audio::InputVolumeStreamArgs  m_inputVolumeStream;
                Audio::OutputVolumeStreamArgs m_outputVolumeStream;

                void initWithContext();
                static bool hasContext();
            };
        } // ns
    } // ns
} // ns

#endif
