/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEOLATITUDE_H
#define BLACKMISC_GEOLATITUDE_H

#include <QtCore/qmath.h>
#include "blackmisc/geoearthangle.h"

namespace BlackMisc
{
    namespace Geo
    {

        //! Latitude
        class CLatitude : public CEarthAngle<CLatitude>
        {
        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const
            {
                QString s(CEarthAngle::convertToQString(i18n));
                if (!this->isZeroEpsilonConsidered())
                {
                    s.append(this->isNegativeWithEpsilonConsidered() ? " S" : " N");
                }
                return s;
            }

        public:
            //! Default constructor
            CLatitude() : CEarthAngle() {}

            //! Constructor
            explicit CLatitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

            //! Init by double value
            CLatitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Virtual destructor
            virtual ~CLatitude() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CLatitude)

#endif // guard
