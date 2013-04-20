/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_COORDINATEECEF_H
#define BLACKMISC_COORDINATEECEF_H
#include "blackmisc/mathvector3d.h"

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
    CCoordinateEcef(double x, double y, double z) : CVector3DBase(x, y, z) {}

    /*!
     * \brief Constructor by math vector
     * \param vector
     */
    CCoordinateEcef(const BlackMisc::Math::CVector3D vector) : CVector3DBase(vector.i(), vector.j(), vector.k()) {}

    /*!
     * \brief x
     * \return
     */
    double x() const
    {
        return this->m_i;
    }

    /*!
     * \brief y
     * \return
     */
    double y() const
    {
        return this->m_j;
    }

    /*!
     * \brief z
     * \return
     */
    double z() const
    {
        return this->m_k;
    }

    /*!
     * \brief Set x
     * \param x
     */
    void setX(double x)
    {
        this->m_i = x;
    }

    /*!
     * \brief Set y
     * \param y
     */
    void setY(double y)
    {
        this->m_j = y;
    }

    /*!
     * \brief Set z
     * \param z
     */
    void setZ(double z)
    {
        this->m_k = z;
    }

    /*!
     * \brief Concrete implementation of a 3D vector
     * \return
     */
    BlackMisc::Math::CVector3D toMathVector() const
    {
        return BlackMisc::Math::CVector3D(this->x(), this->y(), this->z());
    }

protected:
    /*!
     * \brief String for converter
     * \return
     */
    virtual QString stringForConverter() const
    {
        QString s = "ECEF: {x %1, y %2, z %3}";
        s = s.arg(QString::number(this->x(), 'f', 6)).
            arg(QString::number(this->y(), 'f', 6)).
            arg(QString::number(this->z(), 'f', 6));
        return s;
    }

};

} // namespace
} // namespace


#endif // guard
