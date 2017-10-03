/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_MATCHINGUTILS_H
#define BLACKMISC_SIMULATION_MATCHINGUTILS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{
    namespace Simulation
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
                BlackMisc::CStatusMessageList *log, const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft,
                const QString &message, const BlackMisc::CLogCategoryList &categories = {},
                BlackMisc::CStatusMessage::StatusSeverity s = BlackMisc::CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(
                BlackMisc::CStatusMessageList *log, const BlackMisc::Aviation::CCallsign &callsign,
                const QString &message, const BlackMisc::CLogCategoryList &categories = {},
                BlackMisc::CStatusMessage::StatusSeverity s = BlackMisc::CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static BlackMisc::CStatusMessage logMessage(
                const BlackMisc::Aviation::CCallsign &callsign,
                const QString &message, const BlackMisc::CLogCategoryList &categories = {},
                BlackMisc::CStatusMessage::StatusSeverity s = BlackMisc::CStatusMessage::SeverityInfo);
        };
    } // ns
} // ns

#endif // guard
