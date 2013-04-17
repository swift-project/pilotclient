/*  Copyright (C) 2013 VATSIM Community / authors
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
qreal CMatrix3x3::determinant() const
{
    qreal determinant =
        this->m_matrix(0, 0) * this->m_matrix(1, 1) * this->m_matrix(2, 2) +
        this->m_matrix(0, 1) * this->m_matrix(1, 2) * this->m_matrix(2, 0) +
        this->m_matrix(0, 2) * this->m_matrix(1, 0) * this->m_matrix(2, 1) -
        this->m_matrix(0, 1) * this->m_matrix(1, 0) * this->m_matrix(2, 2) -
        this->m_matrix(0, 2) * this->m_matrix(1, 1) * this->m_matrix(2, 0) -
        this->m_matrix(0, 0) * this->m_matrix(1, 2) * this->m_matrix(2, 1);

    return determinant;
}

/*
 * Determinant
 */
CMatrix3x3 CMatrix3x3::inverse() const
{
    CMatrix3x3 inverse;
    qreal det = determinant();

    // should we throw an assert / error here?
    if (det == 0) return inverse;

    qreal invdet = 1.0 / det;

    inverse.m_matrix(0, 0) = (this->m_matrix(1, 1) * this->m_matrix(2, 2) - this->m_matrix(1, 2) * this->m_matrix(2, 1)) * invdet;
    inverse.m_matrix(0, 1) = (- this->m_matrix(0, 1) * this->m_matrix(2, 2) + this->m_matrix(0, 2) * this->m_matrix(2, 1)) * invdet;
    inverse.m_matrix(0, 2) = (this->m_matrix(0, 1) * this->m_matrix(1, 2) - this->m_matrix(0, 2) * this->m_matrix(1, 1)) * invdet;
    inverse.m_matrix(1, 0) = (- this->m_matrix(1, 0) * this->m_matrix(2, 2) + this->m_matrix(1, 2) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(1, 1) = (this->m_matrix(0, 0) * this->m_matrix(2, 2) - this->m_matrix(0, 2) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(1, 2) = (- this->m_matrix(0, 0) * this->m_matrix(1, 2) + this->m_matrix(0, 2) * this->m_matrix(1, 0)) * invdet;
    inverse.m_matrix(2, 0) = (this->m_matrix(1, 0) * this->m_matrix(2, 1) - this->m_matrix(1, 1) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(2, 1) = (- this->m_matrix(0, 0) * this->m_matrix(2, 1) + this->m_matrix(0, 1) * this->m_matrix(2, 0)) * invdet;
    inverse.m_matrix(2, 2) = (this->m_matrix(0, 0) * this->m_matrix(1, 1) - this->m_matrix(0, 1) * this->m_matrix(1, 0)) * invdet;

    return inverse;
}

} // namespace

} // namespace
