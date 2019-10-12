/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CLIENTS_AFVCLIENT_H
#define BLACKCORE_AFV_CLIENTS_AFVCLIENT_H

#include "blackcore/context/contextownaircraft.h"
#include "blackcore/afv/connection/clientconnection.h"
#include "blackcore/afv/audio/input.h"
#include "blackcore/afv/audio/output.h"
#include "blackcore/afv/audio/soundcardsampleprovider.h"
#include "blackcore/afv/dto.h"
#include "blackcore/blackcoreexport.h"

#include "blacksound/sampleprovider/volumesampleprovider.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/ptt.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/worker.h"

#include <QDateTime>
#include <QAudioInput>
#include <QAudioOutput>
#include <QObject>
#include <QString>
#include <QVector>

#include <atomic>

namespace BlackCore
{
    namespace Afv
    {
        namespace Clients
        {
            //! AFV client
            class BLACKCORE_EXPORT CAfvClient final : public BlackMisc::CContinuousWorker
            {
                Q_OBJECT
                Q_PROPERTY(double inputVolumePeakVU  READ getInputVolumePeakVU  NOTIFY inputVolumePeakVU)
                Q_PROPERTY(double outputVolumePeakVU READ getOutputVolumePeakVU NOTIFY outputVolumePeakVU)
                Q_PROPERTY(BlackCore::Afv::Clients::CAfvClient::ConnectionStatus connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)
                Q_PROPERTY(QString receivingCallsignsCom1 READ getReceivingCallsignsCom1 NOTIFY receivingCallsignsChanged)
                Q_PROPERTY(QString receivingCallsignsCom2 READ getReceivingCallsignsCom2 NOTIFY receivingCallsignsChanged)

            public:
                //! Categories
                static const BlackMisc::CLogCategoryList &getLogCategories();

                //! Connection status
                enum ConnectionStatus { Disconnected, Connected };
                Q_ENUM(ConnectionStatus)

                //! Ctor
                CAfvClient(const QString &apiServer, QObject *owner);

                //! Dtor
                virtual ~CAfvClient() override { this->stopAudio(); }

                //! Corresponding callsign
                //! \threadsafe
                //! @{
                QString getCallsign() const;
                void setCallsign(const QString &getCallsign);
                //! @}

                //! Is connected to network?
                bool isConnected() const { return m_connection->isConnected(); }

                //! Connection status
                ConnectionStatus getConnectionStatus() const;

                //! Connect to network
                Q_INVOKABLE void connectTo(const QString &cid, const QString &password, const QString &getCallsign);

                //! Disconnect from network
                Q_INVOKABLE void disconnectFrom();

                //! Audio devices @{
                Q_INVOKABLE QStringList availableInputDevices() const;
                Q_INVOKABLE QStringList availableOutputDevices() const;
                //! @}

                //! Enable/disable VHF simulation, true means effects are NOT used
                Q_INVOKABLE void setBypassEffects(bool value);

                //! Client started?
                bool isStarted() const { return m_isStarted; }

                //! When started
                const QDateTime &getStartDateTimeUtc() const { return m_startDateTimeUtc; }

                //! Muted @{
                bool isMuted() const;
                void setMuted(bool mute);
                //! @}

                //! Start/stop client @{
                void restartWithNewDevices(const BlackMisc::Audio::CAudioDeviceInfo &inputDevice, const BlackMisc::Audio::CAudioDeviceInfo &outputDevice);
                void startAudio(const BlackMisc::Audio::CAudioDeviceInfo &inputDevice, const BlackMisc::Audio::CAudioDeviceInfo &outputDevice, const QVector<quint16> &transceiverIDs);
                Q_INVOKABLE void startAudio(const QString &inputDeviceName, const QString &outputDeviceName);
                void stopAudio();
                //! @}

