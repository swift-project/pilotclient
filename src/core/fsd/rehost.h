// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_REHOST_H
#define SWIFT_CORE_FSD_REHOST_H

#include "enums.h"
#include "messagebase.h"

namespace swift::core::fsd
{
    //! The server requests us to connect to a different server.
    class SWIFT_CORE_EXPORT Rehost : public MessageBase
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
    inline bool operator==(const Rehost &lhs, const Rehost &rhs) { return lhs.m_hostname == rhs.m_hostname; }

    //! Not equal to operator
    inline bool operator!=(const Rehost &lhs, const Rehost &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_REHOST_H
