/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef SAMPLES_AFVCLIENTBRIDGE_H
#define SAMPLES_AFVCLIENTBRIDGE_H

#include "blackcore/afv/clients/afvclient.h"
#include <QObject>

//! Bridge, running in main thread -> voice client in own thread
class CAfvClientBridge : public QObject
{
    Q_OBJECT

    //! Bridge properties
    //! @{
    Q_PROPERTY(double inputVolumePeakVU READ getInputVolumePeakVU NOTIFY inputVolumePeakVU)
    Q_PROPERTY(double outputVolumePeakVU READ getOutputVolumePeakVU NOTIFY outputVolumePeakVU)
    Q_PROPERTY(BlackCore::Afv::Clients::CAfvClient::ConnectionStatus connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString receivingCallsignsCom1 READ getReceivingCallsignsCom1 NOTIFY receivingCallsignsChanged)
    Q_PROPERTY(QString receivingCallsignsCom2 READ getReceivingCallsignsCom2 NOTIFY receivingCallsignsChanged)
    //! @}

public:
    //! Ctor
    CAfvClientBridge(BlackCore::Afv::Clients::CAfvClient *afvClient, QObject *parent = nullptr);

    //! VU values, 0..1
    //! @{
    double getInputVolumePeakVU() const { return m_afvClient->getInputVolumePeakVU(); }
    double getOutputVolumePeakVU() const { return m_afvClient->getOutputVolumePeakVU(); }
    //! @}

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::getConnectionStatus
    BlackCore::Afv::Clients::CAfvClient::ConnectionStatus getConnectionStatus() const
    {
        return m_afvClient->getConnectionStatus();
    }

    //! Callsigns currently received
    //! @{
    QString getReceivingCallsignsCom1() { return m_afvClient->getReceivingCallsignsStringCom1(); }
    QString getReceivingCallsignsCom2() { return m_afvClient->getReceivingCallsignsStringCom2(); }
    //! @}

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::connectTo
    Q_INVOKABLE void connectTo(const QString &cid, const QString &password, const QString &callsign)
    {
        const QString client = "swift " % BlackConfig::CBuildConfig::getShortVersionString();
        m_afvClient->connectTo(cid, password, callsign, client);
    }

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::disconnectFrom
    Q_INVOKABLE void disconnectFrom() { m_afvClient->disconnectFrom(); }

    //! Audio devices
    //! @{
    Q_INVOKABLE QStringList availableInputDevices() const { return m_afvClient->availableInputDevices(); }
    Q_INVOKABLE QStringList availableOutputDevices() const { return m_afvClient->availableOutputDevices(); }
    //! @}

    //! Enable/disable VHF simulation, true means effects are NOT used
    Q_INVOKABLE void setBypassEffects(bool value) { m_afvClient->setBypassEffects(value); }

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::startAudio
    Q_INVOKABLE void startAudio(const QString &inputDeviceName, const QString &outputDeviceName) { m_afvClient->startAudio(inputDeviceName, outputDeviceName); }

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::enableTransceiver
    Q_INVOKABLE void enableTransceiver(quint16 id, bool enable) { m_afvClient->enableTransceiver(id, enable); }

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::updateComFrequency
    Q_INVOKABLE void updateComFrequency(quint16 id, quint32 frequencyHz) { m_afvClient->updateComFrequency(id, frequencyHz); }

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::updatePosition
    Q_INVOKABLE void updatePosition(double latitudeDeg, double longitudeDeg, double heightMeters)
    {
        m_afvClient->updatePosition(latitudeDeg, longitudeDeg, heightMeters);
    }

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::setPtt
    Q_INVOKABLE void setPtt(bool active) { m_afvClient->setPtt(active); }

    //! Loopback
    //! @{
    Q_INVOKABLE void setLoopBack(bool on) { m_afvClient->setLoopBack(on); }
    Q_INVOKABLE bool isLoopback() const { return m_afvClient->isLoopback(); }
    //! @}

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::setInputVolumeDb
    Q_INVOKABLE void setInputVolumeDb(double valueDb) { m_afvClient->setInputVolumeDb(valueDb); }

    //! \copydoc BlackCore::Afv::Clients::CAfvClient::setNormalizedMasterOutputVolume
    Q_INVOKABLE void setNormalizedMasterOutputVolume(double valueDb) { m_afvClient->setNormalizedMasterOutputVolume(valueDb); }

signals:
    //! Receiving callsigns have been changed
    //! \remark callsigns I do receive
    void receivingCallsignsChanged(const BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs &args);

    //! Connection status has been changed
    void connectionStatusChanged(BlackCore::Afv::Clients::CAfvClient::ConnectionStatus status);

    //! Client updated from own aicraft data
    void updatedFromOwnAircraftCockpit();

    //! PTT status in this particular AFV client
    void ptt(bool active, BlackMisc::Audio::PTTCOM pttcom, const BlackMisc::CIdentifier &identifier);

    //! VU levels
    //! @{
    void inputVolumePeakVU(double value);
    void outputVolumePeakVU(double value);
    //! @}

private:
    BlackCore::Afv::Clients::CAfvClient *m_afvClient = nullptr;
};

#endif // guard
