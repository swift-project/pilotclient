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

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! Sent or received as reply to a ping.
    class BLACKCORE_EXPORT Pong : public MessageBase
    {
    public:
        //! Constructor
        Pong(const QString &sender, const QString &receiver, const QString &timestamp);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static Pong fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$PO"; }

        QString m_timestamp; //!< timestamp

    private:
        Pong();
    };

    //! Equal to operator
    inline bool operator==(const Pong &lhs, const Pong &rhs)
    {
        return  lhs.sender() == rhs.sender() &&
                lhs.receiver() == rhs.receiver() &&
                lhs.m_timestamp == rhs.m_timestamp;
    }

    //! Not equal to operator
    inline bool operator!=(const Pong &lhs, const Pong &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
