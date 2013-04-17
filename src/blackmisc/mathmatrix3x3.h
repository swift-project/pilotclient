/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_POSMATRIX3X3_H
#define BLACKMISC_POSMATRIX3X3_H

#include "blackmisc/mathmatrixbase.h"

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief 3x1 matrix
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
    CMatrix3x3(qreal fillValue) : CMatrixBase(fillValue) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CMatrix3x3(const CMatrix3x3 &otherMatrix) : CMatrixBase(otherMatrix) {}

    /*!
     * \brief Calculates the determinant of the matrix
     * \return
     */
    qreal determinant() const;

    /*!
     * \brief Calculate the inverse
     * \return
     */
    CMatrix3x3 inverse() const;

};

} // namespace

} // namespace

#endif // BLACKMISC_POSMATRIX3X3_H
