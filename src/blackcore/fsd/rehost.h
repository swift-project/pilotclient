// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_REHOST_H
#define BLACKCORE_FSD_REHOST_H

#include "messagebase.h"
#include "enums.h"

namespace BlackCore::Fsd
{
    //! The server requests us to connect to a different server.
    class BLACKCORE_EXPORT Rehost : public MessageBase
    {
    public:
        //! Constructor
        Rehost(const QString &sender, const QString &hostname);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static Rehost fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$XX"; }

        //! @{
        //! Properties
        QString m_hostname;
        //! @}

    private:
        Rehost();
    };

    //! Equal to operator
    inline bool operator==(const Rehost &lhs, const Rehost &rhs)
    {
        return lhs.m_hostname == rhs.m_hostname;
    }

    //! Not equal to operator
    inline bool operator!=(const Rehost &lhs, const Rehost &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
