//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/partsstatus.h"

#include "misc/stringutils.h"

namespace swift::misc::simulation
{
    QString CPartsStatus::toQString() const
    {
        return QStringLiteral("Supported parts: ") % boolToYesNo(m_supportsParts) % QStringLiteral(" | reused: ") %
               boolToYesNo(m_resusedParts) % QStringLiteral(" | same: ") % boolToYesNo(m_isSameParts);
    }
} // namespace swift::misc::simulation
