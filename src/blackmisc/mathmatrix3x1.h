/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_POSMATRIX3X1_H
#define BLACKMISC_POSMATRIX3X1_H

#include "blackmisc/mathmatrixbase.h"
#include "blackmisc/mathvector3d.h"

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief 3D matrix
 */
class CMatrix3x1 : public CMatrixBase<CMatrix3x1, 3, 1>
{
public:
    /*!
     * \brief CMatrix3D
     */
    CMatrix3x1() : CMatrixBase() {}

    /*!
     * \brief init with value
     * \param fillValue
     */
    CMatrix3x1(qreal fillValue) : CMatrixBase(fillValue) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CMatrix3x1(const CMatrix3x1 &otherMatrix) : CMatrixBase(otherMatrix) {}

    /*!
     * \brief Convert to vector
     * \return
     */
    CVector3D toVector3D() const
    {
        return CVector3D(this->getElement(0, 0), this->getElement(1, 0), this->getElement(2, 0));
    }

    /*!
     * \brief Convert from vector
     * \return
     */
    void fromVector3D(const CVector3D &vector)
    {
        this->m_matrix[0][0] = vector.x();
        this->m_matrix[1][0] = vector.y();
        this->m_matrix[2][0] = vector.z();
    }
};

} // namespace

} // namespace

#endif // guard
