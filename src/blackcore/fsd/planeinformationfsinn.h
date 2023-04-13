/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_PLANEINFORMATIONFSINN_H
#define BLACKCORE_FSD_PLANEINFORMATIONFSINN_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! This packet is sent in reply to a FSinn request (FSIPIR)
    class BLACKCORE_EXPORT PlaneInformationFsinn : public MessageBase
    {
    public:
        //! Constructor
        PlaneInformationFsinn(const QString &sender,
                              const QString &receiver,
                              const QString &airlineIcao,
                              const QString &aircraftIcao,
                              const QString &aircraftIcaoCombinedType,
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
        PlaneInformationFsinn();
    };

    //! Equal to operator
    inline bool operator==(const PlaneInformationFsinn &lhs, const PlaneInformationFsinn &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_airlineIcao == rhs.m_airlineIcao &&
               lhs.m_aircraftIcao == rhs.m_aircraftIcao &&
               lhs.m_aircraftIcaoCombinedType == rhs.m_aircraftIcaoCombinedType &&
               lhs.m_sendMModelString == rhs.m_sendMModelString;
    }

    //! Not equal to operator
    inline bool operator!=(const PlaneInformationFsinn &lhs, const PlaneInformationFsinn &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
