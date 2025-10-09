// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_REVBCLIENTPARTS_H
#define SWIFT_CORE_FSD_REVBCLIENTPARTS_H

#include "core/fsd/enums.h"
#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! This packet is used to translate client’s parts from RevB IVAO -MD.
    //! -MDMDN0104:262396:262396:262396
    //!
    class SWIFT_CORE_EXPORT RevBClientParts : public MessageBase
    {
    public:
        //! Constructor
        RevBClientParts(const QString &sender, const QString &partsval1, const QString &partsval2,
                        const QString &partsval3);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static RevBClientParts fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "-MD"; }

        //! @{
        //! Properties
        ClientQueryType m_queryType = ClientQueryType::Unknown;
        QStringList m_queryData;
        QString m_partsval1;
        QString m_partsval2;
        QString m_partsval3;
        //! @}

    private:
        RevBClientParts() = default;
    };

    //! Equal to operator
    inline bool operator==(const RevBClientParts &lhs, const RevBClientParts &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.m_partsval1 == rhs.m_partsval1 &&
               lhs.m_partsval2 == rhs.m_partsval2 && lhs.m_partsval3 == rhs.m_partsval3;
    }

    //! Not equal to operator
    inline bool operator!=(const RevBClientParts &lhs, const RevBClientParts &rhs) { return !(lhs == rhs); }

} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_REVBCLIENTPARTS_H
