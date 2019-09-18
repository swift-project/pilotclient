/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "cryptodtoserializer.h"


namespace BlackCore
{
    namespace Afv
    {
        namespace Crypto
        {
            CryptoDtoSerializer::CryptoDtoSerializer() { }

            CryptoDtoSerializer::Deserializer CryptoDtoSerializer::deserialize(CryptoDtoChannel &channel, const QByteArray &bytes, bool loopback)
            {
                return Deserializer(channel, bytes, loopback);
            }

            CryptoDtoSerializer::Deserializer::Deserializer(CryptoDtoChannel &channel, const QByteArray &bytes, bool loopback)
            {
                QByteArray data(bytes);
                QBuffer buffer(&data);
                buffer.open(QIODevice::ReadOnly);

                buffer.read((char *)&headerLength, sizeof(headerLength));

                QByteArray headerBuffer = buffer.read(headerLength);

                msgpack::object_handle oh = msgpack::unpack(headerBuffer.data(), headerBuffer.size());
                header = oh.get().as<CryptoDtoHeaderDto>();

                if (header.Mode == CryptoDtoMode::AEAD_ChaCha20Poly1305)
                {
                    int aeLength = buffer.size() - (2 + headerLength);
                    const QByteArray aePayloadBuffer = buffer.read(aeLength);

                    const QByteArray adBuffer = data.left(2 + headerLength);

                    QByteArray nonce;
                    nonce.fill(0, crypto_aead_chacha20poly1305_IETF_NPUBBYTES);
                    QBuffer nonceBuffer(&nonce);
                    nonceBuffer.open(QIODevice::WriteOnly);
                    uint32_t id = 0;
                    nonceBuffer.write(reinterpret_cast<const char *>(&id), sizeof(id));
                    nonceBuffer.write(reinterpret_cast<const char *>(&header.Sequence), sizeof(header.Sequence));
                    nonceBuffer.close();

                    QByteArray decryptedPayload;
                    unsigned long long mlen = 500;
                    decryptedPayload.fill(0, mlen);

                    QByteArray key;
                    if (loopback) { key = channel.getTransmitKey(CryptoDtoMode::AEAD_ChaCha20Poly1305); }
                    else { key = channel.getReceiveKey(CryptoDtoMode::AEAD_ChaCha20Poly1305); }
                    int result = crypto_aead_chacha20poly1305_ietf_decrypt(reinterpret_cast<unsigned char *>(decryptedPayload.data()), &mlen, nullptr,
                                 reinterpret_cast<const unsigned char *>(aePayloadBuffer.constData()), aePayloadBuffer.size(),
                                 reinterpret_cast<const unsigned char *>(adBuffer.constData()), adBuffer.size(),
                                 reinterpret_cast<const unsigned char *>(nonce.constData()),
                                 reinterpret_cast<const unsigned char *>(key.constData()));

                    if (result == 0)
                    {
                        decryptedPayload.resize(mlen);

                        // Fix this:
                        // if (! channel.checkReceivedSequence(header.Sequence)) { }

                        QBuffer decryptedPayloadBuffer(&decryptedPayload);
                        decryptedPayloadBuffer.open(QIODevice::ReadOnly);
                        decryptedPayloadBuffer.read((char *)&dtoNameLength, sizeof(dtoNameLength));
                        dtoNameBuffer = decryptedPayloadBuffer.read(dtoNameLength);

                        decryptedPayloadBuffer.read((char *)&dataLength, sizeof(dataLength));
                        dataBuffer = decryptedPayloadBuffer.read(dataLength);
                        verified = true;
                    }
                }
            }
        } // ns
    } // ns
} // ns
