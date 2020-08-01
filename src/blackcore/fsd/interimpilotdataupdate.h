/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_INTERIMPILOTDATAUPDATE_H
#define BLACKCORE_FSD_INTERIMPILOTDATAUPDATE_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
    {
        //! Interim pilot data update sent to specific receivers faster than
        //! the standard broadcast update.
        class BLACKCORE_EXPORT InterimPilotDataUpdate : public MessageBase
        {
        public:
            //! Constructor
            InterimPilotDataUpdate(const QString &sender, const QString &receiver, double latitude, double longitude, int altitudeTrue,
                                   int groundSpeed, double pitch, double bank, double heading, bool onGround);

            //! Message converted to tokens
            QStringList toTokens() const;

            //! Construct from tokens
            static InterimPilotDataUpdate fromTokens(const QStringList &tokens);

            //! PDU identifier
            static QString pdu() { return "#SB"; }

            //! Properties
            //! @{
            double m_latitude  = 0.0;
            double m_longitude = 0.0;
            int m_altitudeTrue = 0.0;
            int m_groundSpeed  = 0.0;
            double m_pitch = 0.0;
            double m_bank  = 0.0;
            double m_heading = 0.0;
            bool   m_onGround = false;
            //! @}

        private:
            InterimPilotDataUpdate();
        };

        //! Equal to operator
        inline bool operator==(const InterimPilotDataUpdate &lhs, const InterimPilotDataUpdate &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    qFuzzyCompare(lhs.m_latitude,  rhs.m_latitude)  &&
                    qFuzzyCompare(lhs.m_longitude, rhs.m_longitude) &&
                    lhs.m_altitudeTrue == rhs.m_altitudeTrue &&
                    qFuzzyCompare(lhs.m_pitch, rhs.m_pitch)  &&
                    qFuzzyCompare(lhs.m_bank,  rhs.m_bank)   &&
                    qFuzzyCompare(lhs.m_heading, rhs.m_heading) &&
                    lhs.m_onGround == rhs.m_onGround;
        }

        //! Not equal to operator
        inline bool operator!=(const InterimPilotDataUpdate &lhs, const InterimPilotDataUpdate &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
