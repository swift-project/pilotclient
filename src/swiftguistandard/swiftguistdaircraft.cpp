// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <QString>

#include "swiftguistd.h"

#include "core/context/contextownaircraft.h"
#include "gui/guiapplication.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/mixin/mixincompare.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/simulation/simulatedaircraft.h"

using namespace swift::core;
using namespace swift::misc;
using namespace swift::gui;
using namespace swift::misc::network;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::audio;

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
