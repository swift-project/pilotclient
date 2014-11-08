/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_MATHVECTOR3D_H
#define BLACKMISC_MATHVECTOR3D_H

//! \file

#include "blackmisc/mathvector3dbase.h"

namespace BlackMisc
{
    namespace Math { class CVector3D; }

    //! \private
    template <> struct CValueObjectStdTuplePolicy<Math::CVector3D> : public CValueObjectStdTuplePolicy<>
    {
        using Compare = Policy::Compare::Own;
        using Hash = Policy::Hash::Own;
        using DBus = Policy::DBus::Own;
        using Json = Policy::Json::Own;
    };

    namespace Math
    {

        /*!
         * \brief Concrete vector implementation
         */
        class CVector3D : public CValueObjectStdTuple<CVector3D, CVector3DBase<CVector3D>>
        {
        public:
            /*!
             * \brief Default constructor
             */
            CVector3D() = default;

            /*!
             * \brief Constructor by value
             */
            CVector3D(double i, double j, double k) : CValueObjectStdTuple(i, j, k) {}

            /*!
             * \brief Constructor by value
             */
            explicit CVector3D(double value) : CValueObjectStdTuple(value) {}

            /*!
             * \brief i
             */
            double i() const
            {
                return this->m_i;
            }

            /*!
             * \brief j
             */
            double j() const
            {
                return this->m_j;
            }

            /*!
             * \brief k
             */
            double k() const
            {
                return this->m_k;
            }

            /*!
             * \brief Set i
             */
            void setI(double i)
            {
                this->m_i = i;
            }

            /*!
             * \brief Set j
             */
            void setJ(double j)
            {
                this->m_j = j;
            }

            /*!
             * \brief Set k
             */
            void setK(double k)
            {
                this->m_k = k;
            }
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::Math::CVector3D)

#endif // guard
