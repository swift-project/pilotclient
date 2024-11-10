// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/fsd/servererror.h"
#include <QVector>

#include "misc/logmessage.h"

namespace BlackCore::Fsd
{
    ServerError::ServerError()
    {}

    ServerError::ServerError(const QString &sender, const QString &receiver, ServerErrorCode errorCode, const QString &causingParameter, const QString &description)
        : MessageBase(sender, receiver),
          m_errorNumber(errorCode),
          m_causingParameter(causingParameter),
          m_description(description)
    {}

    bool ServerError::isFatalError() const
    {
        static const QVector<ServerErrorCode> fatalErrors {
            ServerErrorCode::CallsignInUse,
            ServerErrorCode::InvalidCallsign,
            ServerErrorCode::AlreadyRegistered,
            ServerErrorCode::InvalidCidPassword,
            ServerErrorCode::InvalidRevision,
            ServerErrorCode::RequestedLevelTooHigh,
            ServerErrorCode::ServerFull,
            ServerErrorCode::CidSuspended,
            ServerErrorCode::RatingTooLow,
            ServerErrorCode::InvalidClient,
            ServerErrorCode::AuthTimeout,
        };

        if (fatalErrors.contains(m_errorNumber)) { return true; }
        else { return false; }
    }

    QStringList ServerError::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(QString::number(static_cast<int>(m_errorNumber)));
        tokens.push_back(m_causingParameter);
        tokens.push_back(m_description);
        return tokens;
    }

    ServerError ServerError::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 5)
        {
            swift::misc::CLogMessage(static_cast<ServerError *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }
        return ServerError(tokens[0], tokens[1], static_cast<ServerErrorCode>(tokens[2].toInt()), tokens[3], tokens[4]);
    }
}
