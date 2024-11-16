// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "visualpilotdatastopped.h"

#include "pbh.h"
#include "serializer.h"
#include "visualpilotdataupdate.h"

#include "misc/logmessage.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::core::fsd
{
    VisualPilotDataStopped::VisualPilotDataStopped() : MessageBase()
    {}

    VisualPilotDataStopped::VisualPilotDataStopped(const QString &sender, double latitude, double longitude, double altitudeTrue, double heightAgl,
                                                   double pitch, double bank, double heading, double noseGearAngle)
        : MessageBase(sender, {}),
          m_latitude(latitude),
          m_longitude(longitude),
          m_altitudeTrue(altitudeTrue),
          m_heightAgl(heightAgl),
          m_pitch(pitch),
          m_bank(bank),
          m_heading(heading),
          m_noseGearAngle(noseGearAngle)
    {}

    QStringList VisualPilotDataStopped::toTokens() const
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
        tokens.push_back(QString::number(m_noseGearAngle, 'f', 2));
        return tokens;
    }

    VisualPilotDataStopped VisualPilotDataStopped::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 6)
        {
            CLogMessage(static_cast<VisualPilotDataStopped *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        double pitch = 0.0;
        double bank = 0.0;
        double heading = 0.0;
        bool unused = false; //! \todo check if needed?
        unpackPBH(tokens[5].toUInt(), pitch, bank, heading, unused);

        return VisualPilotDataStopped(tokens[0], tokens[1].toDouble(), tokens[2].toDouble(), tokens[3].toDouble(), tokens[4].toDouble(),
                                      pitch, bank, heading, tokens.value(12, QStringLiteral("0")).toDouble());
    }

    VisualPilotDataUpdate VisualPilotDataStopped::toUpdate() const
    {
        return VisualPilotDataUpdate(m_sender, m_latitude, m_longitude, m_altitudeTrue, m_heightAgl, m_pitch, m_bank, m_heading,
                                     0, 0, 0, 0, 0, 0, m_noseGearAngle);
    }
} // namespace swift::core::fsd
