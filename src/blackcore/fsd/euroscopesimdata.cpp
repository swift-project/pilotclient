// SPDX-FileCopyrightText: Copyright (C) 2021 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackcore/fsd/euroscopesimdata.h"
#include "misc/logmessage.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace BlackCore::Fsd
{
    EuroscopeSimData::EuroscopeSimData() = default;

    EuroscopeSimData::EuroscopeSimData(const QString &sender, const QString &model, const QString &livery, quint64 timestamp,
                                       double latitude, double longitude, double altitude, double heading, int bank, int pitch,
                                       int groundSpeed, bool onGround, double gearPercent, double thrustPercent, const swift::misc::aviation::CAircraftLights &lights) : MessageBase(sender, {}),
                                                                                                                                                                       m_model(model),
                                                                                                                                                                       m_livery(livery),
                                                                                                                                                                       m_timestamp(timestamp),
                                                                                                                                                                       m_latitude(latitude),
                                                                                                                                                                       m_longitude(longitude),
                                                                                                                                                                       m_altitude(altitude),
                                                                                                                                                                       m_heading(heading),
                                                                                                                                                                       m_bank(bank),
                                                                                                                                                                       m_pitch(pitch),
                                                                                                                                                                       m_groundSpeed(groundSpeed),
                                                                                                                                                                       m_onGround(onGround),
                                                                                                                                                                       m_gearPercent(gearPercent),
                                                                                                                                                                       m_thrustPercent(thrustPercent),
                                                                                                                                                                       m_lights(lights)
    {}

    QStringList EuroscopeSimData::toTokens() const
    {
        static constexpr auto toFlags = [](const CAircraftLights &lights) {
            int n = 0;
            if (lights.isBeaconOn()) { n |= 0x002; }
            if (lights.isCabinOn()) { n |= 0x400; }
            if (lights.isLandingOn()) { n |= 0x020; }
            if (lights.isLogoOn()) { n |= 0x200; }
            if (lights.isNavOn()) { n |= 0x008; }
            if (lights.isRecognitionOn()) { n |= 0x080; }
            if (lights.isStrobeOn()) { n |= 0x004; }
            if (lights.isTaxiOn()) { n |= 0x040; }
            return n;
        };

        QStringList tokens;
        tokens.push_back({}); // first token is empty
        tokens.push_back(m_sender);
        tokens.push_back(m_model);
        tokens.push_back(m_livery);
        tokens.push_back(QString::number(m_timestamp));
        tokens.push_back(QString::number(m_latitude, 'f', 7));
        tokens.push_back(QString::number(m_longitude, 'f', 7));
        tokens.push_back(QString::number(m_altitude, 'f', 1));
        tokens.push_back(QString::number(m_heading, 'f', 2));
        tokens.push_back(QString::number(m_bank));
        tokens.push_back(QString::number(m_pitch));
        tokens.push_back(QString::number(m_groundSpeed));
        tokens.push_back(m_onGround ? QStringLiteral("1") : QStringLiteral("0"));
        tokens.push_back(QString::number(m_gearPercent));
        tokens.push_back(QString::number(m_thrustPercent));
        tokens.push_back(QStringLiteral("0")); // emergency flags
        tokens.push_back(QStringLiteral("0.0")); // airport altitude
        tokens.push_back(QString::number(toFlags(m_lights)));
        return tokens;
    }

    EuroscopeSimData EuroscopeSimData::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 18)
        {
            CLogMessage(static_cast<EuroscopeSimData *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        static constexpr auto fromFlags = [](int n) {
            return CAircraftLights(n & 0x4, n & 0x20, n & 0x40, n & 0x2, n & 0x8, n & 0x200, n & 0x80, n & 0x400);
        };

        // token[0,15,16] are not used
        return EuroscopeSimData(tokens[1], tokens[2], tokens[3], tokens[4].toULongLong(), tokens[5].toDouble(),
                                tokens[6].toDouble(), tokens[7].toDouble(), tokens[8].toDouble(), tokens[9].toInt(), tokens[10].toInt(),
                                tokens[11].toInt(), tokens[12].toInt(), tokens[13].toDouble(), tokens[14].toDouble(), fromFlags(tokens[17].toInt()));
    }
}
