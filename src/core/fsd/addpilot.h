// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_ADDPILOT_H
#define SWIFT_CORE_FSD_ADDPILOT_H

#include <QString>
#include <QStringList>

#include "core/fsd/enums.h"
#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! FSD Message: Add Pilot
    class SWIFT_CORE_EXPORT AddPilot : public MessageBase
    {
    public:
        //! Constructor
        AddPilot(const QString &callsign, const QString &cid, const QString &password, PilotRating rating,
                 int protocolRevision, SimType simType, const QString &realName);

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
        AddPilot() = default;

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
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() && lhs.cid() == rhs.cid() &&
               lhs.password() == rhs.password() && lhs.rating() == rhs.rating() &&
               lhs.protocolVersion() == rhs.protocolVersion() && lhs.simType() == rhs.simType() &&
               lhs.realName() == rhs.realName();
    }

    //! AddPilot not equal operator
    inline bool operator!=(const AddPilot &lhs, const AddPilot &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_ADDPILOT_H
