/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/loginmode.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Network, CLoginMode)

namespace BlackMisc::Network
{
    QString CLoginMode::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)

        switch (m_loginMode)
        {
        case Pilot:    return QStringLiteral("Pilot");
        case Observer: return QStringLiteral("Observer");
        }

        Q_UNREACHABLE();
        return {};
    }

} // namespace
