/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "visualpilotdataupdate.h"
#include "pbh.h"
#include "serializer.h"

#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackCore::Fsd
{
    VisualPilotDataUpdate::VisualPilotDataUpdate() : MessageBase()
    { }

    VisualPilotDataUpdate::VisualPilotDataUpdate(const QString &sender, double latitude, double longitude, double altitudeTrue,
                                                    double pitch, double bank, double heading, double xVelocity, double yVelocity, double zVelocity,
                                                    double pitchRadPerSec, double bankRadPerSec, double headingRadPerSec)
        : MessageBase(sender, {}),
            m_latitude(latitude),
            m_longitude(longitude),
            m_altitudeTrue(altitudeTrue),
            m_pitch(pitch),
            m_bank(bank),
            m_heading(heading),
            m_xVelocity(xVelocity),
            m_yVelocity(yVelocity),
            m_zVelocity(zVelocity),
            m_pitchRadPerSec(pitchRadPerSec),
            m_bankRadPerSec(bankRadPerSec),
            m_headingRadPerSec(headingRadPerSec)
    { }

    QStringList VisualPilotDataUpdate::toTokens() const
    {
        std::uint32_t pbh;
        packPBH(m_pitch, m_bank, m_heading, false/*! \todo check if needed? */, pbh);

        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(QString::number(m_latitude,  'f', 7));
        tokens.push_back(QString::number(m_longitude, 'f', 7));
        tokens.push_back(QString::number(m_altitudeTrue,     'f', 2));
        tokens.push_back(QString::number(pbh));
        tokens.push_back(QString::number(m_xVelocity, 'f', 4));
        tokens.push_back(QString::number(m_yVelocity, 'f', 4));
        tokens.push_back(QString::number(m_zVelocity, 'f', 4));
        tokens.push_back(QString::number(m_pitchRadPerSec,   'f', 4));
        tokens.push_back(QString::number(m_headingRadPerSec, 'f', 4));
        tokens.push_back(QString::number(m_bankRadPerSec,    'f', 4));
        return tokens;
    }

    VisualPilotDataUpdate VisualPilotDataUpdate::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 11)
        {
            CLogMessage(static_cast<VisualPilotDataUpdate *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        double pitch = 0.0;
        double bank  = 0.0;
        double heading = 0.0;
        bool unused = false; //! \todo check if needed?
        unpackPBH(tokens[4].toUInt(), pitch, bank, heading, unused);

        return VisualPilotDataUpdate(tokens[0], tokens[1].toDouble(), tokens[2].toDouble(), tokens[3].toDouble(),
                pitch, bank, heading, tokens[5].toDouble(), tokens[6].toDouble(), tokens[7].toDouble(),
                tokens[8].toDouble(), tokens[10].toDouble(), tokens[9].toDouble());
    }
}
