// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/connectionstatus.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CConnectionStatus)

namespace swift::misc::network
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

} // namespace swift::misc::network
