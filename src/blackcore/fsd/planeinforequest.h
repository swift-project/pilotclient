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

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! Request to send plane information.
    //! Shall be answered by a PlaneInformation message.
    class BLACKCORE_EXPORT PlaneInfoRequest : public MessageBase
    {
    public:
        //! Ctor
        PlaneInfoRequest(const QString &sender, const QString &receiver);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static PlaneInfoRequest fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return QStringLiteral("#SB"); }

    private:
        PlaneInfoRequest();
    };

    //! Equal to operator
    inline bool operator==(const PlaneInfoRequest &lhs, const PlaneInfoRequest &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver();
    }

    //! Not equal to operator
    inline bool operator!=(const PlaneInfoRequest &lhs, const PlaneInfoRequest &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
