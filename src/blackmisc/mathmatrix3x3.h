/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_POSMATRIX3X3_H
#define BLACKMISC_POSMATRIX3X3_H
#include "blackmisc/mathmatrix1x3.h"
#include "blackmisc/mathmatrix3x1.h"

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief 3x3 matrix
 */
class CMatrix3x3 : public CMatrixBase<CMatrix3x3, 3, 3>
{
public:
    /*!
     * \brief CMatrix3D
     */
    CMatrix3x3() : CMatrixBase() {}

    /*!
     * \brief init with value
     * \param fillValue
     */
    CMatrix3x3(double fillValue) : CMatrixBase(fillValue) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CMatrix3x3(const CMatrix3x3 &otherMatrix) : CMatrixBase(otherMatrix) {}

    /*!
     * \brief Calculates the determinant of the matrix
     * \return
     */
    double determinant() const;

    /*!
     * \brief Calculate the inverse
     * \return
     */
    CMatrix3x3 inverse() const;

    /*!
     * \brief Operator *=
     * \param otherMatrix
     * \return
     */
    CMatrix3x3 &operator *=(const CMatrix3x3 &otherMatrix)
    {
        this->m_matrix = this->m_matrix * otherMatrix.m_matrix;
        return (*this);
    }

    /*!
     * \brief Operator *
     * \param otherMatrix
     * \return
     */
    CMatrix3x1 operator *(const CMatrix3x1 &otherMatrix) const
    {
        CMatrix3x1 m;
        m.m_matrix = this->m_matrix * otherMatrix.m_matrix;
        return m;
    }

    /*!
     * \brief Operator *
     * \param multiply
     * \return
     */
    CMatrix3x3 operator *(const CMatrix3x3 &otherMatrix) const
    {
        CMatrix3x3 m(otherMatrix);
        m *= otherMatrix;
        return m;
    }

    /*!
     * \brief Multiply vector with this 3x3 matrix
     * \param vector
     * \return
     */
    CVector3D operator *(const CVector3D &vector) const
    {
        CVector3D v(vector);
        v.matrixMultiplication(*this);
        return v;
    }

    /*!
     * \brief Transposed matrix
     * \return
     */
    CMatrix3x3 transposed() const
    {
        CMatrix3x3 m(0.0);
        m.m_matrix = this->m_matrix.transposed();
        return m;
    }

    /*!
     * \brief Get column
     * \param column
     * \return
     */
    CMatrix3x1 getColumn(int column) const;

    /*!
     * \brief Get row
     * \param column
     * \return
     */
    CMatrix1x3 getRow(int column) const;

};

} // namespace

} // namespace

#endif // BLACKMISC_POSMATRIX3X3_H
