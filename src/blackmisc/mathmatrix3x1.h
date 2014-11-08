/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_POSMATRIX3X1_H
#define BLACKMISC_POSMATRIX3X1_H

#include "blackmisc/mathmatrixbase.h"
#include "blackmisc/mathvector3d.h"

namespace BlackMisc
{
    namespace Math
    {

        class CMatrix3x3;

        //! Matrix 3x1
        class CMatrix3x1 : public CValueObjectStdTuple<CMatrix3x1, CMatrixBase<CMatrix3x1, 3, 1>>
        {
            friend class CMatrix3x3;

        public:
            //! Constructor
            CMatrix3x1() = default;

            //! Constructor
            CMatrix3x1(double r1, double r2, double r3)
            {
                this->m_matrix(0, 0) = r1;
                this->m_matrix(1, 0) = r2;
                this->m_matrix(2, 0) = r3;
            }

            //! Init by fill value
            explicit CMatrix3x1(double fillValue) : CValueObjectStdTuple(fillValue) {}

            //! Convert to vector
            CVector3D toVector3D() const
            {
                return CVector3D(this->getElement(0, 0), this->getElement(1, 0), this->getElement(2, 0));
            }

            //! Convert from vector
            void fromVector3D(const CVector3D &vector)
            {
                this->m_matrix(0, 0) = vector.i();
                this->m_matrix(1, 0) = vector.j();
                this->m_matrix(2, 0) = vector.k();
            }
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::Math::CMatrix3x1)

#endif // guard
