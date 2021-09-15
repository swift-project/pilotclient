/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/simulation/remoteaircraftproviderdummy.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackMisc::Simulation
{
    CRemoteAircraftProviderDummy *CRemoteAircraftProviderDummy::instance()
    {
        static CRemoteAircraftProviderDummy *dummy = new CRemoteAircraftProviderDummy();
        return dummy;
    }

    CRemoteAircraftProviderDummy::CRemoteAircraftProviderDummy(QObject *parent) : CRemoteAircraftProvider(parent)
    { }

    void CRemoteAircraftProviderDummy::insertNewSituation(const CAircraftSituation &situation)
    {
        this->storeAircraftSituation(situation);
    }

    void CRemoteAircraftProviderDummy::insertNewSituations(const CAircraftSituationList &situations)
    {
        for (const CAircraftSituation &situation : situations)
        {
            this->storeAircraftSituation(situation);
        }
    }

    void CRemoteAircraftProviderDummy::insertNewAircraftParts(const CCallsign &callsign, const CAircraftParts &parts, bool removeOutdatedParts)
    {
        this->storeAircraftParts(callsign, parts, removeOutdatedParts);
    }

    void CRemoteAircraftProviderDummy::insertNewAircraftParts(const CCallsign &callsign, const CAircraftPartsList &partsList, bool removeOutdatedParts)
    {
        for (const CAircraftParts &parts : partsList)
        {
            this->storeAircraftParts(callsign, parts, removeOutdatedParts);
        }
    }

    CAirspaceAircraftSnapshot CRemoteAircraftProviderDummy::getLatestAirspaceAircraftSnapshot() const
    {
        return CAirspaceAircraftSnapshot();
    }
} // namespace
