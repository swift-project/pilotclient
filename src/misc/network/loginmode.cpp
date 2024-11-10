// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/loginmode.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CLoginMode)

namespace swift::misc::network
{
    QString CLoginMode::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)

        switch (m_loginMode)
        {
        case Pilot: return QStringLiteral("Pilot");
        case Observer: return QStringLiteral("Observer");
        }

        Q_UNREACHABLE();
        return {};
    }

} // namespace
