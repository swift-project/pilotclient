#include "voiceclient.h"
#include "cryptodtoserializer.h"
#include "dto.h"
#include "constants.h"

#include <QFile>
#include <QAudio>
#include <QUdpSocket>
#include <QNetworkDatagram>

void AudioInputBuffer::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInputBuffer::stop()
{
    close();
}

qint64 AudioInputBuffer::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 AudioInputBuffer::writeData(const char *data, qint64 len)
{
    QByteArray buffer(data, static_cast<int>(len));
    m_buffer.append(buffer);
    // 20 ms = 960 samples * 2 bytes = 1920 Bytes
    if (m_buffer.size() >= 1920)
    {
        emit frameAvailable(m_buffer.left(1920));
        m_buffer.remove(0, 1920);
    }

    return len;
}

VoiceClient::VoiceClient(QObject *parent) :
    QObject(parent),
    m_context(),
    m_dealerSocket(m_context, zmq::socket_type::dealer),
    decoder(c_sampleRate, c_channelCount),
    encoder(c_sampleRate, c_channelCount)
{
    //m_dealerSocket.setsockopt(ZMQ_IDENTITY, "test");
}

VoiceClient::~VoiceClient()
{
}

void VoiceClient::authenticate(const QString &apiServer, const QString &cid, const QString &password, const QString &callsign)
{
    m_callsign = callsign;
    QUrl url(apiServer);
    url.setPath("/api/v1/auth");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj
    {
        {"cID", cid},
        {"password", password},
        {"callsign", callsign}
    };

    connect(&m_nam, &QNetworkAccessManager::finished, this, &VoiceClient::authRequestFinished);
    m_reply = m_nam.post(request, QJsonDocument(obj).toJson());
}

void VoiceClient::updateTransceivers(const QVector<RadioTransceiverDto> &radioTransceivers)
{
    if (!clientAuthenticated) { return; }

    m_radioTransceivers = radioTransceivers;

    RadioTransceiverUpdateDto updateDto;
    updateDto.callsign = m_callsign.toStdString();
    updateDto.radioTransceivers = m_radioTransceivers.toStdVector();
    QByteArray cryptoBytes = CryptoDtoSerializer::Serialize(*dataCryptoChannel, CryptoDtoMode::HMAC_SHA256, updateDto);

    zmq::multipart_t sendMessages;
    sendMessages.addstr("");
    sendMessages.addmem(cryptoBytes.data(), cryptoBytes.size());
    sendMessages.send(m_dealerSocket);

    zmq::multipart_t multipart;
    multipart.recv(m_dealerSocket);

    if (multipart.size() == 2)
    {
        zmq::message_t &msg = multipart.at(1);
        QByteArray data(msg.data<const char>(), msg.size());
        CryptoDtoSerializer::Deserializer deserializer = CryptoDtoSerializer::deserialize(*dataCryptoChannel, data, false);
        RadioTransceiverUpdateAckDto ack = deserializer.getDto<RadioTransceiverUpdateAckDto>();

        if(ack.success)
        {
            clientAuthenticated = true;
        }
    }
}

void VoiceClient::start(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice)
{
     if (!clientAuthenticated) { return; }
    connect(&m_udpSocket, &QUdpSocket::readyRead, this, &VoiceClient::readPendingDatagrams);
    connect(&m_udpSocket, qOverload<QAbstractSocket::SocketError>(&QUdpSocket::error), this, &VoiceClient::handleSocketError);

    initializeAudio(inputDevice, outputDevice);

    QHostAddress localAddress(QHostAddress::AnyIPv4);
    m_udpSocket.bind(localAddress);
    sendVoiceServerKeepAlive();
    connect(&timerVoiceServerKeepAlive, &QTimer::timeout, this, &VoiceClient::sendVoiceServerKeepAlive);
    timerVoiceServerKeepAlive.start(5000);

    m_isStarted = true;
}

void VoiceClient::setTransmittingTransceivers(const QStringList &transceiverNames)
{
    m_transmittingTransceiverNames.clear();
    for (const QString &transceiverName : transceiverNames)
    {
        auto it = std::find_if(m_radioTransceivers.begin(), m_radioTransceivers.end(), [&transceiverName](const RadioTransceiverDto &receiver)
        {
            return receiver.name == transceiverName.toStdString();
        });

        if (it != m_radioTransceivers.end())
        {
            m_transmittingTransceiverNames.push_back(transceiverName);
        }
    }
}