                //! Enable COM unit/transceiver
                //! \threadsafe
                //! @{
                Q_INVOKABLE void enableTransceiver(quint16 id, bool enable);
                void enableComUnit(BlackMisc::Aviation::CComSystem::ComUnit comUnit, bool enable);
                bool isEnabledTransceiver(quint16 id) const;
                bool isEnabledComUnit(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const;
                //! @}

                //! Set transmitting transceivers
                //! \threadsafe
                //! @{
                void setTransmittingTransceiver(quint16 transceiverID);
                void setTransmittingComUnit(BlackMisc::Aviation::CComSystem::ComUnit comUnit);
                void setTransmittingTransceivers(const QVector<TxTransceiverDto> &transceivers);
                //! @}

                //! Transmitting transceiver/COM unit
                //! \threadsafe
                //! @{
                bool isTransmittingTransceiver(quint16 id) const;
                bool isTransmittingdComUnit(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const;
                //! @}

                //! Get transceivers
                //! \threadsafe
                //! @{
                QVector<TransceiverDto> getTransceivers() const;
                QVector<TxTransceiverDto> getTransmittingTransceivers() const;
                QSet<quint16> getEnabledTransceivers() const;
                //! @}

                //! Update frequency
                //! \threadsafe
                //! @{
                Q_INVOKABLE void updateComFrequency(quint16 id, quint32 frequencyHz);
                void updateComFrequency(BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::PhysicalQuantities::CFrequency &comFrequency);
                void updateComFrequency(BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::Aviation::CComSystem &comSystem);
                //! @}

                //! Update own aircraft position
                //! \threadsafe
                Q_INVOKABLE void updatePosition(double latitudeDeg, double longitudeDeg, double heightMeters);

                //! Update from own aircraft
                //! \remark full update of frequency, position and enabled transceivers in one step
                //! \threadsafe
                void updateFromOwnAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool withSignals = true);

                //! Push to talk @{
                Q_INVOKABLE void setPtt(bool active);
                void setPttForCom(bool active, BlackMisc::Audio::PTTCOM com);
                //! @}

                //! Loopback
                //! \threadsafe
                //! @{
                Q_INVOKABLE void setLoopBack(bool on) { m_loopbackOn = on; }
                Q_INVOKABLE bool isLoopback() const { return m_loopbackOn; }
                //! @}

                //! Input volume in dB, +-18dB
                //! \threadsafe
                //! @{
                double getInputVolumeDb() const;
                Q_INVOKABLE bool setInputVolumeDb(double valueDb);
                //! @}

                //! Output volume in dB, +-18dB
                //! \threadsafe
                //! @{
                double getOutputVolumeDb() const;
                Q_INVOKABLE bool setOutputVolumeDb(double valueDb);
                //! @}

                //! Normalized volumes 0..100
                //! \threadsafe
                //! @{
                int getNormalizedInputVolume() const;
                int getNormalizedOutputVolume() const;
                bool setNormalizedInputVolume(int volume);
                void setNormalizedOutputVolume(int volume);
                //! @}

                //! VU values, 0..1
                //! \threadsafe
                //! @{
                double getInputVolumePeakVU() const;
                double getOutputVolumePeakVU() const;
                //! @}

                //! Recently used device
                //! \threadsafe
                //! @{
                const BlackMisc::Audio::CAudioDeviceInfo &getInputDevice() const;
                const BlackMisc::Audio::CAudioDeviceInfo &getOutputDevice() const;
                bool usesSameDevices(const BlackMisc::Audio::CAudioDeviceInfo &inputDevice, const BlackMisc::Audio::CAudioDeviceInfo &outputDevice);
                //! @}

                //! Callsigns currently received
                //! \threadsafe
                //! @{
                QString getReceivingCallsignsCom1();
                QString getReceivingCallsignsCom2();
                //! @}

                //! Update the voice server URL
                bool updateVoiceServerUrl(const QString &url);

                //! Gracefully shut down AFV client
                void gracefulShutdown();

            signals:
                //! Receiving callsigns have been changed
                //! \remark callsigns I do receive
                void receivingCallsignsChanged(const BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs &args);

                //! Connection status has been changed
                void connectionStatusChanged(ConnectionStatus status);

                //! Client updated from own aicraft data
                void updatedFromOwnAircraftCockpit();

                //! PTT status in this particular AFV client
                void ptt(bool active, BlackMisc::Audio::PTTCOM pttcom, const BlackMisc::CIdentifier &identifier);

                //! VU levels @{
                void inputVolumePeakVU(double value);
                void outputVolumePeakVU(double value);
                //! @}

            protected:
                //! \copydoc BlackMisc::CContinuousWorker::initialize
                virtual void initialize() override;

                //! \copydoc BlackMisc::CContinuousWorker::cleanup
                virtual void cleanup() override;

            private:
                void opusDataAvailable(const Audio::OpusDataAvailableArgs &args);
                void audioOutDataAvailable(const AudioRxOnTransceiversDto &dto);
                void inputVolumeStream(const Audio::InputVolumeStreamArgs &args);
                void outputVolumeStream(const Audio::OutputVolumeStreamArgs &args);
                void inputOpusDataAvailable();

                void onPositionUpdateTimer();
                void onSettingsChanged();

                void updateTransceivers(bool updateFrequencies = true);
                void onUpdateTransceiversFromContext(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

                //! Frequency from aliased stations
                //! \threadsafe
                quint32 getAliasFrequencyHz(quint32 frequencyHz) const;

                static constexpr int PositionUpdatesMs = 5000; //!< position timer
                static constexpr int SampleRate   = 48000;
                static constexpr int FrameSize    =   960;     //!< 20ms
                static constexpr double MinDbIn   = -18.0;
                static constexpr double MaxDbIn   =  18.0;
                static constexpr double MinDbOut  = -60.0;
                static constexpr double MaxDbOut  =  18.0;
                static constexpr quint32 UniCom   = 122800000;

                static quint16 comUnitToTransceiverId(BlackMisc::Aviation::CComSystem::ComUnit comUnit);
                static BlackMisc::Aviation::CComSystem::ComUnit transceiverIdToComUnit(quint16 id);

                Connection::CClientConnection *m_connection = nullptr;
                BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this, &CAfvClient::onSettingsChanged };
                QString m_callsign;

                Audio::CInput *m_input  = nullptr;
                Audio::Output *m_output = nullptr;

                Audio::CSoundcardSampleProvider *m_soundcardSampleProvider = nullptr;
                BlackSound::SampleProvider::CVolumeSampleProvider *m_outputSampleProvider = nullptr;

                std::atomic_bool m_transmit        { false };
                std::atomic_bool m_transmitHistory { false };
                QVector<TxTransceiverDto> m_transmittingTransceivers;
                QVector<TransceiverDto>   m_transceivers;
                QSet<quint16>             m_enabledTransceivers;
                static const QVector<quint16> &allTransceiverIds() { static const QVector<quint16> transceiverIds{0, 1}; return transceiverIds; }

                std::atomic_bool m_isStarted  { false };
                std::atomic_bool m_loopbackOn { false };
                std::atomic_bool m_winCoInitialized { false }; //!< Windows only CoInitializeEx
                QDateTime m_startDateTimeUtc;

                double m_inputVolumeDb;
                double m_outputVolumeDb;
                double m_outputVolume = 1.0;
                double m_maxDbReadingInPTTInterval = -100;

                QTimer             *m_voiceServerPositionTimer = nullptr;
                QVector<StationDto> m_aliasedStations;

                Audio::InputVolumeStreamArgs  m_inputVolumeStream;
                Audio::OutputVolumeStreamArgs m_outputVolumeStream;

                void deferredInit();
                void initTransceivers();
                void connectWithContexts();
                static bool hasContext();

                std::atomic_bool m_connectedWithContext { false };

                mutable QMutex m_mutex;
                mutable QMutex m_mutexInputStream;
                mutable QMutex m_mutexOutputStream;
                mutable QMutex m_mutexTransceivers;
                mutable QMutex m_mutexCallsign;
            };
        } // ns
    } // ns
} // ns

#endif
