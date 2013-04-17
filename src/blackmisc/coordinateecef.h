/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_COORDINATEECEF_H
#define BLACKMISC_COORDINATEECEF_H
#include "blackmisc/mathvector3dbase.h"

namespace BlackMisc
{
namespace Geo
{
/*!
 * \brief Earth centered, earth fixed position
 */
class CCoordinateEcef : public BlackMisc::Math::CVector3DBase<CCoordinateEcef>
{
public:
    /*!
     * \brief Default constructor
     */
    CCoordinateEcef() : CVector3DBase() {}

    /*!
     * \brief Constructor by values
     * \param x
     * \param y
     * \param z
     */
    CCoordinateEcef(qreal x, qreal y, qreal z) : CVector3DBase(x, y, z) {}

    /*!
     * \brief x
     * \return
     */
    qreal x() const
    {
        return this->m_vector.x();
    }

    /*!
     * \brief y
     * \return
     */
    qreal y() const
    {
        return this->m_vector.y();
    }

    /*!
     * \brief z
     * \return
     */
    qreal z() const
    {
        return this->m_vector.z();
    }

    /*!
     * \brief Set x
     * \param x
     */
    void setX(qreal x)
    {
        this->m_vector.setX(x);
    }

    /*!
     * \brief Set y
     * \param y
     */
    void setY(qreal y)
    {
        this->m_vector.setY(y);
    }

    /*!
     * \brief Set z
     * \param z
     */
    void setZ(qreal z)
    {
        this->m_vector.setZ(z);
    }
};

} // namespace
} // namespace


#endif // guard
