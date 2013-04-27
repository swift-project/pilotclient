/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.  */

#ifndef BLACKCORE_INTERPOLATOR_H
#define BLACKCORE_INTERPOLATOR_H

#include "blackmisc/coordinatetransformation.h"
#include "blackmisc/pqspeed.h"
#include "blackmisc/avheading.h"
#include <QElapsedTimer>

namespace BlackCore
{

/*!
 * \typedef Plane's orientation
 */
typedef struct
{
    BlackMisc::Aviation::CHeading heading; // honestly I think this is a track TODO
    BlackMisc::PhysicalQuantities::CAngle pitch;
    BlackMisc::PhysicalQuantities::CAngle bank;

} TOrientation;

/*!
 * \typedef Plane's state
 */
typedef struct
{
    /*!
     * \brief Reset data
     */
    void reset() {}

    qint64 timestamp;
    TOrientation orientation;
    BlackMisc::PhysicalQuantities::CSpeed groundspeed;
    BlackMisc::Math::CVector3D velocity;
    BlackMisc::Geo::CCoordinateEcef position;
    BlackMisc::Geo::CCoordinateNed velNED;

} TPlaneState;

/*!
 * \brief Interpolator, calculation inbetween positions
 */
class CInterpolator
{
public:
    /*!
     * \brief Default constructor
     */
    CInterpolator();

    /*!
     * \brief Virtual destructor
     */
    virtual ~CInterpolator();

    /*!
     * \brief Init object
     */
    void initialize();

    /*!
     * \brief Push an update
     * \param pos
     * \param groundSpeed
     * \param heading
     * \param pitch
     * \param bank
     * \return
     */
    BlackMisc::Geo::CCoordinateNed pushUpdate(const BlackMisc::Geo::CCoordinateGeodetic &pos,
            const BlackMisc::PhysicalQuantities::CSpeed &groundSpeed,
            const BlackMisc::Aviation::CHeading &heading,
            const BlackMisc::PhysicalQuantities::CAngle &pitch,
            const BlackMisc::PhysicalQuantities::CAngle &bank);

    /*!
     * \brief Valid state?
     * \return
     */
    bool isValid() const;

    /*!
     * \brief Calculate current state
     * \param state
     * \return
     */
    bool stateNow(TPlaneState *state);

private:
    BlackMisc::Math::CVector3D m_a;
    BlackMisc::Math::CVector3D m_b;
    QElapsedTimer m_time;
    TPlaneState *m_state_begin;
    TPlaneState *m_state_end;
    bool m_valid;
    double m_timeEnd;
    double m_timeBegin;

    /*!
     * \brief Normalize radians
     * \param angle
     * \return
     */
    BlackMisc::PhysicalQuantities::CAngle normalizeRadians(const BlackMisc::PhysicalQuantities::CAngle &angle) const;
};

} // namespace BlackCore

#endif // guard
