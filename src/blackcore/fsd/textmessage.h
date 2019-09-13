/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_TEXTMESSAGE_H
#define BLACKCORE_FSD_TEXTMESSAGE_H

#include "messagebase.h"

#include <QString>
#include <QStringList>
#include <QVector>

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT TextMessage : public MessageBase
        {
        public:
            enum Type
            {
                PrivateMessage,
                RadioMessage,
            };

            TextMessage(const QString &sender, const QString &receiver, const QString &message);
            virtual ~TextMessage() {}

            QStringList toTokens() const;
            static TextMessage fromTokens(const QStringList &tokens);
            static QString pdu() { return "#TM"; }

            QString m_message;
            Type m_type = PrivateMessage;
            QVector<int> m_frequencies;

        private:
            TextMessage();
        };
    }
}

#endif // guard
