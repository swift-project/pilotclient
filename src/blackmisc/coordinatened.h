/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_COORDINATENED_H
#define BLACKMISC_COORDINATENED_H

//! \file

#include "blackmisc/mathvector3d.h"
#include "blackmisc/mathmatrix3x3.h"
#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Geo
    {
        /*!
         * \brief North, East, Down
         */
        class CCoordinateNed : public CValueObject<CCoordinateNed, Math::CVector3DBase<CCoordinateNed>>
        {
        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override
            {
                Q_UNUSED(i18n)
                QString s = "NED: {N %1, E %2, D %3}";
                s = s.arg(QString::number(this->north(), 'f', 6)).
                    arg(QString::number(this->east(), 'f', 6)).
                    arg(QString::number(this->down(), 'f', 6));
                return s;
            }

        public:
            /*!
             * \brief Default constructor
             */
            CCoordinateNed() : m_hasReferencePosition(false) {}

            /*!
             * \brief Constructor with reference position
             */
            CCoordinateNed(const CCoordinateGeodetic &referencePosition) : m_referencePosition(referencePosition), m_hasReferencePosition(true) {}

            /*!
             * \brief Constructor by values
             */
            CCoordinateNed(const CCoordinateGeodetic &referencePosition, double north, double east, double down) : CValueObject(north, east, down), m_referencePosition(referencePosition), m_hasReferencePosition(true) {}

            /*!
             * \brief Constructor by values
             */
            CCoordinateNed(double north, double east, double down) : CValueObject(north, east, down), m_referencePosition(), m_hasReferencePosition(false) {}

            /*!
             * \brief Constructor by math vector
             */
            explicit CCoordinateNed(const BlackMisc::Math::CVector3D &vector) : CValueObject(vector.i(), vector.j(), vector.k()), m_referencePosition(), m_hasReferencePosition(false) {}

            /*!
             * \brief Constructor by math vector and reference position
             */
            CCoordinateNed(const CCoordinateGeodetic &referencePosition, const BlackMisc::Math::CVector3D &vector) : CValueObject(vector.i(), vector.j(), vector.k()), m_referencePosition(referencePosition), m_hasReferencePosition(true) {}

            /*!
             * \brief Corresponding reference position
             */
            CCoordinateGeodetic referencePosition() const
            {
                return this->m_referencePosition;
            }

            /*!
             * \brief Corresponding reference position
             */
            bool hasReferencePosition() const
            {
                return this->m_hasReferencePosition;
            }

            /*!
             * \brief North
             */
            double north() const
            {
                return this->m_i;
            }

            /*!
             * \brief East
             */
            double east() const
            {
                return this->m_j;
            }

            /*!
             * \brief Down
             */
            double down() const
            {
                return this->m_k;
            }

            /*!
             * \brief Set north
             */
            void setNorth(double north)
            {
                this->m_i = north;
            }

            /*!
             * \brief Set east
             */
            void setEast(double east)
            {
                this->m_j = east;
            }

            /*!
             * \brief Set down
             */
            void setDown(double down)
            {
                this->m_k = down;
            }

            /*!
             * \brief Corresponding reference position
             */
            void setReferencePosition(const CCoordinateGeodetic &referencePosition)
            {
                this->m_referencePosition = referencePosition;
                this->m_hasReferencePosition = true;
            }

            /*!
             * \brief Concrete implementation of a 3D vector
             */
            BlackMisc::Math::CVector3D toMathVector() const
            {
                return BlackMisc::Math::CVector3D(this->north(), this->east(), this->down());
            }

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CCoordinateNed)
            CCoordinateGeodetic m_referencePosition; //!< geodetic reference position
            bool m_hasReferencePosition; //!< valid reference position?
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Geo::CCoordinateNed, (o.m_referencePosition, o.m_hasReferencePosition))
Q_DECLARE_METATYPE(BlackMisc::Geo::CCoordinateNed)

#endif // guard
