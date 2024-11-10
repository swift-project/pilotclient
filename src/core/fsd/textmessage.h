// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_TEXTMESSAGE_H
#define SWIFT_CORE_FSD_TEXTMESSAGE_H

#include "core/fsd/messagebase.h"

#include <QString>
#include <QStringList>
#include <QVector>

namespace swift::core::fsd
{
    //! Text, radio or private message
    class SWIFT_CORE_EXPORT TextMessage : public MessageBase
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
