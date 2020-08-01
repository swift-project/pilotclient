/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_FSDIDENTIFICATION_H
#define BLACKCORE_FSD_FSDIDENTIFICATION_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
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

            //! Properties
            //! @{
            QString m_serverVersion;
            QString m_initialChallenge;
            //! @}

        private:
            FSDIdentification();
        };
    }
}

#endif // guard
