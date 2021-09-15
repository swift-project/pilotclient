/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/planeinformation.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    PlaneInformation::PlaneInformation() : MessageBase()
    { }

    PlaneInformation::PlaneInformation(const QString &sender, const QString &receiver, const QString &aircraft,
                                        const QString &airline = QString(), const QString &livery = QString())
        : MessageBase(sender, receiver),
            m_aircraft(aircraft),
            m_airline(airline),
            m_livery(livery)
    { }

    QStringList PlaneInformation::toTokens() const
    {
        QStringList pairs;
        if (!m_aircraft.isEmpty()) { pairs << QString("EQUIPMENT=" + m_aircraft); }
        if (!m_airline.isEmpty())  { pairs << QString("AIRLINE="   + m_airline);  }
        if (!m_livery.isEmpty())   { pairs << QString("LIVERY="    + m_livery);   }

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
            BlackMisc::CLogMessage(static_cast<PlaneInformation *>(nullptr)).debug(u"Wrong number of arguments.");
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
                if (pair[0] == QLatin1String("EQUIPMENT"))    { aircraft = pair[1]; }
                else if (pair[0] == QLatin1String("AIRLINE")) { airline  = pair[1]; }
                else if (pair[0] == QLatin1String("LIVERY"))  { livery   = pair[1]; }
            }
        }
        return PlaneInformation(tokens[0], tokens[1], aircraft, airline, livery);
    }
}

