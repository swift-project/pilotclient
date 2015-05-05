/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_LONGITUDE_H
#define BLACKMISC_GEO_LONGITUDE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/earthangle.h"

namespace BlackMisc
{

    //! \private
    template <> struct CValueObjectPolicy<Geo::CLongitude> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::Default;
    };

    namespace Geo
    {

        //! Longitude
        class BLACKMISC_EXPORT CLongitude : public CValueObject<CLongitude, CEarthAngle<CLongitude>>
        {
        public:
            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const
            {
                QString s(CEarthAngle::convertToQString(i18n));
                if (!this->isZeroEpsilonConsidered())
                    s.append(this->isNegativeWithEpsilonConsidered() ? " W" : " E");
                return s;
            }

            //! Default constructor
            CLongitude() = default;

            //! Constructor
            explicit CLongitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CValueObject(angle) {}

            //! Init by double value
            CLongitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CValueObject(value, unit) {}
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::Geo::CLongitude)

#endif // guard
