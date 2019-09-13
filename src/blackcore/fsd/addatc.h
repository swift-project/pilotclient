/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_ADDATC_H
#define BLACKCORE_FSD_ADDATC_H

#include "messagebase.h"
#include "enums.h"

#include <QString>
#include <QStringList>

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT AddAtc : public MessageBase
        {
        public:
            AddAtc(const QString &callsign, const QString &realName, const QString &cid,
                   const QString &password, AtcRating rating, int protocolRevision);

            QStringList toTokens() const;
            static AddAtc fromTokens(const QStringList &tokens);
            static QString pdu() { return QStringLiteral("#AA"); }

            QString cid() const { return m_cid; }
            QString password() const { return m_password; }
            AtcRating rating() const { return m_rating; }
            int protocolRevision() const { return m_protocolRevision; }
            QString realName() const { return m_realName; }

        private:
            AddAtc();

            QString m_cid;
            QString m_password;
            AtcRating m_rating = AtcRating::Unknown;
            int m_protocolRevision = 0;
            QString m_realName;
        };

        inline bool operator==(const AddAtc &lhs, const AddAtc &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.cid() == rhs.cid() &&
                    lhs.password() == rhs.password() &&
                    lhs.rating() == rhs.rating() &&
                    lhs.protocolRevision() == rhs.protocolRevision() &&
                    lhs.realName() == rhs.realName();
        }

        inline bool operator!=(const AddAtc &lhs, const AddAtc &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
