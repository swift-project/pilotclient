/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_PONG_H
#define BLACKCORE_FSD_PONG_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT Pong : public MessageBase
        {
        public:
            Pong(const QString &sender, const QString &receiver, const QString &timestamp);

            virtual ~Pong() {}

            QStringList toTokens() const;
            static Pong fromTokens(const QStringList &tokens);
            static QString pdu() { return "$PO"; }

            QString m_timestamp;

        private:
            Pong();
        };

        inline bool operator==(const Pong &lhs, const Pong &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.m_timestamp == rhs.m_timestamp;
        }

        inline bool operator!=(const Pong &lhs, const Pong &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
