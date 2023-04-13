/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_PING_H
#define BLACKCORE_FSD_PING_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! Ping. Needs to be answered with a pong.
    class BLACKCORE_EXPORT Ping : public MessageBase
    {
    public:
        //! Constructor
        Ping(const QString &sender, const QString &receiver, const QString &timestamp);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static Ping fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$PI"; }

        QString m_timestamp; //!< timestamp

    private:
        Ping();
    };

    //! Equal to operator
    inline bool operator==(const Ping &lhs, const Ping &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_timestamp == rhs.m_timestamp;
    }

    //! Not equal to operator
    inline bool operator!=(const Ping &lhs, const Ping &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
