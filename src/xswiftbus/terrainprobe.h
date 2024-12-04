// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_ELEVATIONPROVIDER_H
#define SWIFT_SIM_XSWIFTBUS_ELEVATIONPROVIDER_H

#include <XPLM/XPLMScenery.h>

#include <array>
#include <string>

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
        //! \note Due to the Earth's curvature, the OpenGL vertical axis may not be exactly perpendicular to the surface
        //! of the geoid. \return NaN if no ground was detected.
        std::array<double, 3> getElevation(double degreesLatitude, double degreesLongitude,
                                           double metersAltitude) const;
        std::array<double, 3> getElevation(double degreesLatitude, double degreesLongitude, double metersAltitude,
                                           const std::string &callsign, bool &o_isWater) const;
        //! @}

    private:
        XPLMProbeRef m_ref = nullptr;
        mutable int m_logMessageCount = 0;
    };
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_ELEVATIONPROVIDER_H
