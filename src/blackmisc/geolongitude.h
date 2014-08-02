/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEOLONGITUDE_H
#define BLACKMISC_GEOLONGITUDE_H

#include "blackmisc/geoearthangle.h"

namespace BlackMisc
{
    namespace Geo
    {

        //! Longitude
        class CLongitude : public CEarthAngle<CLongitude>
        {
        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const
            {
                QString s(CEarthAngle::convertToQString(i18n));
                if (!this->isZeroEpsilonConsidered())
                    s.append(this->isNegativeWithEpsilonConsidered() ? " W" : " E");
                return s;
            }

        public:
            //! Default constructor
            CLongitude() : CEarthAngle() {}

            //! Constructor
            explicit CLongitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

            //! Init by double value
            CLongitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Virtual destructor
            virtual ~CLongitude() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CLongitude)

#endif // guard
