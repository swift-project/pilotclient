// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/matchingutils.h"

using namespace swift::misc::aviation;

namespace swift::misc::simulation
{
    void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const CSimulatedAircraft &remoteAircraft, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
    {
        CCallsign::addLogDetailsToList(log, remoteAircraft.getCallsign(), message, extraCategories, s);
    }

    const CLogCategoryList &CMatchingUtils::defaultCategories()
    {
        static const CLogCategoryList cats({ CLogCategories::matching() });
        return cats;
    }

    CLogCategoryList CMatchingUtils::categories(const QStringList &extraCategories)
    {
        if (extraCategories.isEmpty()) { return defaultCategories(); }
        CLogCategoryList cats(defaultCategories());
        cats.push_back(CLogCategoryList::fromQStringList(extraCategories));
        return cats;
    }
} // ns
