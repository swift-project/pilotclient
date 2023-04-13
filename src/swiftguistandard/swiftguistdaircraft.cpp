/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextownaircraft.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "swiftguistd.h"

#include <QString>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;

bool SwiftGuiStd::reloadOwnAircraft()
{
    if (!sApp || sApp->isShuttingDown()) { return false; }
    if (!sApp->getIContextOwnAircraft() || !sApp->getIContextNetwork()) { return false; }

    // check for changed aircraft
    const CSimulatedAircraft contextAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
    if (contextAircraft == m_ownAircraft) { return false; }
    m_ownAircraft = contextAircraft;
    return true;
}

void SwiftGuiStd::setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const CAltitude &altitude, const CAltitude &pressureAltitude)
{
    if (!sGui) { return; }
    CCoordinateGeodetic coordinate(
        CLatitude::fromWgs84(wgsLatitude),
        CLongitude::fromWgs84(wgsLongitude),
        CAltitude(0, CLengthUnit::m()));

    m_ownAircraft.setPosition(coordinate);
    m_ownAircraft.setAltitude(altitude);
    sGui->getIContextOwnAircraft()->updateOwnPosition(coordinate, altitude, pressureAltitude);
}
