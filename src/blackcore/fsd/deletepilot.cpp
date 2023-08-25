// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/fsd/deletepilot.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;

namespace BlackCore::Fsd
{
    DeletePilot::DeletePilot() : MessageBase()
    {}

    DeletePilot::DeletePilot(const QString &callsign, const QString &id)
        : MessageBase(callsign),
          m_cid(id)
    {}

    QStringList DeletePilot::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_cid);
        return tokens;
    }

    DeletePilot DeletePilot::fromTokens(const QStringList &tokens)
    {
        if (tokens.isEmpty())
        {
            CLogMessage(static_cast<DeletePilot *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        // VATSIM FSD will always supply the CERTIFICATE ID when it rebroadcasts this PDU without regard for whether
        // the client originally specified it. But other FSDs might not.
        DeletePilot packet(tokens[0], (tokens.size() >= 2) ? tokens[1] : "");
        return packet;
    }
}
