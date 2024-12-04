// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_MATCHINGUTILS_H
#define SWIFT_MISC_SIMULATION_MATCHINGUTILS_H

#include "misc/simulation/simulatedaircraft.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    /*!
     * Matching utils
     */
    class SWIFT_MISC_EXPORT CMatchingUtils
    {
    public:
        //! Specialized log for matching / reverse lookup
        //! \threadsafe
        static void addLogDetailsToList(CStatusMessageList *log, const CSimulatedAircraft &remoteAircraft,
                                        const QString &message, const QStringList &extraCategories = {},
                                        CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

    private:
        //! Default categories
        static const CLogCategoryList &defaultCategories();

        //! Categories
        static CLogCategoryList categories(const QStringList &extraCategories);
    };
} // namespace swift::misc::simulation

#endif // SWIFT_MISC_SIMULATION_MATCHINGUTILS_H
