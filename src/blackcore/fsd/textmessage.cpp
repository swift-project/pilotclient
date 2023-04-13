/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/textmessage.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    TextMessage::TextMessage() : MessageBase()
    {}

    TextMessage::TextMessage(const QString &sender, const QString &receiver, const QString &message)
        : MessageBase(sender, receiver),
          m_message(message)
    {
        if (receiver.startsWith('@'))
        {
            m_type = RadioMessage;
            const QStringList frequencyStrings = receiver.split('&');
            if (!frequencyStrings.isEmpty())
            {
                for (QString frequencyString : frequencyStrings)
                {
                    frequencyString.remove(0, 1);
                    int frequency = frequencyString.toInt() + 100000;
                    m_frequencies.push_back(frequency);
                }
            }
        }
    }

    QStringList TextMessage::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_message);
        return tokens;
    }

    TextMessage TextMessage::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            BlackMisc::CLogMessage(static_cast<TextMessage *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        };

        QStringList messageTokens = tokens.mid(2);
        return TextMessage(tokens[0], tokens[1], messageTokens.join(":"));
    }
}
