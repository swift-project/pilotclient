// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_FSDIDENTIFICATION_H
#define BLACKCORE_FSD_FSDIDENTIFICATION_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! This packet is sent by the server immediately after a new client connection is made.
    class BLACKCORE_EXPORT FSDIdentification : public MessageBase
    {
    public:
        //! Constructor
        FSDIdentification(const QString &callsign, const QString &receiver, const QString &serverVersion, const QString &initialChallenge);

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
}

#endif // guard
