/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_MATCHINGUTILS_H
#define BLACKMISC_SIMULATION_MATCHINGUTILS_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscexport.h"

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
                CStatusMessageList *log, const Simulation::CSimulatedAircraft &remoteAircraft,
                const QString &message, const CLogCategoryList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const Aviation::CCallsign &callsign,
                                            const QString &message, const CLogCategoryList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const Aviation::CAircraftIcaoCode &icao,
                                            const QString &message, const CLogCategoryList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const Aviation::CAirlineIcaoCode &icao,
                                            const QString &message, const CLogCategoryList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const Aviation::CLivery &livery,
                                            const QString &message, const CLogCategoryList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const Aviation::CCallsign &callsign,
                const QString &message, const CLogCategoryList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const Aviation::CAircraftIcaoCode &icaoCode,
                const QString &message, const CLogCategoryList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const Aviation::CAirlineIcaoCode &icaoCode,
                const QString &message, const CLogCategoryList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const Aviation::CLivery &livery,
                const QString &message, const CLogCategoryList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

        private:
            //! Default categories
            static const CLogCategoryList &defaultCategories();

            //! Categories
            static CLogCategoryList categories(const CLogCategoryList &extraCategories);
        };
    } // ns
} // ns

#endif // guard
