// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SAMPLES_AFVCLIENTBRIDGE_H
#define SAMPLES_AFVCLIENTBRIDGE_H

#include <QObject>

#include "core/afv/clients/afvclient.h"

//! Bridge, running in main thread -> voice client in own thread
class CAfvClientBridge : public QObject
{
    Q_OBJECT

    //! Bridge properties
    //! @{
    Q_PROPERTY(double inputVolumePeakVU READ getInputVolumePeakVU NOTIFY inputVolumePeakVU)
    Q_PROPERTY(double outputVolumePeakVU READ getOutputVolumePeakVU NOTIFY outputVolumePeakVU)
    Q_PROPERTY(swift::core::afv::clients::CAfvClient::ConnectionStatus connectionStatus READ getConnectionStatus NOTIFY
                   connectionStatusChanged)
    Q_PROPERTY(QString receivingCallsignsCom1 READ getReceivingCallsignsCom1 NOTIFY receivingCallsignsChanged)
    Q_PROPERTY(QString receivingCallsignsCom2 READ getReceivingCallsignsCom2 NOTIFY receivingCallsignsChanged)
    //! @}

public:
    //! Ctor
    CAfvClientBridge(swift::core::afv::clients::CAfvClient *afvClient, QObject *parent = nullptr);

    //! VU values, 0..1
    //! @{
    double getInputVolumePeakVU() const { return m_afvClient->getInputVolumePeakVU(); }
    double getOutputVolumePeakVU() const { return m_afvClient->getOutputVolumePeakVU(); }
    //! @}

    //! \copydoc swift::core::afv::clients::CAfvClient::getConnectionStatus
    swift::core::afv::clients::CAfvClient::ConnectionStatus getConnectionStatus() const
    {
        return m_afvClient->getConnectionStatus();
    }

    //! Callsigns currently received
    //! @{
    QString getReceivingCallsignsCom1() { return m_afvClient->getReceivingCallsignsStringCom1(); }
    QString getReceivingCallsignsCom2() { return m_afvClient->getReceivingCallsignsStringCom2(); }
    //! @}

    //! \copydoc swift::core::afv::clients::CAfvClient::connectTo
    Q_INVOKABLE void connectTo(const QString &cid, const QString &password, const QString &callsign)
    {
        const QString client = "swift " % swift::config::CBuildConfig::getShortVersionString();
        m_afvClient->connectTo(cid, password, callsign, client);
    }

    //! \copydoc swift::core::afv::clients::CAfvClient::disconnectFrom
    Q_INVOKABLE void disconnectFrom() { m_afvClient->disconnectFrom(); }

    //! Audio devices
    //! @{
    Q_INVOKABLE QStringList availableInputDevices() const { return m_afvClient->availableInputDevices(); }
    Q_INVOKABLE QStringList availableOutputDevices() const { return m_afvClient->availableOutputDevices(); }
    //! @}

    //! Enable/disable VHF simulation, true means effects are NOT used
    Q_INVOKABLE void setBypassEffects(bool value) { m_afvClient->setBypassEffects(value); }

    //! \copydoc swift::core::afv::clients::CAfvClient::startAudio
    Q_INVOKABLE void startAudio(const QString &inputDeviceName, const QString &outputDeviceName)
    {
        m_afvClient->startAudio(inputDeviceName, outputDeviceName);
    }

    //! \copydoc swift::core::afv::clients::CAfvClient::enableTransceiver
    Q_INVOKABLE void enableTransceiver(quint16 id, bool enable) { m_afvClient->enableTransceiver(id, enable); }

    //! \copydoc swift::core::afv::clients::CAfvClient::updateComFrequency
    Q_INVOKABLE void updateComFrequency(quint16 id, quint32 frequencyHz)
    {
        m_afvClient->updateComFrequency(id, frequencyHz);
    }

    //! \copydoc swift::core::afv::clients::CAfvClient::updatePosition
    Q_INVOKABLE void updatePosition(double latitudeDeg, double longitudeDeg, double heightMeters)
    {
        m_afvClient->updatePosition(latitudeDeg, longitudeDeg, heightMeters);
    }

    //! \copydoc swift::core::afv::clients::CAfvClient::setPtt
    Q_INVOKABLE void setPtt(bool active) { m_afvClient->setPtt(active); }

    //! Loopback
    //! @{
    Q_INVOKABLE void setLoopBack(bool on) { m_afvClient->setLoopBack(on); }
    Q_INVOKABLE bool isLoopback() const { return m_afvClient->isLoopback(); }
    //! @}

    //! \copydoc swift::core::afv::clients::CAfvClient::setInputVolumeDb
    Q_INVOKABLE void setInputVolumeDb(double valueDb) { m_afvClient->setInputVolumeDb(valueDb); }

    //! \copydoc swift::core::afv::clients::CAfvClient::setNormalizedMasterOutputVolume
    Q_INVOKABLE void setNormalizedMasterOutputVolume(double valueDb)
    {
        m_afvClient->setNormalizedMasterOutputVolume(valueDb);
    }

signals:
    //! Receiving callsigns have been changed
    //! \remark callsigns I do receive
    void receivingCallsignsChanged(const swift::core::afv::audio::TransceiverReceivingCallsignsChangedArgs &args);

    //! Connection status has been changed
    void connectionStatusChanged(swift::core::afv::clients::CAfvClient::ConnectionStatus status);

    //! Client updated from own aicraft data
    void updatedFromOwnAircraftCockpit();

    //! PTT status in this particular AFV client
    void ptt(bool active, const swift::misc::CIdentifier &identifier);

    //! VU levels
    //! @{
    void inputVolumePeakVU(double value);
    void outputVolumePeakVU(double value);
    //! @}

private:
    swift::core::afv::clients::CAfvClient *m_afvClient = nullptr;
};

#endif // guard
