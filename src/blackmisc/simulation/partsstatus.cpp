//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/partsstatus.h"

#include "blackmisc/stringutils.h"

namespace BlackMisc::Simulation
{
    QString CPartsStatus::toQString() const
    {
        return QStringLiteral("Supported parts: ") % boolToYesNo(m_supportsParts) %
               QStringLiteral(" | reused: ") % boolToYesNo(m_resusedParts) %
               QStringLiteral(" | same: ") % boolToYesNo(m_isSameParts);
    }
}
