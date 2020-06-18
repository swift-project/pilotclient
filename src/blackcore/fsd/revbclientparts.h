/* Copyright (C) 2020
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_REVBCLIENTPARTS_H
#define BLACKCORE_FSD_REVBCLIENTPARTS_H

#include "messagebase.h"
#include "enums.h"

namespace BlackCore
{
    namespace Fsd
    {
        //! This packet is used to translate client’s parts from RevB IVAO -MD.
        //! -MDMDN0104:262396:262396:262396
        //!
        class BLACKCORE_EXPORT RevBClientParts : public MessageBase
        {
        public:
            //! Constructor
            RevBClientParts(const QString &sender, const QString &partsval1, const QString &partsval2, const QString &partsval3);


            //! Message converted to tokens
            QStringList toTokens() const;


            //! Construct from tokens
            static RevBClientParts fromTokens(const QStringList &tokens);

            //! PDU identifier
            static QString pdu() { return "-MD"; }

            //! Properties @{
            ClientQueryType m_queryType = ClientQueryType::Unknown;
            QStringList m_queryData;
            //! @}


            //! Properties @{
            QString  m_partsval1;
            QString  m_partsval2;
            QString  m_partsval3;
            //! @

        private:
            RevBClientParts();
        };

        //! Equal to operator
        inline bool operator==(const RevBClientParts &lhs, const RevBClientParts &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.m_partsval1 == rhs.m_partsval1 &&
                    lhs.m_partsval2 == rhs.m_partsval2 &&
                    lhs.m_partsval3 == rhs.m_partsval3;
        }

        //! Not equal to operator
        inline bool operator!=(const RevBClientParts &lhs, const RevBClientParts &rhs)
        {
            return !(lhs == rhs);
        }

    }
}

#endif // guard
