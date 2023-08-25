// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_VISUALPILOTDATAUPDATE_H
#define BLACKCORE_FSD_VISUALPILOTDATAUPDATE_H

#include "messagebase.h"
#include "enums.h"

namespace BlackCore::Fsd
{
    class VisualPilotDataPeriodic;
    class VisualPilotDataStopped;

    //! Pilot data update broadcasted to pilots in range every 0.2 seconds.
    class BLACKCORE_EXPORT VisualPilotDataUpdate : public MessageBase
    {
    public:
        //! Constructor
        VisualPilotDataUpdate(const QString &sender, double latitude, double longitude, double altitudeTrue, double heightAgl,
                              double pitch, double bank, double heading, double xVelocity, double yVelocity, double zVelocity,
                              double pitchRadPerSec, double bankRadPerSec, double headingRadPerSec, double noseGearAngle = 0.0);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static VisualPilotDataUpdate fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "^"; }

        //! Return a periodic update with the same values
        VisualPilotDataPeriodic toPeriodic() const;

        //! Return a stopped update with the same values
        VisualPilotDataStopped toStopped() const;

        //! @{
        //! Properties
        double m_latitude = 0.0;
        double m_longitude = 0.0;
        double m_altitudeTrue = 0.0;
        double m_heightAgl = 0.0;
        double m_pitch = 0.0;
        double m_bank = 0.0;
        double m_heading = 0.0;
        double m_xVelocity = 0.0;
        double m_yVelocity = 0.0;
        double m_zVelocity = 0.0;
        double m_pitchRadPerSec = 0.0;
        double m_bankRadPerSec = 0.0;
        double m_headingRadPerSec = 0.0;
        double m_noseGearAngle = 0.0;
        //! @}

        // private: // not private: used in CFSDClient::handleVisualPilotDataUpdate
        VisualPilotDataUpdate();
    };

    //! Equal to operator
    inline bool operator==(const VisualPilotDataUpdate &lhs, const VisualPilotDataUpdate &rhs)
    {
        return qFuzzyCompare(lhs.m_latitude, rhs.m_latitude) &&
               qFuzzyCompare(lhs.m_longitude, rhs.m_longitude) &&
               qFuzzyCompare(lhs.m_altitudeTrue, rhs.m_altitudeTrue) &&
               qFuzzyCompare(lhs.m_heightAgl, rhs.m_heightAgl) &&
               qFuzzyCompare(lhs.m_pitch, rhs.m_pitch) &&
               qFuzzyCompare(lhs.m_bank, rhs.m_bank) &&
               qFuzzyCompare(lhs.m_heading, rhs.m_heading) &&
               qFuzzyCompare(lhs.m_xVelocity, rhs.m_xVelocity) &&
               qFuzzyCompare(lhs.m_yVelocity, rhs.m_yVelocity) &&
               qFuzzyCompare(lhs.m_zVelocity, rhs.m_zVelocity) &&
               qFuzzyCompare(lhs.m_pitchRadPerSec, rhs.m_pitchRadPerSec) &&
               qFuzzyCompare(lhs.m_bankRadPerSec, rhs.m_bankRadPerSec) &&
               qFuzzyCompare(lhs.m_headingRadPerSec, rhs.m_headingRadPerSec) &&
               qFuzzyCompare(lhs.m_noseGearAngle, rhs.m_noseGearAngle);
    }

    //! Not equal to operator
    inline bool operator!=(const VisualPilotDataUpdate &lhs, const VisualPilotDataUpdate &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
