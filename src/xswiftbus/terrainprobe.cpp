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
#include <cmath>

namespace XSwiftBus
{
    CTerrainProbe::CTerrainProbe() : m_ref(XPLMCreateProbe(xplm_ProbeY)) {}

    CTerrainProbe::~CTerrainProbe() { XPLMDestroyProbe(m_ref); }

    std::array<double, 3> CTerrainProbe::getElevation(double degreesLatitude, double degreesLongitude, double metersAltitude) const
    {
        static const std::string callsign = "myself";
        bool unused = false;
        return getElevation(degreesLatitude, degreesLongitude, metersAltitude, callsign, unused);
    }

    std::array<double, 3> CTerrainProbe::getElevation(double degreesLatitude, double degreesLongitude, double metersAltitude, const std::string &callsign, bool &o_isWater) const
    {
        double x, y, z;
        XPLMWorldToLocal(degreesLatitude, degreesLongitude, metersAltitude, &x, &y, &z);

        XPLMProbeInfo_t probe;
        probe.structSize = sizeof(probe);
        auto result = XPLMProbeTerrainXYZ(m_ref, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), &probe);
        if (result != xplm_ProbeHitTerrain)
        {
            if (m_logMessageCount < 100)
            {
                m_logMessageCount++;
                std::string error;
                if (result == xplm_ProbeError) { error = "probe error"; }
                else if (result == xplm_ProbeMissed) { error = "probe missed"; }
                else { error = "unknown probe result"; }
                WARNING_LOG(callsign + " " + error + " at " + std::to_string(degreesLatitude) + ", " + std::to_string(degreesLongitude) + ", " + std::to_string(metersAltitude));
            }

            o_isWater = false;
            return { { std::numeric_limits<double>::quiet_NaN(), degreesLatitude, degreesLongitude } };
        }
        XPLMLocalToWorld(probe.locationX, probe.locationY, probe.locationZ, &degreesLatitude, &degreesLongitude, &metersAltitude);

        if (probe.is_wet && m_logMessageCount < 100)
        {
            m_logMessageCount++;
            DEBUG_LOG(callsign + " probe hit water at " + std::to_string(degreesLatitude) + ", " + std::to_string(degreesLongitude) + ", " + std::to_string(metersAltitude));
        }
        if (std::isnan(metersAltitude) && m_logMessageCount < 100)
        {
            m_logMessageCount++;
            DEBUG_LOG(callsign + " probe returned NaN at " + std::to_string(degreesLatitude) + ", " + std::to_string(degreesLongitude) + ", " + std::to_string(metersAltitude));
        }
        o_isWater = probe.is_wet;
        return { { metersAltitude, degreesLatitude, degreesLongitude } };
    }
} // ns
