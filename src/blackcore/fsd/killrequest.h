/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_KILLREQUEST_H
#define BLACKCORE_FSD_KILLREQUEST_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT KillRequest : public MessageBase
        {
        public:
            KillRequest(const QString &sender, const QString &receiver, const QString &reason);

            virtual ~KillRequest() {}

            QStringList toTokens() const;
            static KillRequest fromTokens(const QStringList &tokens);
            static QString pdu() { return "$!!"; }

            QString m_reason;

        private:
            KillRequest();
        };

        inline bool operator==(const KillRequest &lhs, const KillRequest &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.m_reason == rhs.m_reason;
        }

        inline bool operator!=(const KillRequest &lhs, const KillRequest &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
