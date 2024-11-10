// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "visualpilotdataupdate.h"
#include "visualpilotdataperiodic.h"
#include "visualpilotdatastopped.h"
#include "pbh.h"
#include "serializer.h"

#include "misc/logmessage.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace BlackCore::Fsd
{
    VisualPilotDataUpdate::VisualPilotDataUpdate() : MessageBase()
    {}

    VisualPilotDataUpdate::VisualPilotDataUpdate(const QString &sender, double latitude, double longitude, double altitudeTrue, double heightAgl,
                                                 double pitch, double bank, double heading, double xVelocity, double yVelocity, double zVelocity,
                                                 double pitchRadPerSec, double bankRadPerSec, double headingRadPerSec, double noseGearAngle)
        : MessageBase(sender, {}),
          m_latitude(latitude),
          m_longitude(longitude),
          m_altitudeTrue(altitudeTrue),
          m_heightAgl(heightAgl),
          m_pitch(pitch),
          m_bank(bank),
          m_heading(heading),
          m_xVelocity(xVelocity),
          m_yVelocity(yVelocity),
          m_zVelocity(zVelocity),
          m_pitchRadPerSec(pitchRadPerSec),
          m_bankRadPerSec(bankRadPerSec),
          m_headingRadPerSec(headingRadPerSec),
          m_noseGearAngle(noseGearAngle)
    {}

    QStringList VisualPilotDataUpdate::toTokens() const
    {
        std::uint32_t pbh;
        packPBH(m_pitch, m_bank, m_heading, false /*! \todo check if needed? */, pbh);

        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(QString::number(m_latitude, 'f', 7));
        tokens.push_back(QString::number(m_longitude, 'f', 7));
        tokens.push_back(QString::number(m_altitudeTrue, 'f', 2));
        tokens.push_back(QString::number(m_heightAgl, 'f', 2));
        tokens.push_back(QString::number(pbh));
        tokens.push_back(QString::number(m_xVelocity, 'f', 4));
        tokens.push_back(QString::number(m_yVelocity, 'f', 4));
        tokens.push_back(QString::number(m_zVelocity, 'f', 4));
        tokens.push_back(QString::number(m_pitchRadPerSec, 'f', 4));
        tokens.push_back(QString::number(m_headingRadPerSec, 'f', 4));
        tokens.push_back(QString::number(m_bankRadPerSec, 'f', 4));
        tokens.push_back(QString::number(m_noseGearAngle, 'f', 2));
        return tokens;
    }

    VisualPilotDataUpdate VisualPilotDataUpdate::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 12)
        {
            CLogMessage(static_cast<VisualPilotDataUpdate *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        double pitch = 0.0;
        double bank = 0.0;
        double heading = 0.0;
        bool unused = false; //! \todo check if needed?
        unpackPBH(tokens[5].toUInt(), pitch, bank, heading, unused);

        return VisualPilotDataUpdate(tokens[0], tokens[1].toDouble(), tokens[2].toDouble(), tokens[3].toDouble(), tokens[4].toDouble(),
                                     pitch, bank, heading, tokens[6].toDouble(), tokens[7].toDouble(), tokens[8].toDouble(), tokens[9].toDouble(),
                                     tokens[11].toDouble(), tokens[10].toDouble(), tokens.value(12, QStringLiteral("0")).toDouble());
    }

    VisualPilotDataPeriodic VisualPilotDataUpdate::toPeriodic() const
    {
        return VisualPilotDataPeriodic(m_sender, m_latitude, m_longitude, m_altitudeTrue, m_heightAgl, m_pitch, m_bank, m_heading,
                                       m_xVelocity, m_yVelocity, m_zVelocity, m_pitchRadPerSec, m_bankRadPerSec, m_headingRadPerSec, m_noseGearAngle);
    }

    VisualPilotDataStopped VisualPilotDataUpdate::toStopped() const
    {
        return VisualPilotDataStopped(m_sender, m_latitude, m_longitude, m_altitudeTrue, m_heightAgl, m_pitch, m_bank, m_heading,
                                      m_noseGearAngle);
    }
}
