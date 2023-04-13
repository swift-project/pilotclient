/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/flightplan.h"
#include "blackcore/fsd/serializer.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    FlightPlan::FlightPlan() {}

    FlightPlan::FlightPlan(const QString &sender, const QString &receiver, FlightType flightType, const QString &aircraftIcaoType,
                           int trueCruisingSpeed, const QString &depAirport, int estimatedDepTime, int actualDepTime, const QString &cruiseAlt,
                           const QString &destAirport, int hoursEnroute, int minutesEnroute, int fuelAvailHours, int fuelAvailMinutes,
                           const QString &altAirport, const QString &remarks, const QString &route)
        : MessageBase(sender, receiver),
          m_flightType(flightType),
          m_aircraftIcaoType(aircraftIcaoType),
          m_trueCruisingSpeed(trueCruisingSpeed),
          m_depAirport(depAirport),
          m_estimatedDepTime(estimatedDepTime),
          m_actualDepTime(actualDepTime),
          m_cruiseAlt(cruiseAlt),
          m_destAirport(destAirport),
          m_hoursEnroute(hoursEnroute),
          m_minutesEnroute(minutesEnroute),
          m_fuelAvailHours(fuelAvailHours),
          m_fuelAvailMinutes(fuelAvailMinutes),
          m_altAirport(altAirport),
          m_remarks(remarks),
          m_route(route)
    {}

    QStringList FlightPlan::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(toQString(m_flightType));
        tokens.push_back(m_aircraftIcaoType);
        tokens.push_back(QString::number(m_trueCruisingSpeed));
        tokens.push_back(m_depAirport);
        tokens.push_back(QString::number(m_estimatedDepTime));
        tokens.push_back(QString::number(m_actualDepTime));
        tokens.push_back(m_cruiseAlt);
        tokens.push_back(m_destAirport);
        tokens.push_back(QString::number(m_hoursEnroute));
        tokens.push_back(QString::number(m_minutesEnroute));
        tokens.push_back(QString::number(m_fuelAvailHours));
        tokens.push_back(QString::number(m_fuelAvailMinutes));
        tokens.push_back(m_altAirport);
        tokens.push_back(m_remarks);
        tokens.push_back(m_route);

        Q_ASSERT(tokens.size() == 17);
        return tokens;
    }

    FlightPlan FlightPlan::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 17)
        {
            BlackMisc::CLogMessage(static_cast<FlightPlan *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return FlightPlan(tokens[0], tokens[1], fromQString<FlightType>(tokens[2]), tokens[3], tokens[4].toInt(), tokens[5],
                          tokens[6].toInt(), tokens[7].toInt(), tokens[8], tokens[9], tokens[10].toInt(), tokens[11].toInt(), tokens[12].toInt(),
                          tokens[13].toInt(), tokens[14], tokens[15], tokens[16]);
    }
}