void VoiceClient::authRequestFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << m_reply->errorString();
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(m_reply->readAll());
    clientAuthenticationData = AuthResponseDto::fromJson(doc.object());
    m_reply->deleteLater();

    voiceCryptoChannel = new CryptoDtoChannel(clientAuthenticationData.VoiceServer.channelConfig);
    dataCryptoChannel = new CryptoDtoChannel(clientAuthenticationData.DataServer.channelConfig);

    dataServerAddress = QString("tcp://" + clientAuthenticationData.DataServer.addressIpV4);
    m_dealerSocket.connect(dataServerAddress.toStdString());

    ClientDataHeartbeatDto heartBeatDto;
    heartBeatDto.callsign = m_callsign.toStdString();
    QByteArray cryptoBytes = CryptoDtoSerializer::Serialize(*dataCryptoChannel, CryptoDtoMode::AEAD_ChaCha20Poly1305, heartBeatDto);

    zmq::multipart_t sendMessages;
    sendMessages.addstr("");
    sendMessages.addmem(cryptoBytes.data(), cryptoBytes.size());
    sendMessages.send(m_dealerSocket);

    zmq::multipart_t multipart;
    multipart.recv(m_dealerSocket);

    if (multipart.size() == 2)
    {
        zmq::message_t &msg = multipart.at(1);
        QByteArray data(msg.data<const char>(), msg.size());
        CryptoDtoSerializer::Deserializer deserializer = CryptoDtoSerializer::deserialize(*dataCryptoChannel, data, false);
        ClientDataHeartbeatAckDto ack = deserializer.getDto<ClientDataHeartbeatAckDto>();

        if(ack.success)
        {
            clientAuthenticated = true;
            emit isAuthenticated();
        }
    }
}

void VoiceClient::readPendingDatagrams()
{
    while (m_udpSocket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_udpSocket.receiveDatagram();
        processMessage(datagram.data());
    }
}

void VoiceClient::processMessage(const QByteArray &messageDdata, bool loopback)
{
    CryptoDtoSerializer::Deserializer deserializer = CryptoDtoSerializer::deserialize(*voiceCryptoChannel, messageDdata, loopback);
    if(deserializer.dtoNameBuffer == "VHA")
    {
        ClientVoiceHeartbeatAckDto ack = deserializer.getDto<ClientVoiceHeartbeatAckDto>();
        Q_UNUSED(ack);
    }
    else if(deserializer.dtoNameBuffer == "AT")
    {
        AudioOnTransceiversDto at = deserializer.getDto<AudioOnTransceiversDto>();
        QStringList transeiverNames;
        for (const auto &transeiverName : at.transceiverNames)
        {
            transeiverNames.append(QString::fromStdString(transeiverName));
        }

        QByteArray audio(at.audio.data(), at.audio.size());
        int decodedLength = 0;
        QVector<qint16> decoded = decoder.decode(audio, audio.size(), &decodedLength);
        m_audioSampleProvider->addSamples(decoded, QString::fromStdString(at.callsign));
    }
    else
    {
        qDebug() << "Received unknown data:" << deserializer.dtoNameBuffer << deserializer.dataLength;
    }
}

void VoiceClient::sendVoiceServerKeepAlive()
{
    ClientVoiceHeartbeatDto keepAlive;
    QUrl voiceServerUrl("udp://" + clientAuthenticationData.VoiceServer.addressIpV4);
    QByteArray dataBytes = CryptoDtoSerializer::Serialize(*voiceCryptoChannel, CryptoDtoMode::AEAD_ChaCha20Poly1305, keepAlive);
    m_udpSocket.writeDatagram(dataBytes, QHostAddress(voiceServerUrl.host()), voiceServerUrl.port());
}

void VoiceClient::handleSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    qDebug() << "UDP socket error" << m_udpSocket.errorString();
}

