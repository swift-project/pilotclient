/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/plane.h"

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{

/*
 * Default constructor
 */
CPlane::CPlane() : m_interpolator(nullptr), m_driver(nullptr) { }

/*
 * Constructor
 */
CPlane::CPlane(const QString &callsign, ISimulator *driver) : m_callsign(callsign), m_interpolator(NULL), m_driver(driver)
{
    m_interpolator = new CInterpolator();
    Q_ASSERT(m_interpolator);
    Q_ASSERT(driver);
}

/*
 * Add a position
 */
void CPlane::addPosition(const CCoordinateGeodetic &position, const CSpeed &groundVelocity, const BlackMisc::Aviation::CHeading &heading, const CAngle &pitch, const CAngle &bank)
{
    Q_ASSERT(m_interpolator);
    m_interpolator->pushUpdate(position, groundVelocity, heading, pitch, bank);
}

/*
 * Render this object
 */
void CPlane::render()
{
    // void
}

/*
 * Last update timestamp
 */
double CPlane::getLastUpdateTime() const
{
    return 0;
}

} // namespace BlackCore
