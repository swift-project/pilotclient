/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "terrainprobe.h"
#include "utils.h"
#include <XPLM/XPLMGraphics.h>
#include <limits>

namespace XSwiftBus
{
    CTerrainProbe::CTerrainProbe() : m_ref(XPLMCreateProbe(xplm_ProbeY)) {}

    CTerrainProbe::~CTerrainProbe() { XPLMDestroyProbe(m_ref); }

    double CTerrainProbe::getElevation(double degreesLatitude, double degreesLongitude, double metersAltitude, const std::string &callsign) const
    {
        double x, y, z;
        XPLMWorldToLocal(degreesLatitude, degreesLongitude, metersAltitude, &x, &y, &z);

        XPLMProbeInfo_t probe;
        probe.structSize = sizeof(probe);
        auto result = XPLMProbeTerrainXYZ(m_ref, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), &probe);
        if (result != xplm_ProbeHitTerrain)
        {
            std::string error;
            if (result == xplm_ProbeError) { error = "probe error"; }
            else if (result == xplm_ProbeMissed) { error = "probe missed"; }
            else { error = "unknown probe result"; }
            WARNING_LOG(callsign + " " + error + " at " + std::to_string(degreesLatitude) + ", " + std::to_string(degreesLongitude) + ", " + std::to_string(metersAltitude));

            return std::numeric_limits<double>::quiet_NaN();
        }
        if (probe.is_wet)
        {
            DEBUG_LOG(callsign + " probe hit water at " + std::to_string(degreesLatitude) + ", " + std::to_string(degreesLongitude) + ", " + std::to_string(metersAltitude));
        }

        XPLMLocalToWorld(probe.locationX, probe.locationY, probe.locationZ, &degreesLatitude, &degreesLongitude, &metersAltitude);
        return metersAltitude;
    }
} // ns
