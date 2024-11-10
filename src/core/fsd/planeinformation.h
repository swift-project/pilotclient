// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_PLANEINFORMATION_H
#define SWIFT_CORE_FSD_PLANEINFORMATION_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! This packet is sent in reply to a PIR request to inform the client which multiplayer model to use.
    //! The airline and livery fields are optional.
    class SWIFT_CORE_EXPORT PlaneInformation : public MessageBase
    {
    public:
        //! Constructor
        PlaneInformation(const QString &sender, const QString &receiver, const QString &aircraft, const QString &airline, const QString &livery);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static PlaneInformation fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#SB"; }

        //! @{
        //! Properties
        QString m_aircraft;
        QString m_airline;
        QString m_livery;
        //! @}

    private:
        PlaneInformation();
    };

    //! Equal to operator
    inline bool operator==(const PlaneInformation &lhs, const PlaneInformation &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_aircraft == rhs.m_aircraft &&
               lhs.m_airline == rhs.m_airline &&
               lhs.m_livery == rhs.m_livery;
    }

    //! Not equal to operator
    inline bool operator!=(const PlaneInformation &lhs, const PlaneInformation &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
