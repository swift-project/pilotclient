/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_FLIGHTPLAN_H
#define BLACKCORE_FSD_FLIGHTPLAN_H

#include "messagebase.h"
#include "enums.h"

namespace BlackCore
{
    namespace Fsd
    {
        //! FSD Message: flightplan
        class BLACKCORE_EXPORT FlightPlan : public MessageBase
        {
        public:
            //! Constructor
            FlightPlan(const QString &sender, const QString &receiver, FlightType flightType, const QString &aircraftIcaoType,
                       int trueCruisingSpeed, const QString &depAirport, int estimatedDepTime, int actualDepTime, const QString &cruiseAlt,
                       const QString &destAirport, int hoursEnroute, int minutesEnroute, int fuelAvailHours, int fuelAvailMinutes,
                       const QString &altAirport, const QString &remarks, const QString &route);

            //! Message converted to tokens
            QStringList toTokens() const;

            //! Construct from tokens
            static FlightPlan fromTokens(const QStringList &tokens);

            //! PDU identifier
            static QString pdu() { return "$FP"; }

            //! Properties @{
            FlightType m_flightType {};
            QString m_aircraftIcaoType;
            int m_trueCruisingSpeed = 0;
            QString m_depAirport;
            int m_estimatedDepTime = 0;
            int m_actualDepTime = 0;
            QString m_cruiseAlt;
            QString m_destAirport;
            int m_hoursEnroute = 0;
            int m_minutesEnroute = 0;
            int m_fuelAvailHours = 0;
            int m_fuelAvailMinutes = 0;
            QString m_altAirport;
            QString m_remarks;
            QString m_route;
            //! @

        protected:
            FlightPlan();
        };

        //! Equal to operator
        inline bool operator==(const FlightPlan &lhs, const FlightPlan &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.m_flightType == rhs.m_flightType &&
                    lhs.m_aircraftIcaoType == rhs.m_aircraftIcaoType &&
                    lhs.m_trueCruisingSpeed == rhs.m_trueCruisingSpeed &&
                    lhs.m_depAirport == rhs.m_depAirport &&
                    lhs.m_estimatedDepTime == rhs.m_estimatedDepTime &&
                    lhs.m_actualDepTime == rhs.m_actualDepTime &&
                    lhs.m_cruiseAlt == rhs.m_cruiseAlt &&
                    lhs.m_destAirport == rhs.m_destAirport &&
                    lhs.m_hoursEnroute == rhs.m_hoursEnroute &&
                    lhs.m_minutesEnroute == rhs.m_minutesEnroute &&
                    lhs.m_fuelAvailHours == rhs.m_fuelAvailHours &&
                    lhs.m_fuelAvailMinutes == rhs.m_fuelAvailMinutes &&
                    lhs.m_altAirport == rhs.m_altAirport &&
                    lhs.m_remarks == rhs.m_remarks &&
                    lhs.m_route == rhs.m_route;
        }

        //! Not equal to operator
        inline bool operator!=(const FlightPlan &lhs, const FlightPlan &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
