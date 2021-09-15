/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "interimpilotdataupdate.h"
#include "pbh.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    InterimPilotDataUpdate::InterimPilotDataUpdate() : MessageBase()
    { }

    InterimPilotDataUpdate::InterimPilotDataUpdate(const QString &sender, const QString &receiver, double latitude, double longitude, int altitudeTrue,
                                                    int groundSpeed, double pitch, double bank, double heading, bool onGround)
        : MessageBase(sender, receiver),
            m_latitude(latitude),
            m_longitude(longitude),
            m_altitudeTrue(altitudeTrue),
            m_groundSpeed(groundSpeed),
            m_pitch(pitch),
            m_bank(bank),
            m_heading(heading),
            m_onGround(onGround)
    { }

    QStringList InterimPilotDataUpdate::toTokens() const
    {
        std::uint32_t pbh;
        packPBH(m_pitch, m_bank, m_heading, m_onGround, pbh);

        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);

        // SquawkBox used "I", but since we are using a different message format,
        // we also need to use a different subtype.
        // VI = vatlib interim
        tokens.push_back("VI");
        tokens.push_back(QString::number(m_latitude, 'f', 5));
        tokens.push_back(QString::number(m_longitude, 'f', 5));
        tokens.push_back(QString::number(m_altitudeTrue));
        tokens.push_back(QString::number(m_groundSpeed));
        tokens.push_back(QString::number(pbh));
        return tokens;
    }

    InterimPilotDataUpdate InterimPilotDataUpdate::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 8)
        {
            BlackMisc::CLogMessage(static_cast<InterimPilotDataUpdate *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };

        double pitch = 0.0;
        double bank = 0.0;
        double heading = 0.0;
        bool onGround = false;
        unpackPBH(tokens[7].toUInt(), pitch, bank, heading, onGround);

        return InterimPilotDataUpdate(tokens[0], tokens[1], tokens[3].toDouble(), tokens[4].toDouble(), tokens[5].toInt(), tokens[6].toInt(),
                pitch, bank, heading, onGround);
    }
}
