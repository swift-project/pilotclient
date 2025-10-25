// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_VISUALPILOTDATAPERIODIC_H
#define SWIFT_CORE_FSD_VISUALPILOTDATAPERIODIC_H

#include "enums.h"
#include "messagebase.h"

namespace swift::core::fsd
{
    class VisualPilotDataUpdate;

    //! Every 25th VisualPilotDataUpdate is actually one of these ("slowfast").
    class SWIFT_CORE_EXPORT VisualPilotDataPeriodic : public MessageBase
    {
    public:
        //! Constructor
        VisualPilotDataPeriodic(const QString &sender, double latitude, double longitude, double altitudeTrue,
                                double heightAgl, double pitch, double bank, double heading, double xVelocity,
                                double yVelocity, double zVelocity, double pitchRadPerSec, double bankRadPerSec,
                                double headingRadPerSec, double noseGearAngle = 0.0);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static VisualPilotDataPeriodic fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#SL"; }

        //! Return a regular visual update with the same values
        VisualPilotDataUpdate toUpdate() const;

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

    private:
        VisualPilotDataPeriodic() = default;
    };

    //! Equal to operator
    inline bool operator==(const VisualPilotDataPeriodic &lhs, const VisualPilotDataPeriodic &rhs)
    {
        return qFuzzyCompare(lhs.m_latitude, rhs.m_latitude) && qFuzzyCompare(lhs.m_longitude, rhs.m_longitude) &&
               qFuzzyCompare(lhs.m_altitudeTrue, rhs.m_altitudeTrue) &&
               qFuzzyCompare(lhs.m_heightAgl, rhs.m_heightAgl) && qFuzzyCompare(lhs.m_pitch, rhs.m_pitch) &&
               qFuzzyCompare(lhs.m_bank, rhs.m_bank) && qFuzzyCompare(lhs.m_heading, rhs.m_heading) &&
               qFuzzyCompare(lhs.m_xVelocity, rhs.m_xVelocity) && qFuzzyCompare(lhs.m_yVelocity, rhs.m_yVelocity) &&
               qFuzzyCompare(lhs.m_zVelocity, rhs.m_zVelocity) &&
               qFuzzyCompare(lhs.m_pitchRadPerSec, rhs.m_pitchRadPerSec) &&
               qFuzzyCompare(lhs.m_bankRadPerSec, rhs.m_bankRadPerSec) &&
               qFuzzyCompare(lhs.m_headingRadPerSec, rhs.m_headingRadPerSec) &&
               qFuzzyCompare(lhs.m_noseGearAngle, rhs.m_noseGearAngle);
    }

    //! Not equal to operator
    inline bool operator!=(const VisualPilotDataPeriodic &lhs, const VisualPilotDataPeriodic &rhs)
    {
        return !(lhs == rhs);
    }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_VISUALPILOTDATAPERIODIC_H
