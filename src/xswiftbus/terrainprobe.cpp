/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "terrainprobe.h"
#include <XPLM/XPLMGraphics.h>
#include <limits>

namespace XSwiftBus
{
    CTerrainProbe::CTerrainProbe() : m_ref(XPLMCreateProbe(xplm_ProbeY)) {}

    CTerrainProbe::~CTerrainProbe() { XPLMDestroyProbe(m_ref); }

    double CTerrainProbe::getElevation(double degreesLatitude, double degreesLongitude, double metersAltitude) const
    {
        double x, y, z;
        XPLMWorldToLocal(degreesLatitude, degreesLongitude, metersAltitude, &x, &y, &z);

        XPLMProbeInfo_t probe;
        probe.structSize = sizeof(probe);
        auto result = XPLMProbeTerrainXYZ(m_ref, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), &probe);
        if (result != xplm_ProbeHitTerrain)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        XPLMLocalToWorld(probe.locationX, probe.locationY, probe.locationZ, &degreesLatitude, &degreesLongitude, &metersAltitude);
        return metersAltitude;
    }
} // ns
