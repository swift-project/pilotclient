/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "matchingutils.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc::Simulation
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
