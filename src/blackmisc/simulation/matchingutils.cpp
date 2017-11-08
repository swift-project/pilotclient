/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "matchingutils.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const CSimulatedAircraft &remoteAircraft, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft.getCallsign(), message, extraCategories, s);
        }

        void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const CCallsign &callsign, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CMatchingUtils::logMessage(callsign, message, extraCategories, s));
        }

        void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const CAircraftIcaoCode &icao, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CMatchingUtils::logMessage(icao, message, extraCategories, s));
        }

        void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const CAirlineIcaoCode &icao, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CMatchingUtils::logMessage(icao, message, extraCategories, s));
        }

        void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const CLivery &livery, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(CMatchingUtils::logMessage(livery, message, extraCategories, s));
        }

        CStatusMessage CMatchingUtils::logMessage(const CCallsign &callsign, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, callsign.isEmpty() ? message.trimmed() : callsign.toQString() + ": " + message.trimmed());
            return m;
        }

        CStatusMessage CMatchingUtils::logMessage(const CAircraftIcaoCode &icaoCode, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, icaoCode.hasDesignator() ? icaoCode.getDesignatorDbKey() + ": " + message.trimmed() : message.trimmed());
            return m;
        }

        CStatusMessage CMatchingUtils::logMessage(const CAirlineIcaoCode &icaoCode, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, icaoCode.hasValidDesignator() ? icaoCode.getVDesignatorDbKey() + ": " + message.trimmed() : message.trimmed());
            return m;
        }

        CStatusMessage CMatchingUtils::logMessage(const CLivery &livery, const QString &message, const CLogCategoryList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            const CStatusMessage m(categories(extraCategories), s, livery.hasCombinedCode() ? livery.getCombinedCodePlusInfoAndId() + ": " + message.trimmed() : message.trimmed());
            return m;
        }

        const CLogCategoryList &CMatchingUtils::defaultCategories()
        {
            static const CLogCategoryList cats({ CLogCategory::matching() });
            return cats;
        }

        CLogCategoryList CMatchingUtils::categories(const CLogCategoryList &extraCategories)
        {
            if (extraCategories.isEmpty()) { return defaultCategories(); }
            CLogCategoryList cats(defaultCategories());
            cats.push_back(extraCategories);
            return cats;
        }
    } // ns
} // ns
