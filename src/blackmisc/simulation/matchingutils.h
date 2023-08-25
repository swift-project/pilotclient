// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_MATCHINGUTILS_H
#define BLACKMISC_SIMULATION_MATCHINGUTILS_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Simulation
{
    /*!
     * Matching utils
     */
    class BLACKMISC_EXPORT CMatchingUtils
    {
    public:
        //! Specialized log for matching / reverse lookup
        //! \threadsafe
        static void addLogDetailsToList(
            CStatusMessageList *log, const Simulation::CSimulatedAircraft &remoteAircraft,
            const QString &message, const QStringList &extraCategories = {},
            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

    private:
        //! Default categories
        static const CLogCategoryList &defaultCategories();

        //! Categories
        static CLogCategoryList categories(const QStringList &extraCategories);
    };
} // ns

#endif // guard
