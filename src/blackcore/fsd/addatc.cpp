// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/fsd/addatc.h"
#include "blackcore/fsd/serializer.h"
#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    AddAtc::AddAtc() : MessageBase()
    {}

    AddAtc::AddAtc(const QString &callsign, const QString &realName, const QString &cid, const QString &password,
                   AtcRating rating, int protocolRevision)
        : MessageBase(callsign, "SERVER"),
          m_cid(cid),
          m_password(password),
          m_rating(rating),
          m_protocolRevision(protocolRevision),
          m_realName(realName)
    {}

    QStringList AddAtc::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_realName);
        tokens.push_back(m_cid);
        tokens.push_back(m_password);
        tokens.push_back(toQString(m_rating));
        tokens.push_back(QString::number(m_protocolRevision));
        return tokens;
    }

    AddAtc AddAtc::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 7)
        {
            BlackMisc::CLogMessage(static_cast<AddAtc *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        }

        const AtcRating rating = fromQString<AtcRating>(tokens[5]);
        const int protocolRevision = tokens[6].toInt();
        return AddAtc(tokens[0], tokens[2], tokens[3], tokens[4], rating, protocolRevision);
    }
}