void VoiceClient::initializeAudio(const QAudioDeviceInfo &inputDevice, const QAudioDeviceInfo &outputDevice)
{
    QAudioFormat format;
    format.setSampleRate(c_sampleRate);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    if (!inputDevice.isFormatSupported(format))
    {
        qWarning() << "Default format not supported - trying to use nearest";
        format = inputDevice.nearestFormat(format);
    }

    // m_recorder.start();
    m_audioInput.reset(new QAudioInput(inputDevice, format));
    // We want 20 ms of buffer size
    // 20 ms * nSamplesPerSec × nChannels × wBitsPerSample  / 8 x 1000
    int bufferSize = 20 * format.sampleRate() * format.channelCount() * format.sampleSize() / ( 8 * 1000 );
    m_audioInput->setBufferSize(bufferSize);
    m_audioInputBuffer.start();
    m_audioInput->start(&m_audioInputBuffer);
    connect(&m_audioInputBuffer, &AudioInputBuffer::frameAvailable, this, &VoiceClient::audioInDataAvailable);
    connect(m_audioInput.data(), &QAudioInput::stateChanged, [&] (QAudio::State state) { qDebug() << "QAudioInput changed state to" << state; });

    if (!outputDevice.isFormatSupported(format))
    {
        qWarning() << "Default format not supported - trying to use nearest";
        format = outputDevice.nearestFormat(format);
    }

    m_audioOutput.reset(new QAudioOutput(outputDevice, format));
    // m_audioOutput->setBufferSize(bufferSize);
    m_audioSampleProvider.reset(new AircraftVHFSampleProvider(format, 4, 0.1));
    m_audioSampleProvider->open(QIODevice::ReadWrite | QIODevice::Unbuffered);

    m_audioOutput->start(m_audioSampleProvider.data());
}

void VoiceClient::audioInDataAvailable(const QByteArray &frame)
{
    QVector<qint16> samples = convertBytesTo16BitPCM(frame);

    int length;
    QByteArray encodedBuffer = encoder.encode(samples, samples.size(), &length);

    if (m_transmittingTransceiverNames.size() > 0 && m_isStarted)
    {
        if (m_transmit)
        {
            AudioOnTransceiversDto dto;
            dto.callsign = m_callsign.toStdString();
            dto.sequenceCounter = audioSequenceCounter++;
            dto.audio = std::vector<char>(encodedBuffer.begin(), encodedBuffer.end());
            dto.lastPacket = false;
            for (const QString &transceiverName : m_transmittingTransceiverNames)
            {
                dto.transceiverNames.push_back(transceiverName.toStdString());
            }

            QUrl voiceServerUrl("udp://" + clientAuthenticationData.VoiceServer.addressIpV4);
            QByteArray dataBytes = CryptoDtoSerializer::Serialize(*voiceCryptoChannel, CryptoDtoMode::AEAD_ChaCha20Poly1305, dto);
            if (m_loopbackOn) { processMessage(dataBytes, true); }
            else { m_udpSocket.writeDatagram(dataBytes, QHostAddress(voiceServerUrl.host()), voiceServerUrl.port()); }
        }

        if (!m_transmit && m_transmitHistory)
        {
            AudioOnTransceiversDto dto;
            dto.callsign = m_callsign.toStdString();
            dto.sequenceCounter = audioSequenceCounter++;
            dto.audio = std::vector<char>(encodedBuffer.begin(), encodedBuffer.end());
            dto.lastPacket = true;
            for (const QString &transceiverName : m_transmittingTransceiverNames)
            {
                dto.transceiverNames.push_back(transceiverName.toStdString());
            }

            QUrl voiceServerUrl("udp://" + clientAuthenticationData.VoiceServer.addressIpV4);
            QByteArray dataBytes = CryptoDtoSerializer::Serialize(*voiceCryptoChannel, CryptoDtoMode::AEAD_ChaCha20Poly1305, dto);
            if (m_loopbackOn) { processMessage(dataBytes, true); }
            else { m_udpSocket.writeDatagram(dataBytes, QHostAddress(voiceServerUrl.host()), voiceServerUrl.port()); }
        }

        m_transmitHistory = m_transmit;
    }
}

QVector<qint16> VoiceClient::convertBytesTo16BitPCM(const QByteArray input)
{
    int inputSamples = input.size() / 2; // 16 bit input, so 2 bytes per sample
    QVector<qint16> output;
    output.fill(0, inputSamples);

    for (int n = 0; n < inputSamples; n++)
    {
        output[n] = *reinterpret_cast<const qint16*>(input.data() + n * 2);
    }
    return output;
}
