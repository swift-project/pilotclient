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
     * \brief Copy constructor
     * \param otherMatrix
     */
    CMatrix3x3(const CMatrix3x3 &otherMatrix) : CMatrixBase(otherMatrix) {}

    /*!
     * \brief Init by fill value
     * \param fillValue
     */
    explicit CMatrix3x3(double fillValue) : CMatrixBase(fillValue) {}

    /*!
     * \brief Stupid but handy constructor
     * \param r1c1
     * \param r1c2
     * \param r1c3
     * \param r2c1
     * \param r2c2
     * \param r2c3
     * \param r3c1
     * \param r3c2
     * \param r3c3
     */
    explicit CMatrix3x3(double r1c1, double r1c2, double r1c3,
                        double r2c1, double r2c2, double r2c3,
                        double r3c1, double r3c2, double r3c3) : CMatrixBase()
    {
        this->setElement(0, 0, r1c1);
        this->setElement(0, 1, r1c2);
        this->setElement(0, 2, r1c3);
        this->setElement(1, 0, r2c1);
        this->setElement(1, 1, r2c2);
        this->setElement(1, 2, r2c3);
        this->setElement(2, 0, r3c1);
        this->setElement(2, 1, r3c2);
        this->setElement(2, 2, r3c3);
    }

    /*!
     * \brief Calculates the determinant of the matrix
     * \return
     */
    double determinant() const;

    /*!
     * \brief Calculate the inverse
     * \param invertible
     * \return
     */
    CMatrix3x3 inverse(bool &invertible) const;

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
     * \param otherMatrix
     * \return
     */
    CMatrix3x3 operator *(const CMatrix3x3 &otherMatrix) const
    {
        CMatrix3x3 m(*this);
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
     * \brief Multiply with factor
     * \param factor
     * \return
     */
    CMatrix3x3 operator *(double factor) const
    {
        CMatrix3x3 m(*this);
        m *= factor;
        return m;
    }

    /*!
     * \brief Multiply with factor
     * \param factor
     * \return
     */
    CMatrix3x3 &operator *=(double factor)
    {
        CMatrixBase::operator *=(factor);
        return (*this);
    }

    /*!
     * \brief Transposed matrix
     * \return
     */
    CMatrix3x3 transposed() const
    {
        CMatrix3x3 m(0);
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
     * \param row
     * \return
     */
    CMatrix1x3 getRow(int row) const;

};

} // namespace

} // namespace

#endif // guard
