/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistd.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Audio;

/*
 * Load own aircraft
 */
bool SwiftGuiStd::ps_reloadOwnAircraft()
{
    if (!this->isContextNetworkAvailableCheck()) return false;

    // check for changed aircraft
    bool changed = false;
    CAircraft loadedAircraft = this->getIContextOwnAircraft()->getOwnAircraft();
    if (loadedAircraft != this->m_ownAircraft)
    {
        this->m_ownAircraft = loadedAircraft;
        changed = true;
    }
    return changed;
}

/*
* Position
*/
void SwiftGuiStd::setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const CAltitude &altitude)
{
    CCoordinateGeodetic coordinate(
        CLatitude::fromWgs84(wgsLatitude),
        CLongitude::fromWgs84(wgsLongitude),
        CLength(0, CLengthUnit::m()));

    this->m_ownAircraft.setPosition(coordinate);
    this->m_ownAircraft.setAltitude(altitude);
    this->getIContextOwnAircraft()->updateOwnPosition(coordinate, altitude, SwiftGuiStd::swiftGuiStandardOriginator());
}
