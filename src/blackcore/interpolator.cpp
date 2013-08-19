/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.  */

#include "blackcore/interpolator.h"
#include <iostream>

using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackCore
{

/*
 * Constructor
 */
CInterpolator::CInterpolator() : m_state_begin(0), m_state_end(0)
{
    m_time.start();
}

/*
 * Virtual destructor
 */
CInterpolator::~CInterpolator()
{
    delete m_state_begin;
    delete m_state_end;
}

/*
 * Initialize
 */
void CInterpolator::initialize() {}

/*
 * Push an update
 */
CCoordinateNed CInterpolator::pushUpdate(const CCoordinateGeodetic &pos, const CSpeed &groundSpeed, const CHeading &heading, const CAngle &pitch, const CAngle &bank)
{
    CCoordinateNed velocityNED;
    if (m_state_begin == 0)
    {
        m_state_begin = new TPlaneState();
        m_state_begin->position = CCoordinateTransformation::toEcef(pos);
        m_state_begin->orientation.heading = heading;
        m_state_begin->orientation.pitch = pitch;
        m_state_begin->orientation.bank = bank;
        m_state_begin->groundspeed = groundSpeed;

        velocityNED =
            CCoordinateNed(pos,
                           cos(m_state_begin->orientation.heading.value(CAngleUnit::rad())) * m_state_begin->groundspeed.value(CSpeedUnit::m_s()),
                           sin(m_state_begin->orientation.heading.value(CAngleUnit::rad())) * m_state_begin->groundspeed.value(CSpeedUnit::m_s()), 0);

        m_state_begin->velocity = CCoordinateTransformation::toEcef(velocityNED).toMathVector();
        m_state_begin->timestamp = 0;
        return velocityNED;
    }

    stateNow(m_state_begin);
    if (m_state_end == 0) m_state_end = new TPlaneState();
    m_state_end->reset();

    m_state_end->timestamp = m_time.elapsed();
    m_state_end->position = CCoordinateTransformation::toEcef(pos);
    m_state_end->orientation.heading = CHeading(normalizeRadians(heading), CHeading::True);
    m_state_end->orientation.pitch = normalizeRadians(pitch);
    m_state_end->orientation.bank = normalizeRadians(bank);
    m_state_end->groundspeed = groundSpeed;

    velocityNED =
        CCoordinateNed(pos,
                       cos(m_state_end->orientation.heading.value(CAngleUnit::rad())) * m_state_end->groundspeed.value(CSpeedUnit::m_s()),
                       sin(m_state_end->orientation.heading.value(CAngleUnit::rad())) * m_state_end->groundspeed.value(CSpeedUnit::m_s()), 0);
    m_state_end->velocity = CCoordinateTransformation::toEcef(velocityNED).toMathVector();
    m_timeEnd = 5;

    double m_TFpow4 = CMath::cubic(m_timeEnd) * m_timeEnd;

    m_a = m_state_begin->velocity * CMath::square(m_timeEnd);
    m_a += m_state_end->velocity * CMath::square(m_timeEnd);

    m_a += m_state_begin->position.toMathVector() * m_timeEnd * 2;
    m_a -= m_state_end->position.toMathVector() * m_timeEnd * 2;
    m_a *= 6;
    m_a /= m_TFpow4;

    m_b = m_state_begin->velocity * CMath::cubic(m_timeEnd) * (-2) - m_state_end->velocity * CMath::cubic(m_timeEnd);
    m_b = m_b - m_state_begin->position.toMathVector() * CMath::square(m_timeEnd) * 3 + m_state_end->position.toMathVector() * CMath::square(m_timeEnd) * 3;
    m_b = m_b * 2 / (m_TFpow4);
    return velocityNED;
}

/*
 * Valid object?
 */
bool CInterpolator::isValid() const
{
    return (m_state_begin && m_state_end);
}

/*
 * Calculate current state
 */
bool CInterpolator::stateNow(TPlaneState *state)
{
    if (!this->isValid()) return false;
    double time = 5;

    // Plane Position
    double timePow2 = CMath::square(time);
    double timePow3 = CMath::cubic(time);

    CCoordinateEcef pos(m_b * 3 * timePow2 * m_timeEnd + m_a * timePow3 * m_timeEnd - m_b * 3 * time * CMath::square(m_timeEnd) - m_a * time * CMath::cubic(m_timeEnd));
    pos += m_state_begin->position * (-6) * time + m_state_begin->position * 6 * m_timeEnd + m_state_end->position * 6 * time;
    pos /= 6 * m_timeEnd;

    state->position = pos;

    CVector3D vel(m_a * (3 * m_timeEnd * CMath::square(time) - CMath::cubic(m_timeEnd)));
    vel += m_b * (6 * m_timeEnd * time - 3 * CMath::square(m_timeEnd)) + (m_state_end->position - m_state_begin->position).toMathVector() * 6;
    vel /= 6 * m_timeEnd;

    state->velocity = vel;
    state->velNED = CCoordinateTransformation::toNed(CCoordinateEcef(vel), CCoordinateTransformation::toGeodetic(pos));

    // Plane Orientation
    double vEast = state->velNED.east();
    double vNorth = state->velNED.north();
    state->orientation.heading = CHeading(atan2(vNorth, vEast), CHeading::True, CAngleUnit::rad());

    return true;
}

/*
 * Normalize radians, clarify what happens here
 */
CAngle CInterpolator::normalizeRadians(const CAngle &angle) const
{
    double radian = angle.value(CAngleUnit::rad());
    radian = radian - BlackMisc::Math::CMath::PI2() * floor(0.5 + radian / BlackMisc::Math::CMath::PI2());
    return CAngle(radian, CAngleUnit::rad());
}

} // namespace
