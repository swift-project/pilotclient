/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_COORDINATENED_H
#define BLACKMISC_COORDINATENED_H
#include "blackmisc/mathvector3dbase.h"

namespace BlackMisc
{
namespace Geo
{
/*!
 * \brief North, East, Down
 */
class CCoordinateNed : public BlackMisc::Math::CVector3DBase<CCoordinateNed>
{
public:
    /*!
     * \brief Default constructor
     */
    CCoordinateNed() : CVector3DBase() {}

    /*!
     * \brief Constructor by values
     * \param north
     * \param east
     * \param down
     */
    CCoordinateNed(qreal north, qreal east, qreal down) : CVector3DBase(north, east, down) {}

    /*!
     * \brief North
     * \return
     */
    qreal north() const
    {
        return this->m_vector.x();
    }

    /*!
     * \brief East
     * \return
     */
    qreal east() const
    {
        return this->m_vector.y();
    }

    /*!
     * \brief Down
     * \return
     */
    qreal down() const
    {
        return this->m_vector.z();
    }

    /*!
     * \brief Set north
     * \param north
     */
    void setNorth(qreal north)
    {
        this->m_vector.setX(north);
    }

    /*!
     * \brief Set east
     * \param east
     */
    void setEast(qreal east)
    {
        this->m_vector.setY(east);
    }

    /*!
     * \brief Set down
     * \param down
     */
    void setDown(qreal down)
    {
        this->m_vector.setZ(down);
    }
};

} // namespace
} // namespace


#endif // guard
