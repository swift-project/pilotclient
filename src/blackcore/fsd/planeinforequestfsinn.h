/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_PLANEINFOREQUESTFSINN_H
#define BLACKCORE_FSD_PLANEINFOREQUESTFSINN_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! FSinn specific version of plane information request
    class BLACKCORE_EXPORT PlaneInfoRequestFsinn : public MessageBase
    {
    public:
        //! Constructor
        PlaneInfoRequestFsinn(const QString &sender,
                              const QString &receiver,
                              const QString &airlineIcao,
                              const QString &aircraftIcao,
                              const QString &aircraftIcaoCombinedType,
                              const QString &sendMModelString);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static PlaneInfoRequestFsinn fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return QStringLiteral("#SB"); }

        //! @{
        //! Properties
        QString m_airlineIcao;
        QString m_aircraftIcao;
        QString m_aircraftIcaoCombinedType;
        QString m_sendMModelString;
        //! @}

    private:
        PlaneInfoRequestFsinn();
    };

    //! Equal to operator
    inline bool operator==(const PlaneInfoRequestFsinn &lhs, const PlaneInfoRequestFsinn &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_airlineIcao == rhs.m_airlineIcao &&
               lhs.m_aircraftIcao == rhs.m_aircraftIcao &&
               lhs.m_aircraftIcaoCombinedType == rhs.m_aircraftIcaoCombinedType &&
               lhs.m_sendMModelString == rhs.m_sendMModelString;
    }

    //! Not equal to operator
    inline bool operator!=(const PlaneInfoRequestFsinn &lhs, const PlaneInfoRequestFsinn &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
