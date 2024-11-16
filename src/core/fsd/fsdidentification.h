// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_FSDIDENTIFICATION_H
#define SWIFT_CORE_FSD_FSDIDENTIFICATION_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! This packet is sent by the server immediately after a new client connection is made.
    class SWIFT_CORE_EXPORT FSDIdentification : public MessageBase
    {
    public:
        //! Constructor
        FSDIdentification(const QString &callsign, const QString &receiver, const QString &serverVersion,
                          const QString &initialChallenge);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static FSDIdentification fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$DI"; }

        //! @{
        //! Properties
        QString m_serverVersion;
        QString m_initialChallenge;
        //! @}

    private:
        FSDIdentification();
    };
} // namespace swift::core::fsd

#endif // guard
