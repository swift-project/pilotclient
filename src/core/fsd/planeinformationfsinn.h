// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_PLANEINFORMATIONFSINN_H
#define SWIFT_CORE_FSD_PLANEINFORMATIONFSINN_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! This packet is sent in reply to a FSinn request (FSIPIR)
    class SWIFT_CORE_EXPORT PlaneInformationFsinn : public MessageBase
    {
    public:
        //! Constructor
        PlaneInformationFsinn(const QString &sender, const QString &receiver, const QString &airlineIcao,
                              const QString &aircraftIcao, const QString &aircraftIcaoCombinedType,
                              const QString &sendMModelString);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static PlaneInformationFsinn fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#SB"; }

        //! @{
        //! Properties
        QString m_airlineIcao;
        QString m_aircraftIcao;
        QString m_aircraftIcaoCombinedType;
        QString m_sendMModelString;
        //! @}

    private:
        PlaneInformationFsinn() = default;
    };

    //! Equal to operator
    inline bool operator==(const PlaneInformationFsinn &lhs, const PlaneInformationFsinn &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() &&
               lhs.m_airlineIcao == rhs.m_airlineIcao && lhs.m_aircraftIcao == rhs.m_aircraftIcao &&
               lhs.m_aircraftIcaoCombinedType == rhs.m_aircraftIcaoCombinedType &&
               lhs.m_sendMModelString == rhs.m_sendMModelString;
    }

    //! Not equal to operator
    inline bool operator!=(const PlaneInformationFsinn &lhs, const PlaneInformationFsinn &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_PLANEINFORMATIONFSINN_H
