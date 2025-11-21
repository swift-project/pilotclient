// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/planeinformation.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    PlaneInformation::PlaneInformation(const QString &sender, const QString &receiver, const QString &aircraft,
                                       const QString &airline = QString(), const QString &livery = QString())
        : MessageBase(sender, receiver), m_aircraft(aircraft), m_airline(airline), m_livery(livery)
    {}

    QStringList PlaneInformation::toTokens() const
    {
        QStringList pairs;
        if (!m_aircraft.isEmpty()) { pairs << QString("EQUIPMENT=" + m_aircraft); }
        if (!m_airline.isEmpty()) { pairs << QString("AIRLINE=" + m_airline); }
        if (!m_livery.isEmpty()) { pairs << QString("LIVERY=" + m_livery); }

        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back("PI");
        tokens.push_back("GEN");
        tokens.push_back(pairs.join(":"));
        return tokens;
    }

    PlaneInformation PlaneInformation::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 5)
        {
            swift::misc::CLogMessage(static_cast<PlaneInformation *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        QString aircraft;
        QString airline;
        QString livery;

        for (int i = 4; i < tokens.size(); ++i)
        {
            const QStringList pair = tokens.at(i).split("=");
            if (pair.size() == 2)
            {
                if (pair[0] == QLatin1String("EQUIPMENT")) { aircraft = pair[1]; }
                else if (pair[0] == QLatin1String("AIRLINE")) { airline = pair[1]; }
                else if (pair[0] == QLatin1String("LIVERY")) { livery = pair[1]; }
            }
        }
        return { tokens[0], tokens[1], aircraft, airline, livery };
    }
} // namespace swift::core::fsd
