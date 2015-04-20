/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_LATITUDE_H
#define BLACKMISC_GEO_LATITUDE_H

#include <QtCore/qmath.h>
#include "blackmisc/geo/earthangle.h"

namespace BlackMisc
{

    //! \private
    template <> struct CValueObjectPolicy<Geo::CLatitude> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::Default;
    };

    namespace Geo
    {

        //! Latitude
        class CLatitude : public CValueObject<CLatitude, CEarthAngle<CLatitude>>
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
            CLatitude() = default;

            //! Constructor
            explicit CLatitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CValueObject(angle) {}

            //! Init by double value
            CLatitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CValueObject(value, unit) {}
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::Geo::CLatitude)

#endif // guard
