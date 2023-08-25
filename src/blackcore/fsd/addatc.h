// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_ADDATC_H
#define BLACKCORE_FSD_ADDATC_H

#include "blackcore/fsd/messagebase.h"
#include "blackcore/fsd/enums.h"

#include <QString>
#include <QStringList>

namespace BlackCore::Fsd
{
    //! Add ATC message
    class BLACKCORE_EXPORT AddAtc : public MessageBase
    {
    public:
        //! Constructor
        AddAtc(const QString &callsign, const QString &realName, const QString &cid,
               const QString &password, AtcRating rating, int protocolRevision);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static AddAtc fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return QStringLiteral("#AA"); }

        //! Get user cid
        QString cid() const { return m_cid; }

        //! Get user password
        QString password() const { return m_password; }

        //! Get ATC rating
        AtcRating rating() const { return m_rating; }

        //! Get protocol version
        int protocolRevision() const { return m_protocolRevision; }

        //! Get real name
        QString realName() const { return m_realName; }

    private:
        AddAtc();

        QString m_cid;
        QString m_password;
        AtcRating m_rating = AtcRating::Unknown;
        int m_protocolRevision = 0;
        QString m_realName;
    };

    //! Equal operator
    inline bool operator==(const AddAtc &lhs, const AddAtc &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.cid() == rhs.cid() &&
               lhs.password() == rhs.password() &&
               lhs.rating() == rhs.rating() &&
               lhs.protocolRevision() == rhs.protocolRevision() &&
               lhs.realName() == rhs.realName();
    }

    //! Not equal operator
    inline bool operator!=(const AddAtc &lhs, const AddAtc &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
