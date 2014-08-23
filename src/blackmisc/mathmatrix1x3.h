/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_POSMATRIX1x3_H
#define BLACKMISC_POSMATRIX1x3_H

#include "blackmisc/mathmatrixbase.h"
#include "blackmisc/mathvector3d.h"

namespace BlackMisc
{
    namespace Math
    {

        //! Matrix 1x3
        class CMatrix1x3 : public CMatrixBase<CMatrix1x3, 1, 3>
        {
        public:

            //! Constructor
            CMatrix1x3() : CMatrixBase() {}

            //! Copy constructor
            CMatrix1x3(const CMatrix1x3 &other) : CMatrixBase(other) {}

            //! Init by fill value
            explicit CMatrix1x3(double fillValue) : CMatrixBase(fillValue) {}

            //! Constructor
            CMatrix1x3(double c1, double c2, double c3) : CMatrixBase()
            {
                this->m_matrix(0, 0) = c1;
                this->m_matrix(0, 1) = c2;
                this->m_matrix(0, 2) = c3;
            }

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Math::CMatrix1x3)

#endif // guard
