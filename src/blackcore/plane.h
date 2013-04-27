/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_PLANE_H
#define BLACKCORE_PLANE_H
#include "blackcore/interpolator.h"

namespace BlackCore
{

class ISimulator;

/*!
 * \brief Plane class
 */
class CPlane
{
public:

    enum ESquawkMode {Standby   = 'S',
                      Charlie   = 'N',
                      Ident     = 'Y'
                     };

    /*!
     * \brief Default constructor
     */
    CPlane();

    /*!
     * \brief Constructor
     * \param callsign
     * \param driver
     */
    CPlane(const QString &callsign, ISimulator *driver);

    /*!
     * \brief Add a position (for the interpolator)
     * \param position
     * \param groundVelocity
     * \param heading
     * \param pitch
     * \param bank
     */
    void addPosition(const BlackMisc::Geo::CCoordinateGeodetic &position,
                     const BlackMisc::PhysicalQuantities::CSpeed &groundVelocity,
                     const BlackMisc::Aviation::CHeading &heading,
                     const BlackMisc::PhysicalQuantities::CAngle &pitch,
                     const BlackMisc::PhysicalQuantities::CAngle &bank);

    /*!
     * \brief Returns the callsign of the multiplayer plane
     * \return
     */
    QString callsign() { return m_callsign; }


    /*!
     * \brief render
     */
    void render();

    /*!
     * \brief Last updated timestamp
     * \return
     */
    double getLastUpdateTime() const;

private:

    QString         m_callsign;
    CInterpolator   *m_interpolator;
    ISimulator      *m_driver;
};

} // namespace BlackCore

#endif // guard
