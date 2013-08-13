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
 * \brief Matrix 3x1
 */
class CMatrix3x1 : public CMatrixBase<CMatrix3x1, 3, 1>
{
    friend class CMatrix3x3; // for matrix multiplicaion to access m_matrix

public:
    /*!
     * \brief Constructor
     */
    CMatrix3x1() : CMatrixBase() {}

    /*!
     * \brief Constructor
     * \param r1
     * \param r2
     * \param r3
     */
    CMatrix3x1(double r1, double r2, double r3) : CMatrixBase()
    {
        this->m_matrix(0, 0) = r1;
        this->m_matrix(1, 0) = r2;
        this->m_matrix(2, 0) = r3;
    }

    /*!
     * \brief Copy constructor
     * \param otherMatrix
     */
    CMatrix3x1(const CMatrix3x1 &other) : CMatrixBase(other) {}

    /*!
     * \brief Init by fill value
     * \param fillValue
     */
    explicit CMatrix3x1(double fillValue) : CMatrixBase(fillValue) {}

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
        this->m_matrix(0, 0) = vector.i();
        this->m_matrix(1, 0) = vector.j();
        this->m_matrix(2, 0) = vector.k();
    }
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Math::CMatrix3x1)

#endif // guard
