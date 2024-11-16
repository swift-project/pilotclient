// SPDX-FileCopyrightText: Copyright (C) 2022 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_VISUALPILOTDATATOGGLE_H
#define SWIFT_CORE_FSD_VISUALPILOTDATATOGGLE_H

#include "enums.h"
#include "messagebase.h"

namespace swift::core::fsd
{
    //! Message from server to start or stop sending visual pilot data updates.
    class SWIFT_CORE_EXPORT VisualPilotDataToggle : public MessageBase
    {
    public:
        //! Constructor
        VisualPilotDataToggle(const QString &sender, const QString &client, bool active);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static VisualPilotDataToggle fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$SF"; }

        //! @{
        //! Properties
        QString m_client;
        bool m_active = false;
        //! @}

    private:
        VisualPilotDataToggle();
    };

    //! Equal to operator
    inline bool operator==(const VisualPilotDataToggle &lhs, const VisualPilotDataToggle &rhs)
    {
        return lhs.m_client == rhs.m_client && lhs.m_active == rhs.m_active;
    }

    //! Not equal to operator
    inline bool operator!=(const VisualPilotDataToggle &lhs, const VisualPilotDataToggle &rhs)
    {
        return !(lhs == rhs);
    }
} // namespace swift::core::fsd

#endif // guard
