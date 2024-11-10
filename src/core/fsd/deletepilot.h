// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_DELETEPILOT_H
#define SWIFT_CORE_FSD_DELETEPILOT_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! Used to notify the server of the intention to close the connection.
    //! If a client receives this packet it should remove the client from its internal database.
    class SWIFT_CORE_EXPORT DeletePilot : public MessageBase
    {
    public:
        //! Constructor
        DeletePilot(const QString &sender, const QString &cid);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static DeletePilot fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return QStringLiteral("#DP"); }

        QString m_cid; //!< id

    private:
        //! Ctor
        DeletePilot();
    };

    //! Equal to operator
    inline bool operator==(const DeletePilot &lhs, const DeletePilot &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_cid == rhs.m_cid;
    }

    //! Not equal to operator
    inline bool operator!=(const DeletePilot &lhs, const DeletePilot &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
