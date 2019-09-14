#ifndef CRYPTODTOSERIALIZER_H
#define CRYPTODTOSERIALIZER_H

#include "cryptodtochannel.h"
#include "cryptodtomode.h"
#include "cryptodtoheaderdto.h"
#include "sodium.h"

#include <QByteArray>
#include <QBuffer>
#include <QtDebug>

extern QHash<QByteArray, QByteArray> gShortDtoNames;

class CryptoDtoSerializer
{
public:
    CryptoDtoSerializer();

    template<typename T>
    static QByteArray Serialize(const QString &channelTag, CryptoDtoMode mode, const QByteArray &transmitKey, uint sequenceToBeSent, T dto)
    {
        const CryptoDtoHeaderDto header = { channelTag.toStdString(), sequenceToBeSent, mode };

        QBuffer headerBuffer;
        headerBuffer.open(QIODevice::WriteOnly);
        msgpack::pack(headerBuffer, header);
        headerBuffer.close();
        const quint16 headerLength = static_cast<quint16>(headerBuffer.buffer().size());

        const QByteArray dtoNameBuffer = T::getDtoName();
        const QByteArray dtoShortName = T::getShortDtoName();
        const quint16 dtoNameLength = static_cast<quint16>(dtoShortName.size());

        QBuffer dtoBuffer;
        dtoBuffer.open(QIODevice::WriteOnly);
        msgpack::pack(dtoBuffer, dto);
        dtoBuffer.close();
        const quint16 dtoLength = static_cast<quint16>(dtoBuffer.buffer().size());

        if(header.Mode == CryptoDtoMode::AEAD_ChaCha20Poly1305)
        {
            QBuffer aePayloadBuffer;
            aePayloadBuffer.open(QIODevice::WriteOnly);
            aePayloadBuffer.write(reinterpret_cast<const char *>(&dtoNameLength), sizeof(dtoNameLength));
            aePayloadBuffer.write(dtoShortName);
            aePayloadBuffer.write(reinterpret_cast<const char *>(&dtoLength), sizeof(dtoLength));
            aePayloadBuffer.write(dtoBuffer.buffer());
            aePayloadBuffer.close();

            QBuffer adPayloadBuffer;
            adPayloadBuffer.open(QIODevice::WriteOnly);
            adPayloadBuffer.write(reinterpret_cast<const char *>(&headerLength), sizeof(headerLength));
            adPayloadBuffer.write(headerBuffer.buffer());
            adPayloadBuffer.close();

            QByteArray nonce;
            nonce.fill(0, crypto_aead_chacha20poly1305_IETF_NPUBBYTES);
            QBuffer nonceBuffer(&nonce);
            nonceBuffer.open(QIODevice::WriteOnly);
            uint32_t id = 0;
            nonceBuffer.write(reinterpret_cast<const char *>(&id), sizeof(id));
            nonceBuffer.write(reinterpret_cast<const char *>(&header.Sequence), sizeof(header.Sequence));
            nonceBuffer.close();

            unsigned long long clen;
            QByteArray aeadPayload;
            aeadPayload.fill(0, static_cast<int>(aePayloadBuffer.size() + crypto_aead_chacha20poly1305_IETF_ABYTES));
            int result = crypto_aead_chacha20poly1305_ietf_encrypt(reinterpret_cast<unsigned char*>(aeadPayload.data()),
                                                      &clen,
                                                      reinterpret_cast<const unsigned char*>(aePayloadBuffer.buffer().constData()), aePayloadBuffer.size(),
                                                      reinterpret_cast<const unsigned char*>(adPayloadBuffer.buffer().constData()), adPayloadBuffer.size(),
                                                      nullptr,
                                                      reinterpret_cast<const unsigned char*>(nonce.constData()),
                                                      reinterpret_cast<const unsigned char*>(transmitKey.constData()));
            if (result != 0) { return {}; }

            QBuffer packetBuffer;
            packetBuffer.open(QIODevice::WriteOnly);
            packetBuffer.write(reinterpret_cast<const char *>(&headerLength), sizeof(headerLength));
            packetBuffer.write(headerBuffer.buffer());
            packetBuffer.write(aeadPayload);
            packetBuffer.close();

            return packetBuffer.buffer();
        }

        return {};
    }

    template<typename T>
    static QByteArray Serialize(CryptoDtoChannel &channel, CryptoDtoMode mode, T dto)
    {
        uint sequenceToSend = 0;
        QByteArray transmitKey = channel.getTransmitKey(mode, sequenceToSend);
        return Serialize(channel.getChannelTag(), mode, transmitKey, sequenceToSend++, dto);
    }

    struct Deserializer
    {
        Deserializer(CryptoDtoChannel &channel, const QByteArray &bytes, bool loopback);

        template<typename T>
        T getDto()
        {
            if (! verified) return {};
            if (dtoNameBuffer == T::getDtoName() || dtoNameBuffer == T::getShortDtoName())
            {
                msgpack::object_handle oh2 = msgpack::unpack(dataBuffer.data(), dataBuffer.size());
                msgpack::object obj = oh2.get();
                T dto = obj.as<T>();
                return dto;
            }
            return {};
        }

        quint16 headerLength;
        CryptoDtoHeaderDto header;

        quint16 dtoNameLength;
        QByteArray dtoNameBuffer;

        quint16 dataLength;
        QByteArray dataBuffer;

        bool verified = false;
    };

    static Deserializer deserialize(CryptoDtoChannel &channel, const QByteArray &bytes, bool loopback);
};

#endif // CRYPTODTOSERIALIZER_H
