// SPDX-FileCopyrightText: Copyright (C) 2022 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_VISUALPILOTDATATOGGLE_H
#define BLACKCORE_FSD_VISUALPILOTDATATOGGLE_H

#include "messagebase.h"
#include "enums.h"

namespace BlackCore::Fsd
{
    //! Message from server to start or stop sending visual pilot data updates.
    class BLACKCORE_EXPORT VisualPilotDataToggle : public MessageBase
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
}

#endif // guard
