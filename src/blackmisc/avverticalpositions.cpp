/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avverticalpositions.h"

using BlackMisc::PhysicalQuantities::CLength;
using BlackMisc::PhysicalQuantities::CLengthUnit;
using BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants;

namespace BlackMisc
{
namespace Aviation
{

/*
 * Constructor
 */
CAviationVerticalPositions::CAviationVerticalPositions() :
    m_altitude(CAltitude(0, true, CLengthUnit::ft())),
    m_height(CPhysicalQuantitiesConstants::Length0ft()),
    m_elevation(CPhysicalQuantitiesConstants::Length0ft())
{
    // void
}

/*
 * Constructor
 */
CAviationVerticalPositions::CAviationVerticalPositions(const CAltitude &altitude, const CLength &elevation, const CLength &height) :
    m_altitude(altitude), m_elevation(elevation), m_height(height)
{
    // void
}

/*
 * Copy constructor
 */
CAviationVerticalPositions::CAviationVerticalPositions(const CAviationVerticalPositions &otherPosition) :
    m_altitude(otherPosition.m_altitude), m_elevation(otherPosition.m_elevation), m_height(otherPosition.m_height)
{
    // void
}

/*
 * Assigment
 */
CAviationVerticalPositions &CAviationVerticalPositions::operator =(const CAviationVerticalPositions &otherPositions)
{
    if (this == &otherPositions) return *this; // Same object?
    this->m_altitude = otherPositions.m_altitude;
    this->m_elevation = otherPositions.m_elevation;
    this->m_height = otherPositions.m_height;
    return *this;
}

/*
 * Equal operator
 */
bool CAviationVerticalPositions::operator ==(const CAviationVerticalPositions &otherPositions)
{
    return this->m_altitude == otherPositions.m_altitude &&
           this->m_elevation == otherPositions.m_elevation &&
           this->m_height == otherPositions.m_height;
}

/*
 * Unequal operator
 */
bool CAviationVerticalPositions::operator !=(const CAviationVerticalPositions &otherPositions)
{
    return !(*this == otherPositions);
}

/*
 * String representation for streaming
 */
QString CAviationVerticalPositions::stringForStreamingOperator() const
{
    QString s = QString("Altitude: ").
                append(this->m_altitude.unitValueRoundedWithUnit()).
                append(" Elevation: ").
                append(this->m_elevation.unitValueRoundedWithUnit()).
                append(" Height: ").
                append(this->m_height.unitValueRoundedWithUnit());
    return s;
}

/*
 * Factory by elevation and altitude
 */
CAviationVerticalPositions CAviationVerticalPositions::fromAltitudeAndElevationInFt(double altitudeMslFt, double elevationFt)
{
    CAltitude a(altitudeMslFt, true, CLengthUnit::ft());
    CLength e(elevationFt, CLengthUnit::ft());
    CLength h(altitudeMslFt - elevationFt, CLengthUnit::ft());
    return CAviationVerticalPositions(a, e, h);
}

/*
 * Factory by elevation and altitude
 */
CAviationVerticalPositions CAviationVerticalPositions::fromAltitudeAndElevationInM(double altitudeMslM, double elevationM)
{
    CAltitude a(altitudeMslM, true, CLengthUnit::m());
    CLength e(elevationM, CLengthUnit::m());
    CLength h(altitudeMslM - elevationM, CLengthUnit::m());
    return CAviationVerticalPositions(a, e, h);
}

/*
 * Stream for log message
 */
CLogMessage operator <<(CLogMessage log, const CAviationVerticalPositions &positions)
{
    log << positions.stringForStreamingOperator();
    return log;
}

/*
 * Stream for qDebug
 */
QDebug operator <<(QDebug d, const CAviationVerticalPositions &positions)
{
    d <<  positions.stringForStreamingOperator();
    return d;
}

} // namespace
} // namespace
