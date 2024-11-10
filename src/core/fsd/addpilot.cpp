// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/addpilot.h"
#include "core/fsd/serializer.h"

#include "misc/logmessage.h"

using namespace swift::misc;

namespace swift::core::fsd
{
    AddPilot::AddPilot(const QString &callsign, const QString &cid, const QString &password, PilotRating rating,
                       int protocolRevision, SimType simType, const QString &realName)
        : MessageBase(callsign, "SERVER"),
          m_cid(cid), m_password(password), m_rating(rating), m_protocolRevision(protocolRevision),
          m_simType(simType), m_realName(realName)
    {}

    QStringList AddPilot::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_cid);
        tokens.push_back(m_password);
        tokens.push_back(toQString(m_rating));
        tokens.push_back(QString::number(m_protocolRevision));
        tokens.push_back(toQString(m_simType));
        tokens.push_back(m_realName);
        return tokens;
    }

    AddPilot AddPilot::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 6)
        {
            CLogMessage(static_cast<AddPilot *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        }

        const PilotRating rating = fromQString<PilotRating>(tokens[4]);
        const int protocolRevision = tokens[5].toInt();
        const SimType simType = fromQString<SimType>(tokens[6]);
        const AddPilot packet(tokens[0], tokens[2], tokens[3], rating, protocolRevision, simType, tokens[7]);
        return packet;
    }
}
