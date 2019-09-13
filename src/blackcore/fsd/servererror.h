/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_SERVERERROR_H
#define BLACKCORE_FSD_SERVERERROR_H

#include "messagebase.h"
#include "enums.h"

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT ServerError : public MessageBase
        {
        public:
            ServerError(const QString &sender, const QString &receiver, ServerErrorCode errorCode, const QString &causingParameter, const QString &description);

            virtual ~ServerError() {}

            bool isFatalError () const;

            QStringList toTokens() const;
            static ServerError fromTokens(const QStringList &tokens);
            static QString pdu() { return "$ER"; }

            ServerErrorCode m_errorNumber;
            QString m_causingParameter;
            QString m_description;

        private:
            ServerError();
        };

        inline bool operator==(const ServerError &lhs, const ServerError &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.m_errorNumber == rhs.m_errorNumber &&
                    lhs.m_causingParameter == rhs.m_causingParameter &&
                    lhs.m_description == rhs.m_description;
        }

        inline bool operator!=(const ServerError &lhs, const ServerError &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
