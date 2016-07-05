/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/matchingutils.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore;

namespace BlackCore
{
    void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const QString &message, const CLogCategoryList &categories, CStatusMessage::StatusSeverity s)
    {
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft.getCallsign(), message, categories, s);
    }

    void CMatchingUtils::addLogDetailsToList(CStatusMessageList *log, const BlackMisc::Aviation::CCallsign &callsign, const QString &message, const CLogCategoryList &categories, CStatusMessage::StatusSeverity s)
    {
        if (!log) { return; }
        if (message.isEmpty()) { return; }
        log->push_back(CMatchingUtils::logMessage(callsign, message, categories, s));
    }

    CStatusMessage CMatchingUtils::logMessage(const BlackMisc::Aviation::CCallsign &callsign, const QString &message, const CLogCategoryList &categories, CStatusMessage::StatusSeverity s)
    {
        const CStatusMessage m(categories, s, callsign.isEmpty() ? callsign.toQString() + ": " + message.trimmed() : message.trimmed());
        return m;
    }
} // namespace
