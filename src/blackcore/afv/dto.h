/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_DTO_H
#define BLACKCORE_AFV_DTO_H

#include "msgpack.hpp"

#include <QByteArray>
#include <QJsonObject>
#include <QString>

namespace BlackCore
{
    namespace Afv
    {
        struct IMsgPack
        { };

        struct CryptoDtoChannelConfigDto
        {
            QString channelTag;
            QByteArray aeadReceiveKey;
            QByteArray aeadTransmitKey;
            QByteArray hmacKey;

            QJsonObject toJson() const
            {
                QJsonObject json;
                json["channelTag"] = channelTag;
                json["aeadReceiveKey"] = QString(aeadReceiveKey);
                json["aeadTransmitKey"] = QString(aeadTransmitKey);
                json["hmacKey"] = QString(hmacKey);
                return json;
            }

            static CryptoDtoChannelConfigDto fromJson(const QJsonObject &json)
            {
                CryptoDtoChannelConfigDto dto;

                dto.channelTag = json.value("channelTag").toString();
                dto.aeadReceiveKey = QByteArray::fromBase64(json.value("aeadReceiveKey").toString().toLocal8Bit());
                dto.aeadTransmitKey = QByteArray::fromBase64(json.value("aeadTransmitKey").toString().toLocal8Bit());
                dto.hmacKey = QByteArray::fromBase64(json.value("hmacKey").toString().toLocal8Bit());
                return dto;
            }
        };

        struct VoiceServerConnectionDataDto
        {
            QString addressIpV4;    // Example: 123.123.123.123:50000
            QString addressIpV6;    // Example: 123.123.123.123:50000
            CryptoDtoChannelConfigDto channelConfig;

            QJsonObject toJson() const
            {
                QJsonObject json;
                json["addressIpV4"] = addressIpV4;
                json["addressIpV6"] = addressIpV6;
                json["channelConfig"] = channelConfig.toJson();
                return json;
            }

            static VoiceServerConnectionDataDto fromJson(const QJsonObject &json)
            {
                VoiceServerConnectionDataDto dto;
                dto.addressIpV4 = json.value("addressIpV4").toString();
                dto.addressIpV6 = json.value("addressIpV6").toString();
                dto.channelConfig = CryptoDtoChannelConfigDto::fromJson(json.value("channelConfig").toObject());
                return dto;
            }
        };

        struct PostCallsignResponseDto
        {
            VoiceServerConnectionDataDto VoiceServer;
            // DataServerConnectionDataDto DataServer;
            bool isValid = false;

            QJsonObject toJson() const
            {
                QJsonObject json;
                json["voiceserverauthdatadto"] = VoiceServer.toJson();
                // json["dataserverauthdatadto"] = DataServer.toJson();
                return json;
            }

            static PostCallsignResponseDto fromJson(const QJsonObject &json)
            {
                PostCallsignResponseDto dto;
                dto.VoiceServer = VoiceServerConnectionDataDto::fromJson(json.value("voiceServer").toObject());
                // dto.DataServer = DataServerConnectionDataDto::fromJson(json.value("dataServer").toObject());
                dto.isValid = true;
                return dto;
            }
        };

        struct TransceiverDto
        {
            quint16 id;
            quint32 frequency;
            double LatDeg = 0.0;
            double LonDeg = 0.0;
            double HeightMslM = 0.0;
            double HeightAglM = 0.0;
            MSGPACK_DEFINE(id, frequency, LatDeg, LonDeg, HeightMslM, HeightAglM)

            QJsonObject toJson() const
            {
                QJsonObject json;
                json["ID"] = id;
                json["Frequency"] = static_cast<int>(frequency);
                json["LatDeg"] = LatDeg;
                json["LonDeg"] = LonDeg;
                json["AltMslM"] = HeightMslM;
                return json;
            }

            static TransceiverDto fromJson(const QJsonObject &json)
            {
                TransceiverDto dto;
                dto.id = json.value("id").toInt();
                dto.frequency = json.value("frequency").toInt();
                dto.LatDeg = json.value("latDeg").toDouble();
                dto.LonDeg = json.value("lonDeg").toDouble();
                dto.HeightMslM = json.value("heightMslM").toDouble();
                dto.HeightAglM = json.value("heightAglM").toDouble();
                return dto;
            }
        };

        struct HeartbeatDto
        {
            static QByteArray getDtoName() { return "HeartbeatDto"; }
            static QByteArray getShortDtoName() { return "H"; }

            std::string callsign;
            MSGPACK_DEFINE(callsign)
        };

        struct HeartbeatAckDto
        {
            static QByteArray getDtoName() { return "HeartbeatAckDto"; }
            static QByteArray getShortDtoName() { return "HA"; }
            MSGPACK_DEFINE()
        };

        struct RxTransceiverDto
        {
            uint16_t id;
            uint32_t frequency;
            float distanceRatio;
            // std::string RelayCallsign;

            MSGPACK_DEFINE(id, frequency, distanceRatio/*, RelayCallsign*/)
        };

        struct TxTransceiverDto
        {
            uint16_t id;

            MSGPACK_DEFINE(id)
        };

        struct AudioTxOnTransceiversDto
        {
            static QByteArray getDtoName() { return "AudioTxOnTransceiversDto"; }
            static QByteArray getShortDtoName() { return "AT"; }

            std::string callsign;
            uint sequenceCounter;
            std::vector<char> audio;
            bool lastPacket;
            std::vector<TxTransceiverDto> transceivers;
            MSGPACK_DEFINE(callsign, sequenceCounter, audio, lastPacket, transceivers)
        };

        struct AudioRxOnTransceiversDto
        {
            static QByteArray getDtoName() { return "AudioRxOnTransceiversDto"; }
            static QByteArray getShortDtoName() { return "AR"; }

            std::string callsign;
            uint sequenceCounter;
            std::vector<char> audio;
            bool lastPacket;
            std::vector<RxTransceiverDto> transceivers;
            MSGPACK_DEFINE(callsign, sequenceCounter, audio, lastPacket, transceivers)
        };

        struct IAudioDto
        {
            QString callsign;           // Callsign that audio originates from
            uint sequenceCounter;      // Receiver optionally uses this in reordering algorithm/gap detection
            QByteArray audio;              // Opus compressed audio
            bool lastPacket;           // Used to indicate to receiver that the sender has stopped sending
        };
    } // ns
} // ns

#endif // guard
