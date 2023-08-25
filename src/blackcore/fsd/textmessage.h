// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_TEXTMESSAGE_H
#define BLACKCORE_FSD_TEXTMESSAGE_H

#include "blackcore/fsd/messagebase.h"

#include <QString>
#include <QStringList>
#include <QVector>

namespace BlackCore::Fsd
{
    //! Text, radio or private message
    class BLACKCORE_EXPORT TextMessage : public MessageBase
    {
    public:
        //! Message type
        enum Type
        {
            PrivateMessage,
            RadioMessage,
        };

        //! Constructor
        TextMessage(const QString &sender, const QString &receiver, const QString &message);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static TextMessage fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#TM"; }

        QString m_message; //!< message text
        Type m_type = PrivateMessage; //!< message type
        QVector<int> m_frequencies; //!< frequencies in case of radio message.

    private:
        TextMessage();
    };
}

#endif // guard
