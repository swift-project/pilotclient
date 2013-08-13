/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_POSMATRIX1x3_H
#define BLACKMISC_POSMATRIX1x3_H

#include "blackmisc/mathmatrixbase.h"
#include "blackmisc/mathvector3d.h"

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief 3D matrix
 */
class CMatrix1x3 : public CMatrixBase<CMatrix1x3, 1, 3>
{
public:
    /*!
     * \brief Constructor
     */
    CMatrix1x3() : CMatrixBase() {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CMatrix1x3(const CMatrix1x3 &other) : CMatrixBase(other) {}

    /*!
     * \brief Init by fill value
     * \param fillValue
     */
    explicit CMatrix1x3(double fillValue) : CMatrixBase(fillValue) {}

    /*!
     * \brief Constructor
     * \param c1
     * \param c2
     * \param c3
     */
    CMatrix1x3(double c1, double c2, double c3) : CMatrixBase()
    {
        this->m_matrix(0, 0) = c1;
        this->m_matrix(0, 1) = c2;
        this->m_matrix(0, 2) = c3;
    }
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Math::CMatrix1x3)

#endif // guard
