/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_ELEVATIONPROVIDER_H
#define BLACKSIM_XSWIFTBUS_ELEVATIONPROVIDER_H

#include <XPLM/XPLMScenery.h>
#include <string>
#include <array>

namespace XSwiftBus
{
    /*!
     * Class based interface to X-Plane SDK terrain probe.
     */
    class CTerrainProbe
    {
    public:
        //! Constructor.
        CTerrainProbe();

        //! Destructor;
        ~CTerrainProbe();

        //! @{
        //! Not copyable.
        CTerrainProbe(const CTerrainProbe &) = delete;
        CTerrainProbe &operator=(const CTerrainProbe &) = delete;
        //! @}

        //! @{
        //! Get the elevation in meters at the given point in OpenGL space.
        //! \note Due to the Earth's curvature, the OpenGL vertical axis may not be exactly perpendicular to the surface of the geoid.
        //! \return NaN if no ground was detected.
        std::array<double, 3> getElevation(double degreesLatitude, double degreesLongitude, double metersAltitude) const;
        std::array<double, 3> getElevation(double degreesLatitude, double degreesLongitude, double metersAltitude, const std::string &callsign, bool &o_isWater) const;
        //! @}

    private:
        XPLMProbeRef m_ref = nullptr;
        mutable int m_logMessageCount = 0;
    };
} // ns

#endif
