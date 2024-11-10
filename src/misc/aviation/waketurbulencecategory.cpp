// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "waketurbulencecategory.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CWakeTurbulenceCategory)

namespace swift::misc::aviation
{
    CWakeTurbulenceCategory::CWakeTurbulenceCategory(WakeTurbulenceCategory wtc) : m_wtc(wtc)
    {
    }

    CWakeTurbulenceCategory::CWakeTurbulenceCategory(QChar letter)
    {
        letter = letter.toUpper();
        if (letter == 'L')
        {
            m_wtc = LIGHT;
        }
        else if (letter == 'M')
        {
            m_wtc = MEDIUM;
        }
        else if (letter == 'H')
        {
            m_wtc = HEAVY;
        }
        else if (letter == 'J')
        {
            m_wtc = SUPER;
        }
        else
        {
            // This includes "-" which is used in the database for aircraft with unknown wake turbulence category
            m_wtc = UNKNOWN;
        }
    }

    QString CWakeTurbulenceCategory::convertToQString(bool) const
    {
        switch (m_wtc)
        {
        case WakeTurbulenceCategory::LIGHT: return QStringLiteral("L");
        case WakeTurbulenceCategory::MEDIUM: return QStringLiteral("M");
        case WakeTurbulenceCategory::HEAVY: return QStringLiteral("H");
        case WakeTurbulenceCategory::SUPER: return QStringLiteral("J");
        default: [[fallthrough]];
        case WakeTurbulenceCategory::UNKNOWN: return QStringLiteral("-");
        }
    }
}
