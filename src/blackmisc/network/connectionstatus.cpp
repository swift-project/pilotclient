// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/connectionstatus.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Network, CConnectionStatus)

namespace BlackMisc::Network
{
    QString CConnectionStatus::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);

        switch (m_connectionStatus)
        {
        case Connected: return QStringLiteral("Connected");
        case Connecting: return QStringLiteral("Connecting");
        case Disconnecting: return QStringLiteral("Disconnecting");
        case Disconnected: return QStringLiteral("Disconnected");
        }

        Q_UNREACHABLE();
        return {};
    }

} // namespace
