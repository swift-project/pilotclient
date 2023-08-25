// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
