/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_PLANEINFORMATION_H
#define BLACKCORE_FSD_PLANEINFORMATION_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! This packet is sent in reply to a PIR request to inform the client which multiplayer model to use.
    //! The airline and livery fields are optional.
    class BLACKCORE_EXPORT PlaneInformation : public MessageBase
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

        //! Properties
        //! @{
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
        return  lhs.sender() == rhs.sender() &&
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
