// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_VISUALPILOTDATASTOPPED_H
#define SWIFT_CORE_FSD_VISUALPILOTDATASTOPPED_H

#include "enums.h"
#include "messagebase.h"

namespace swift::core::fsd
{
    class VisualPilotDataUpdate;

    //! VisualPilotDataUpdate with velocity assumed to be zero.
    class SWIFT_CORE_EXPORT VisualPilotDataStopped : public MessageBase
    {
    public:
        //! Constructor
        VisualPilotDataStopped(const QString &sender, double latitude, double longitude, double altitudeTrue, double heightAgl,
                               double pitch, double bank, double heading, double noseGearAngle = 0.0);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static VisualPilotDataStopped fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#ST"; }

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
        double m_noseGearAngle = 0.0;
        //! @}

    private:
        VisualPilotDataStopped();
    };

    //! Equal to operator
    inline bool operator==(const VisualPilotDataStopped &lhs, const VisualPilotDataStopped &rhs)
    {
        return qFuzzyCompare(lhs.m_latitude, rhs.m_latitude) &&
               qFuzzyCompare(lhs.m_longitude, rhs.m_longitude) &&
               qFuzzyCompare(lhs.m_altitudeTrue, rhs.m_altitudeTrue) &&
               qFuzzyCompare(lhs.m_heightAgl, rhs.m_heightAgl) &&
               qFuzzyCompare(lhs.m_pitch, rhs.m_pitch) &&
               qFuzzyCompare(lhs.m_bank, rhs.m_bank) &&
               qFuzzyCompare(lhs.m_heading, rhs.m_heading) &&
               qFuzzyCompare(lhs.m_noseGearAngle, rhs.m_noseGearAngle);
    }

    //! Not equal to operator
    inline bool operator!=(const VisualPilotDataStopped &lhs, const VisualPilotDataStopped &rhs)
    {
        return !(lhs == rhs);
    }
} // namespace swift::core::fsd

#endif // guard
