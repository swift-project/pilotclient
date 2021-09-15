/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_ADDPILOT_H
#define BLACKCORE_FSD_ADDPILOT_H

#include "blackcore/fsd/messagebase.h"
#include "blackcore/fsd/enums.h"

#include <QString>
#include <QStringList>

namespace BlackCore::Fsd
{
    //! FSD Message: Add Pilot
    class BLACKCORE_EXPORT AddPilot : public MessageBase
    {
    public:
        //! Constructor
        AddPilot(const QString &callsign, const QString &cid, const QString &password, PilotRating rating, int protocolRevision, SimType simType, const QString &realName);

        //! Get user cid
        const QString &cid() const { return m_cid; }

        //! Get user password
        const QString &password() const { return m_password; }

        //! Get pilot rating
        PilotRating rating() const { return m_rating; }

        //! Get protocol version
        int protocolVersion() const { return m_protocolRevision; }

        //! Get simulator type
        SimType simType() const { return m_simType; }

        //! Get real name
        const QString &realName() const { return m_realName; }

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static AddPilot fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#AP"; }

    private:
        AddPilot() : MessageBase() {}

        QString m_cid;
        QString m_password;
        PilotRating m_rating = PilotRating::Unknown;
        int m_protocolRevision = 0;
        SimType m_simType = SimType::Unknown;
        QString m_realName;
    };

    //! AddPilot equal operator
    inline bool operator==(const AddPilot &lhs, const AddPilot &rhs)
    {
        return  lhs.sender() == rhs.sender() &&
                lhs.receiver() == rhs.receiver() &&
                lhs.cid() == rhs.cid() &&
                lhs.password() == rhs.password() &&
                lhs.rating() == rhs.rating() &&
                lhs.protocolVersion() == rhs.protocolVersion() &&
                lhs.simType() == rhs.simType() &&
                lhs.realName() == rhs.realName();
    }

    //! AddPilot not equal operator
    inline bool operator!=(const AddPilot &lhs, const AddPilot &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
