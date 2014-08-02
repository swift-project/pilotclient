/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQANGLE_H
#define BLACKMISC_PQANGLE_H

#include "blackmisc/pqphysicalquantity.h"
#include "blackmisc/mathematics.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        //! Physical unit angle (radians, degrees)
        class CAngle : public CPhysicalQuantity<CAngleUnit, CAngle>
        {
        public:
            //! Default constructor
            CAngle() : CPhysicalQuantity(0, CAngleUnit::defaultUnit()) {}

            //! Init by double value
            CAngle(double value, const CAngleUnit &unit): CPhysicalQuantity(value, unit) {}

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CAngle(const QString &unitString) : CPhysicalQuantity(unitString) {}

            /*!
             * \brief Init as sexagesimal degrees, minutes, seconds
             * The sign of all parameters must be the same, either all positive or all negative.
             * \param degrees
             * \param minutes
             * \param seconds
             */
            CAngle(int degrees, int minutes, double seconds) :
                CPhysicalQuantity(
                    degrees + minutes / 100.0 + seconds / 10000.0,
                    CAngleUnit::sexagesimalDeg()) {}

            /*!
             * \brief Init as sexagesimal degrees, minutes
             * The sign of both parameters must be the same, either both positive or both negative.
             * \param degrees
             * \param minutes
             */
            CAngle(int degrees, double minutes) :
                CPhysicalQuantity(
                    degrees + minutes / 100.0,
                    CAngleUnit::sexagesimalDegMin()) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::toQVariant
            virtual BlackMisc::CIcon toIcon() const override;

            //! Virtual destructor
            virtual ~CAngle() {}

            //! Value as factor of PI (e.g. 0.5PI)
            double piFactor() const;

            //! PI as convenience method
            static const double &PI();

        private:

        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAngle)

#endif // guard
