/* Copyright (C) 2022
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "visualpilotdatatoggle.h"
#include "pbh.h"
#include "serializer.h"

#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackCore::Fsd
{
    VisualPilotDataToggle::VisualPilotDataToggle() : MessageBase()
    {}

    VisualPilotDataToggle::VisualPilotDataToggle(const QString &sender, const QString &client, bool active)
        : MessageBase(sender, {}),
          m_client(client),
          m_active(active)
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

        return VisualPilotDataToggle(tokens[0], tokens[1], tokens[2] == QStringLiteral("1"));
    }
}
