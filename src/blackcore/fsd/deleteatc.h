/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_DELETEATC_H
#define BLACKCORE_FSD_DELETEATC_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT DeleteAtc : public MessageBase
        {
        public:
            DeleteAtc(const QString &sender, const QString &cid);
            virtual ~DeleteAtc() {}

            QStringList toTokens() const;
            static DeleteAtc fromTokens(const QStringList &tokens);
            static QString pdu() { return "#DA"; }

            QString m_cid;

        private:
            DeleteAtc();
        };

        inline bool operator==(const DeleteAtc &lhs, const DeleteAtc &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.m_cid == rhs.m_cid;
        }

        inline bool operator!=(const DeleteAtc &lhs, const DeleteAtc &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
