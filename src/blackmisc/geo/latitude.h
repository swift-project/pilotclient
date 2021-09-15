/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_LATITUDE_H
#define BLACKMISC_GEO_LATITUDE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/earthangle.h"

namespace BlackMisc::Geo
{
    //! Latitude
    class BLACKMISC_EXPORT CLatitude :
        public CEarthAngle<CLatitude>,
        public Mixin::MetaType<CLatitude>,
        public Mixin::String<CLatitude>,
        public Mixin::DBusOperators<CLatitude>,
        public Mixin::DataStreamOperators<CLatitude>,
        public Mixin::Index<CLatitude>
    {
    public:
        //! Base type
        using base_type = CEarthAngle<CLatitude>;

        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CLatitude)
        BLACKMISC_DECLARE_USING_MIXIN_STRING(CLatitude)
        BLACKMISC_DECLARE_USING_MIXIN_INDEX(CLatitude)

        //! To WGS84 string
        QString toWgs84(int fractionalDigits = 3) const
        {
            return CEarthAngle<CLatitude>::toWgs84('N', 'S', fractionalDigits);
        }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const
        {
            QString s(CEarthAngle::convertToQString(i18n));
            if (!this->isZeroEpsilonConsidered())
            {
                s.append(this->isNegativeWithEpsilonConsidered() ? " S" : " N");
            }
            return s;
        }

        //! Default constructor
        CLatitude() = default;

        //! Constructor
        explicit CLatitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

        //! Init by double value
        //! \remark Latitude measurements range from 0° to (+/–)90°
        CLatitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Geo::CLatitude)

#endif // guard
