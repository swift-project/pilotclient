/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CRYPTO_CRYPTODTO_SERIALIZER_H
#define BLACKCORE_AFV_CRYPTO_CRYPTODTO_SERIALIZER_H

#include "blackcore/afv/crypto/cryptodtochannel.h"
#include "blackcore/afv/crypto/cryptodtomode.h"
#include "blackcore/afv/crypto/cryptodtoheaderdto.h"
#include "sodium.h"

#include <QByteArray>
#include <QBuffer>
#include <QtDebug>

#ifndef crypto_aead_chacha20poly1305_IETF_ABYTES
//! Number of a bytes
#    define crypto_aead_chacha20poly1305_IETF_ABYTES 16U
#endif

namespace BlackCore::Afv::Crypto
{
    //! Hash of AFV short dto names
    extern QHash<QByteArray, QByteArray> gShortDtoNames;

    //! Crypto serializer
    class CryptoDtoSerializer
    {
    public:
        CryptoDtoSerializer();

        //! Serialize a DTO
        template <typename T>
        static QByteArray serialize(const QString &channelTag, CryptoDtoMode mode, const QByteArray &transmitKey, uint sequenceToBeSent, T dto)
        {
            const CryptoDtoHeaderDto header = { channelTag.toStdString(), sequenceToBeSent, mode };

            QBuffer headerBuffer;
            headerBuffer.open(QIODevice::WriteOnly);
            msgpack::pack(headerBuffer, header);
            headerBuffer.close();
            const quint16 headerLength = static_cast<quint16>(headerBuffer.buffer().size());

            const QByteArray dtoShortName = T::getShortDtoName();
            const quint16 dtoNameLength = static_cast<quint16>(dtoShortName.size());

            QBuffer dtoBuffer;
            dtoBuffer.open(QIODevice::WriteOnly);
            msgpack::pack(dtoBuffer, dto);
            dtoBuffer.close();
            const quint16 dtoLength = static_cast<quint16>(dtoBuffer.buffer().size());

            if (header.Mode == CryptoDtoMode::AEAD_ChaCha20Poly1305)
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
                int result = crypto_aead_chacha20poly1305_ietf_encrypt(reinterpret_cast<unsigned char *>(aeadPayload.data()),
                                                                       &clen,
                                                                       reinterpret_cast<const unsigned char *>(aePayloadBuffer.buffer().constData()), aePayloadBuffer.size(),
                                                                       reinterpret_cast<const unsigned char *>(adPayloadBuffer.buffer().constData()), adPayloadBuffer.size(),
                                                                       nullptr,
                                                                       reinterpret_cast<const unsigned char *>(nonce.constData()),
                                                                       reinterpret_cast<const unsigned char *>(transmitKey.constData()));
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

        //! Serialize a DTO
        template <typename T>
        static QByteArray serialize(CCryptoDtoChannel &channel, CryptoDtoMode mode, T dto)
        {
            uint sequenceToSend = 0;
            QByteArray transmitKey = channel.getTransmitKey(mode, sequenceToSend);
            return serialize(channel.getChannelTag(), mode, transmitKey, sequenceToSend++, dto);
        }

        //! Deserializer
        struct Deserializer
        {
            //! Ctor
            Deserializer(CCryptoDtoChannel &channel, const QByteArray &bytes, bool loopback);

            //! Get DTO
            template <typename T>
            T getDto()
            {
                if (!m_verified) return {};
                if (m_dtoNameBuffer == T::getDtoName() || m_dtoNameBuffer == T::getShortDtoName())
                {
                    msgpack::object_handle oh2 = msgpack::unpack(m_dataBuffer.data(), static_cast<std::size_t>(m_dataBuffer.size()));
                    msgpack::object obj = oh2.get();
                    T dto = obj.as<T>();
                    return dto;
                }
                return {};
            }

            //! @{
            //! Header data
            quint16 m_headerLength;
            CryptoDtoHeaderDto m_header;
            //! @}

            //! @{
            //! Name data
            quint16 m_dtoNameLength;
            QByteArray m_dtoNameBuffer;
            //! @}

            //! @{
            //! Data
            quint16 m_dataLength;
            QByteArray m_dataBuffer;
            //! @}

            bool m_verified = false; //!< is verified
        };

        //! Deserialize
        static Deserializer deserialize(CCryptoDtoChannel &channel, const QByteArray &bytes, bool loopback);
    };
} // ns

#endif // guard
