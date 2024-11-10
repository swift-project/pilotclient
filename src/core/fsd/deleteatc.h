// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_DELETEATC_H
#define SWIFT_CORE_FSD_DELETEATC_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! FSD Message Delete ATC
    class SWIFT_CORE_EXPORT DeleteAtc : public MessageBase
    {
    public:
        //! Constructor
        DeleteAtc(const QString &sender, const QString &cid);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static DeleteAtc fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#DA"; }

        QString m_cid; //!< id

    private:
        DeleteAtc();
    };

    //! Equal to operator
    inline bool operator==(const DeleteAtc &lhs, const DeleteAtc &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_cid == rhs.m_cid;
    }

    //! Not equal to operator
    inline bool operator!=(const DeleteAtc &lhs, const DeleteAtc &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
