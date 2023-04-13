/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_DELETEPILOT_H
#define BLACKCORE_FSD_DELETEPILOT_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! Used to notify the server of the intention to close the connection.
    //! If a client receives this packet it should remove the client from its internal database.
    class BLACKCORE_EXPORT DeletePilot : public MessageBase
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
