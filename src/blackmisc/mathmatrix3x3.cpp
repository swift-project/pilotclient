/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/mathmatrix3x3.h"

namespace BlackMisc
{
namespace Math
{

/*
 * Determinant
 */
double CMatrix3x3::determinant() const
{
    double determinant =
        this->m_matrix(0, 0) * this->m_matrix(1, 1) * this->m_matrix(2, 2) +
        this->m_matrix(0, 1) * this->m_matrix(1, 2) * this->m_matrix(2, 0) +
        this->m_matrix(0, 2) * this->m_matrix(1, 0) * this->m_matrix(2, 1) -
        this->m_matrix(0, 1) * this->m_matrix(1, 0) * this->m_matrix(2, 2) -
        this->m_matrix(0, 2) * this->m_matrix(1, 1) * this->m_matrix(2, 0) -
        this->m_matrix(0, 0) * this->m_matrix(1, 2) * this->m_matrix(2, 1);

    return determinant;
}

/*
 * Inverse
 */
CMatrix3x3 CMatrix3x3::inverse(bool &invertible) const
{
    CMatrix3x3 inverse;
    double det;
    if (this->allValuesEqual() || (det = determinant()) == 0)
    {
        invertible = false;
        inverse.setZero();
        return inverse;
    }
    double invdet = 1.0 / det;

    inverse.m_matrix(0, 0) = (this->m_matrix(1, 1) * this->m_matrix(2, 2) - this->m_matrix(1, 2) * this->m_matrix(2, 1)) * invdet;
    inverse.m_matrix(0, 1) = (- this->m_matrix(0, 1) * this->m_matrix(2, 2) + this->m_matrix(0, 2) * this->m_matrix(2, 1)) * invdet;
    inverse.m_matrix(0, 2) = (this->m_matrix(0, 1) * this->m_matrix(1, 2) - this->m_matrix(0, 2) * this->m_matrix(1, 1)) * invdet;
    inverse.m_matrix(1, 0) = (- this->m_matrix(1, 0) * this->m_matrix(2, 2) + this->m_matrix(1, 2) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(1, 1) = (this->m_matrix(0, 0) * this->m_matrix(2, 2) - this->m_matrix(0, 2) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(1, 2) = (- this->m_matrix(0, 0) * this->m_matrix(1, 2) + this->m_matrix(0, 2) * this->m_matrix(1, 0)) * invdet;
    inverse.m_matrix(2, 0) = (this->m_matrix(1, 0) * this->m_matrix(2, 1) - this->m_matrix(1, 1) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(2, 1) = (- this->m_matrix(0, 0) * this->m_matrix(2, 1) + this->m_matrix(0, 1) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(2, 2) = (this->m_matrix(0, 0) * this->m_matrix(1, 1) - this->m_matrix(0, 1) * this->m_matrix(1, 0)) * invdet;

    invertible = true;
    return inverse;
}

/*
 * Get a row
 */
CMatrix1x3 CMatrix3x3::getRow(int row) const
{
    bool valid = row >= 0 && row <= 3;
    Q_ASSERT_X(valid, "getRow", "invalid row");
    if (!valid) throw new std::range_error("invalid row");
    return CMatrix1x3(this->getElement(row, 0), this->getElement(row, 1), this->getElement(row, 2));
}

/*
 * Get a column
 */
CMatrix3x1 CMatrix3x3::getColumn(int column) const
{
    bool valid = column >= 0 && column <= 3;
    Q_ASSERT_X(valid, "getColumn", "invalid column");
    if (!valid) throw new std::range_error("invalid column");
    return CMatrix3x1(this->getElement(0, column), this->getElement(1, column), this->getElement(2, column));
}

} // namespace

} // namespace
