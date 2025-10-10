// SPDX-FileCopyrightText: Copyright (C) 2022 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "visualpilotdatatoggle.h"

#include "pbh.h"
#include "serializer.h"

#include "misc/logmessage.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::core::fsd
{
    VisualPilotDataToggle::VisualPilotDataToggle() : MessageBase() {}

    VisualPilotDataToggle::VisualPilotDataToggle(const QString &sender, const QString &client, bool active)
        : MessageBase(sender, {}), m_client(client), m_active(active)
    {}

    QStringList VisualPilotDataToggle::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_client);
        tokens.push_back(QString::number(m_active ? 1 : 0));
        return tokens;
    }

    VisualPilotDataToggle VisualPilotDataToggle::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            CLogMessage(static_cast<VisualPilotDataToggle *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        return { tokens[0], tokens[1], tokens[2] == QStringLiteral("1") };
    }
} // namespace swift::core::fsd
