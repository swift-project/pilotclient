// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_ADDATC_H
#define SWIFT_CORE_FSD_ADDATC_H

#include <QString>
#include <QStringList>

#include "core/fsd/enums.h"
#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! Add ATC message
    class SWIFT_CORE_EXPORT AddAtc : public MessageBase
    {
    public:
        //! Constructor
        AddAtc(const QString &callsign, const QString &realName, const QString &cid, const QString &password,
               AtcRating rating, int protocolRevision);

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
        AddAtc() = default;

        QString m_cid;
        QString m_password;
        AtcRating m_rating = AtcRating::Unknown;
        int m_protocolRevision = 0;
        QString m_realName;
    };

    //! Equal operator
    inline bool operator==(const AddAtc &lhs, const AddAtc &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() && lhs.cid() == rhs.cid() &&
               lhs.password() == rhs.password() && lhs.rating() == rhs.rating() &&
               lhs.protocolRevision() == rhs.protocolRevision() && lhs.realName() == rhs.realName();
    }

    //! Not equal operator
    inline bool operator!=(const AddAtc &lhs, const AddAtc &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_ADDATC_H
