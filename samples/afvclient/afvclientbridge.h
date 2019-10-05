#ifndef AFVCLIENTBRIDGE_H
#define AFVCLIENTBRIDGE_H

#include "blackcore/afv/clients/afvclient.h"
#include <QObject>

class CAfvClientBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double inputVolumePeakVU  READ getInputVolumePeakVU  NOTIFY inputVolumePeakVU)
    Q_PROPERTY(double outputVolumePeakVU READ getOutputVolumePeakVU NOTIFY outputVolumePeakVU)
    Q_PROPERTY(BlackCore::Afv::Clients::CAfvClient::ConnectionStatus connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString receivingCallsignsCom1 READ getReceivingCallsignsCom1 NOTIFY receivingCallsignsChanged)
    Q_PROPERTY(QString receivingCallsignsCom2 READ getReceivingCallsignsCom2 NOTIFY receivingCallsignsChanged)

public:
    CAfvClientBridge(BlackCore::Afv::Clients::CAfvClient *afvClient, QObject *parent = nullptr);

    double getInputVolumePeakVU() const { return m_afvClient->getInputVolumePeakVU(); }
    double getOutputVolumePeakVU() const { return m_afvClient->getOutputVolumePeakVU(); }

    BlackCore::Afv::Clients::CAfvClient::ConnectionStatus getConnectionStatus() const
    {
        return m_afvClient->getConnectionStatus();
    }

    QString getReceivingCallsignsCom1() { return m_afvClient->getReceivingCallsignsCom1(); }
    QString getReceivingCallsignsCom2() { return m_afvClient->getReceivingCallsignsCom2(); }

    Q_INVOKABLE void connectTo(const QString &cid, const QString &password, const QString &callsign)
    {
        m_afvClient->connectTo(cid, password, callsign);
    }

    Q_INVOKABLE void disconnectFrom() { m_afvClient->disconnectFrom(); }

    //! Audio devices @{
    Q_INVOKABLE QStringList availableInputDevices() const { return m_afvClient->availableInputDevices(); }
    Q_INVOKABLE QStringList availableOutputDevices() const { return m_afvClient->availableOutputDevices(); }
    //! @}

    //! Enable/disable VHF simulation, true means effects are NOT used
    Q_INVOKABLE void setBypassEffects(bool value) { /*m_afvClient->setBypassEffects(value);*/ }

    Q_INVOKABLE void startAudio(const QString &inputDeviceName, const QString &outputDeviceName) { m_afvClient->startAudio(inputDeviceName, outputDeviceName); }

    Q_INVOKABLE void enableTransceiver(quint16 id, bool enable) { /*m_afvClient->enableTransceiver(id, enable);*/ }

    Q_INVOKABLE void updateComFrequency(quint16 id, quint32 frequencyHz) { m_afvClient->updateComFrequency(id, frequencyHz); }

    //! Update own aircraft position
    Q_INVOKABLE void updatePosition(double latitudeDeg, double longitudeDeg, double heightMeters)
    {
        m_afvClient->updatePosition(latitudeDeg, longitudeDeg, heightMeters);
    }

    //! Push to talk @{
    Q_INVOKABLE void setPtt(bool active) { m_afvClient->setPtt(active); }
    //! @}

    //! Loopback @{
    Q_INVOKABLE void setLoopBack(bool on) { m_afvClient->setLoopBack(on); }
    Q_INVOKABLE bool isLoopback() const { return false; m_afvClient->isLoopback(); }
    //! @}

    //! Input volume in dB, +-18dB @{
    Q_INVOKABLE void setInputVolumeDb(double valueDb) { /*m_afvClient->setInputVolumeDb(valueDb);*/ }
    //! @}

    //! Output volume in dB, +-18dB @{
    Q_INVOKABLE void setOutputVolumeDb(double valueDb) { /*m_afvClient->setOutputVolumeDb(valueDb);*/ }
    //! @}

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

    //! VU levels @{
    void inputVolumePeakVU(double value);
    void outputVolumePeakVU(double value);
    //! @}

private:
    BlackCore::Afv::Clients::CAfvClient *m_afvClient = nullptr;
};

#endif // CAFVCLIENTBRIDGE_H
