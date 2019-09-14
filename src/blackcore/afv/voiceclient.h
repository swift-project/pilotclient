#ifndef VOICECLIENT_H
#define VOICECLIENT_H

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "dto.h"
#include "cryptodtochannel.h"
#include "opusdecoder.h"
#include "opusencoder.h"
#include "audio/aircraftvhfsampleprovider.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QTimer>
#include <QUdpSocket>
#include <QScopedPointer>
#include <QAudioInput>
#include <QAudioOutput>

class AudioInputBuffer : public QIODevice
{
    Q_OBJECT

public:
    AudioInputBuffer() {}

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

signals:
    void frameAvailable(const QByteArray &frame);

private:
    static constexpr qint64 frameSize = 960;
    QByteArray m_buffer;
};


class VoiceClient : public QObject
{
    Q_OBJECT
public:
    VoiceClient(QObject *parent = nullptr);
    ~VoiceClient();

    void authenticate(const QString &apiServer, const QString &cid, const QString &password, const QString &callsign);
    void updateTransceivers(const QVector<RadioTransceiverDto> &radioTransceivers);
    void start(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice);
    bool isStarted() const { return m_isStarted; }
    void setBypassEffects(bool value) { if(m_audioSampleProvider) { m_audioSampleProvider->setBypassEffects(value); } }
    void setPtt(bool transmit)
    {
        qDebug() << "setPtt(" << transmit << ")";
        m_transmit = transmit;
    }
    void setLoopBack(bool on) { m_loopbackOn = on; }

    void setTransmittingTransceivers(const QStringList &transceiverNames);

signals:
    void isAuthenticated();

private:
    void authRequestFinished(QNetworkReply *reply);
    void readPendingDatagrams();
    void processMessage(const QByteArray &messageDdata, bool loopback = false);

    void sendVoiceServerKeepAlive();
    void handleSocketError(QAbstractSocket::SocketError error);

    void initializeAudio(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice);

    void audioInDataAvailable(const QByteArray &frame);

    QVector<qint16> convertBytesTo16BitPCM(const QByteArray input);

    // Tasks
    QTimer timerVoiceServerKeepAlive;
    QTimer timerVoiceServerTransmit;

    bool clientAuthenticated = false;
    AuthResponseDto clientAuthenticationData;
    CryptoDtoChannel *voiceCryptoChannel;
    CryptoDtoChannel *dataCryptoChannel;

    QString dataServerAddress;

    zmq::context_t m_context;
    zmq::socket_t m_dealerSocket;

    QNetworkAccessManager m_nam;
    QNetworkReply *m_reply = nullptr;

    QString m_callsign;

    QVector<RadioTransceiverDto> m_radioTransceivers;

    QUdpSocket m_udpSocket;

    COpusDecoder decoder;
    COpusEncoder encoder;

    QScopedPointer<QAudioInput> m_audioInput;
    AudioInputBuffer m_audioInputBuffer;
    QScopedPointer<QAudioOutput> m_audioOutput;
    QScopedPointer<AircraftVHFSampleProvider> m_audioSampleProvider;

    bool m_isStarted = false;

    QByteArray m_notEncodedBuffer;

    int frameSize = 960;

    QStringList m_transmittingTransceiverNames;
    bool m_transmit = false;
    bool m_transmitHistory = false;
    uint audioSequenceCounter = 0;

    bool m_loopbackOn = false;
};

#endif // VOICECLIENT_H
