/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftlights.h"

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftLights::CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn)
            : m_strobeOn(strobeOn), m_landingOn(landingOn), m_taxiOn(taxiOn), m_beaconOn(beaconOn), m_navOn(navOn), m_logoOn(logoOn)
        {
        }

        CAircraftLights CAircraftLights::allLightsOn()
        {
            return CAircraftLights {true, true, true, true, true, true};
        }

        CAircraftLights CAircraftLights::allLightsOff()
        {
            return CAircraftLights {false, false, false, false, false, false};
        }

        QString CAircraftLights::convertToQString(bool /** i18n */) const
        {
            QString s;
            s += " strobe: ";
            s += m_strobeOn;
            s += " landing: ";
            s += m_landingOn;
            s += " taxi: ";
            s += m_taxiOn;
            s += " beacon: ";
            s += m_beaconOn;
            s += " nav: ";
            s += m_navOn;
            s += " logo: ";
            s += m_logoOn;
            return s;
        }
    } // namespace
} // namespace
