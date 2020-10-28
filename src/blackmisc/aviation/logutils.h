/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_MATCHINGUTILS_H
#define BLACKMISC_AVIATION_MATCHINGUTILS_H

#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Log.utilities
         */
        class BLACKMISC_EXPORT CLogUtilities
        {
        public:
            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const CCallsign &callsign,
                                            const QString &message, const QStringList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const CAircraftIcaoCode &icao,
                                            const QString &message, const QStringList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const CAirlineIcaoCode &icao,
                                            const QString &message, const QStringList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const CLivery &livery,
                                            const QString &message, const QStringList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const CCallsign &callsign,
                const QString &message, const QStringList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const CAircraftIcaoCode &icaoCode,
                const QString &message, const QStringList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const CAirlineIcaoCode &icaoCode,
                const QString &message, const QStringList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(
                const CLivery &livery,
                const QString &message, const QStringList &extraCategories = {},
                CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

        private:
            //! Default categories
            static const CLogCategoryList &defaultCategories();

            //! Categories
            static CLogCategoryList categories(const QStringList &extraCategories);
        };
    } // ns
} // ns

#endif // guard
