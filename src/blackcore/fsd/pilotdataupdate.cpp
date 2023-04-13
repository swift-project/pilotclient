/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/pilotdataupdate.h"
#include "blackcore/fsd/pbh.h"
#include "blackcore/fsd/serializer.h"

#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackCore::Fsd
{
    PilotDataUpdate::PilotDataUpdate() : MessageBase()
    {}

    PilotDataUpdate::PilotDataUpdate(CTransponder::TransponderMode transponderMode, const QString &sender, int transponderCode, PilotRating rating,
                                     double latitude, double longitude, int altitudeTrue, int altitudePressure, int groundSpeed,
                                     double pitch, double bank, double heading, bool onGround)
        : MessageBase(sender, {}),
          m_transponderMode(transponderMode),
          m_transponderCode(transponderCode),
          m_rating(rating),
          m_latitude(latitude),
          m_longitude(longitude),
          m_altitudeTrue(altitudeTrue),
          m_altitudePressure(altitudePressure),
          m_groundSpeed(groundSpeed),
          m_pitch(pitch),
          m_bank(bank),
          m_heading(heading),
          m_onGround(onGround)
    {}

    QStringList PilotDataUpdate::toTokens() const
    {
        std::uint32_t pbh;
        packPBH(m_pitch, m_bank, m_heading, m_onGround, pbh);

        QStringList tokens;
        tokens.push_back(toQString(m_transponderMode));
        tokens.push_back(m_sender);
        tokens.push_back(QString::number(m_transponderCode));
        tokens.push_back(toQString(m_rating));
        tokens.push_back(QString::number(m_latitude, 'f', 5));
        tokens.push_back(QString::number(m_longitude, 'f', 5));
        tokens.push_back(QString::number(m_altitudeTrue));
        tokens.push_back(QString::number(m_groundSpeed));
        tokens.push_back(QString::number(pbh));
        tokens.push_back(QString::number(m_altitudePressure - m_altitudeTrue));
        return tokens;
    }

    PilotDataUpdate PilotDataUpdate::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 10)
        {
            CLogMessage(static_cast<PilotDataUpdate *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        double pitch = 0.0;
        double bank = 0.0;
        double heading = 0.0;
        bool onGround = false;
        unpackPBH(tokens[8].toUInt(), pitch, bank, heading, onGround);

        return PilotDataUpdate(fromQString<CTransponder::TransponderMode>(tokens[0]), tokens[1], tokens[2].toInt(), fromQString<PilotRating>(tokens[3]),
                               tokens[4].toDouble(), tokens[5].toDouble(), tokens[6].toInt(), tokens[6].toInt() + tokens[9].toInt(), tokens[7].toInt(),
                               pitch, bank, heading, onGround);
    }
}
