/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_VISUALPILOTDATAPERIODIC_H
#define BLACKCORE_FSD_VISUALPILOTDATAPERIODIC_H

#include "messagebase.h"
#include "enums.h"

namespace BlackCore::Fsd
{
    class VisualPilotDataUpdate;

    //! Every 25th VisualPilotDataUpdate is actually one of these ("slowfast").
    class BLACKCORE_EXPORT VisualPilotDataPeriodic : public MessageBase
    {
    public:
        //! Constructor
        VisualPilotDataPeriodic(const QString &sender, double latitude, double longitude, double altitudeTrue, double heightAgl,
                                double pitch, double bank, double heading, double xVelocity, double yVelocity, double zVelocity,
                                double pitchRadPerSec, double bankRadPerSec, double headingRadPerSec, double noseGearAngle = 0.0);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static VisualPilotDataPeriodic fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#SL"; }

        //! Return a regular visual update with the same values
        VisualPilotDataUpdate toUpdate() const;

        //! Properties
        //! @{
        double m_latitude      = 0.0;
        double m_longitude     = 0.0;
        double m_altitudeTrue  = 0.0;
        double m_heightAgl     = 0.0;
        double m_pitch         = 0.0;
        double m_bank          = 0.0;
        double m_heading       = 0.0;
        double m_xVelocity     = 0.0;
        double m_yVelocity     = 0.0;
        double m_zVelocity     = 0.0;
        double m_pitchRadPerSec     = 0.0;
        double m_bankRadPerSec      = 0.0;
        double m_headingRadPerSec   = 0.0;
        double m_noseGearAngle      = 0.0;
        //! @}

    private:
        VisualPilotDataPeriodic();
    };

    //! Equal to operator
    inline bool operator==(const VisualPilotDataPeriodic &lhs, const VisualPilotDataPeriodic &rhs)
    {
        return  qFuzzyCompare(lhs.m_latitude, rhs.m_latitude) &&
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
    inline bool operator!=(const VisualPilotDataPeriodic &lhs, const VisualPilotDataPeriodic &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
