/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "logutils.h"

namespace BlackMisc
{
    namespace Aviation
    {
        void CLogUtilities::addLogDetailsToList(CStatusMessageList *log, const CCallsign &callsign, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CLogUtilities::logMessage(callsign, message, extraCategories, s));
        }

        void CLogUtilities::addLogDetailsToList(CStatusMessageList *log, const CAircraftIcaoCode &icao, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CLogUtilities::logMessage(icao, message, extraCategories, s));
        }

        void CLogUtilities::addLogDetailsToList(CStatusMessageList *log, const CAirlineIcaoCode &icao, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CLogUtilities::logMessage(icao, message, extraCategories, s));
        }

        void CLogUtilities::addLogDetailsToList(CStatusMessageList *log, const CLivery &livery, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CLogUtilities::logMessage(livery, message, extraCategories, s));
        }

        CStatusMessage CLogUtilities::logMessage(const CCallsign &callsign, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, callsign.isEmpty() ? message.trimmed() : callsign.toQString() + ": " + message.trimmed());
            return m;
        }

        CStatusMessage CLogUtilities::logMessage(const CAircraftIcaoCode &icaoCode, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, icaoCode.hasDesignator() ? icaoCode.getDesignatorDbKey() + ": " + message.trimmed() : message.trimmed());
            return m;
        }

        CStatusMessage CLogUtilities::logMessage(const CAirlineIcaoCode &icaoCode, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, icaoCode.hasValidDesignator() ? icaoCode.getVDesignatorDbKey() + ": " + message.trimmed() : message.trimmed());
            return m;
        }

        CStatusMessage CLogUtilities::logMessage(const CLivery &livery, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, livery.hasCombinedCode() ? livery.getCombinedCodePlusInfoAndId() + ": " + message.trimmed() : message.trimmed());
            return m;
        }

        const CLogCategoryList &CLogUtilities::defaultCategories()
        {
            static const CLogCategoryList cats({ CLogCategories::aviation() });
            return cats;
        }

        CLogCategoryList CLogUtilities::categories(const QStringList &extraCategories)
        {
            if (extraCategories.isEmpty()) { return defaultCategories(); }
            CLogCategoryList cats(defaultCategories());
            cats.push_back(CLogCategoryList::fromQStringList(extraCategories));
            return cats;
        }
    } // ns
} // ns
