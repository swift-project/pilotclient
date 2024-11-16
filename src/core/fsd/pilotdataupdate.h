// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_PILOTDATAUPDATE_H
#define SWIFT_CORE_FSD_PILOTDATAUPDATE_H

#include "core/fsd/enums.h"
#include "core/fsd/messagebase.h"
#include "misc/aviation/transponder.h"

namespace swift::core::fsd
{
    //! Pilot data update broadcasted to all clients in range every 5 seconds.
    class SWIFT_CORE_EXPORT PilotDataUpdate : public MessageBase
    {
    public:
        //! Constructor
        PilotDataUpdate(swift::misc::aviation::CTransponder::TransponderMode transponderMode, const QString &sender, int transponderCode, PilotRating rating,
                        double latitude, double longitude, int altitudeTrue, int altitudePressure, int groundSpeed,
                        double pitch, double bank, double heading, bool onGround);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static PilotDataUpdate fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "@"; }

        //! @{
        //! Properties
        swift::misc::aviation::CTransponder::TransponderMode m_transponderMode = swift::misc::aviation::CTransponder::StateStandby;
        int m_transponderCode = 0;
        PilotRating m_rating = PilotRating::Unknown;
        double m_latitude = 0.0;
        double m_longitude = 0.0;
        int m_altitudeTrue = 0.0;
        int m_altitudePressure = 0.0;
        int m_groundSpeed = 0;
        double m_pitch = 0.0;
        double m_bank = 0.0;
        double m_heading = 0.0;
        bool m_onGround = false;
        //! @}

    private:
        PilotDataUpdate();
    };

    //! Equal to operator
    inline bool operator==(const PilotDataUpdate &lhs, const PilotDataUpdate &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_transponderMode == rhs.m_transponderMode &&
               lhs.m_transponderCode == rhs.m_transponderCode &&
               lhs.m_rating == rhs.m_rating &&
               qFuzzyCompare(lhs.m_latitude, rhs.m_latitude) &&
               qFuzzyCompare(lhs.m_longitude, rhs.m_longitude) &&
               lhs.m_altitudeTrue == rhs.m_altitudeTrue &&
               lhs.m_altitudePressure == rhs.m_altitudePressure &&
               lhs.m_groundSpeed == rhs.m_groundSpeed &&
               qFuzzyCompare(lhs.m_pitch, rhs.m_pitch) &&
               qFuzzyCompare(lhs.m_bank, rhs.m_bank) &&
               qFuzzyCompare(lhs.m_heading, rhs.m_heading) &&
               lhs.m_onGround == rhs.m_onGround;
    }

    //! Not equal to operator
    inline bool operator!=(const PilotDataUpdate &lhs, const PilotDataUpdate &rhs)
    {
        return !(lhs == rhs);
    }
} // namespace swift::core::fsd

#endif // guard
