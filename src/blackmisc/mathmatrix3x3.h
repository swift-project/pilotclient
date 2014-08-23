/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_POSMATRIX3X3_H
#define BLACKMISC_POSMATRIX3X3_H
#include "blackmisc/mathmatrix1x3.h"
#include "blackmisc/mathmatrix3x1.h"
#include <stdexcept>

namespace BlackMisc
{
    namespace Math
    {

        //! 3x3 matrix
        class CMatrix3x3 : public CMatrixBase<CMatrix3x3, 3, 3>
        {
        public:
            //! Constructor
            CMatrix3x3() : CMatrixBase() {}

            //! Copy constructor
            CMatrix3x3(const CMatrix3x3 &other) : CMatrixBase(other) {}

            //! Init by fill value
            explicit CMatrix3x3(double fillValue) : CMatrixBase(fillValue) {}

            //! Stupid, but handy constructor by single row column values
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

            //! Calculates the determinant of the matrix
            double determinant() const;

            //! Calculate the inverse and mark flag if successful
            CMatrix3x3 inverse(bool &o_isInvertible) const;

            //! Operator *=
            CMatrix3x3 &operator *=(const CMatrix3x3 &other)
            {
                this->m_matrix = this->m_matrix * other.m_matrix;
                return *this;
            }

            //! Operator *
            CMatrix3x3 operator *(const CMatrix3x3 &other) const
            {
                CMatrix3x3 m(*this);
                m *= other;
                return m;
            }

            //! Operator *
            CMatrix3x1 operator *(const CMatrix3x1 &other) const
            {
                CMatrix3x1 m;
                m.m_matrix = this->m_matrix * other.m_matrix;
                return m;
            }

            //! Multiply this matrix with vector
            CVector3D operator *(const CVector3D &vector) const
            {
                return ((*this) * CMatrix3x1(vector.toMatrix3x1())).toVector3D();
            }

            //! Multiply with factor
            CMatrix3x3 &operator *=(double factor)
            {
                this->CMatrixBase::operator *=(factor);
                return *this;
            }

            //! Multiply with factor
            CMatrix3x3 operator *(double factor) const
            {
                CMatrix3x3 m(*this);
                m *= factor;
                return m;
            }

            //! Transposed matrix
            CMatrix3x3 transposed() const
            {
                CMatrix3x3 m(0);
                m.m_matrix = this->m_matrix.transposed();
                return m;
            }

            //! Get column
            CMatrix3x1 getColumn(int column) const;

            //! Get row
            CMatrix1x3 getRow(int row) const;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Math::CMatrix3x3)

#endif // guard
