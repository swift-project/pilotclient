/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_PLANEINFOREQUEST_H
#define BLACKCORE_FSD_PLANEINFOREQUEST_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT PlaneInfoRequest : public MessageBase
        {
        public:
            PlaneInfoRequest(const QString &sender, const QString &receiver);

            virtual ~PlaneInfoRequest() {}

            QStringList toTokens() const;
            static PlaneInfoRequest fromTokens(const QStringList &tokens);
            static QString pdu() { return QStringLiteral("#SB"); }

        private:
            PlaneInfoRequest();
        };

        inline bool operator==(const PlaneInfoRequest &lhs, const PlaneInfoRequest &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver();
        }

        inline bool operator!=(const PlaneInfoRequest &lhs, const PlaneInfoRequest &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
