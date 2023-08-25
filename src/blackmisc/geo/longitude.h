// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_GEO_LONGITUDE_H
#define BLACKMISC_GEO_LONGITUDE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/earthangle.h"

namespace BlackMisc::Geo
{
    //! Longitude
    class BLACKMISC_EXPORT CLongitude :
        public CEarthAngle<CLongitude>,
        public Mixin::MetaType<CLongitude>,
        public Mixin::String<CLongitude>,
        public Mixin::DBusOperators<CLongitude>,
        public Mixin::DataStreamOperators<CLatitude>,
        public Mixin::Index<CLongitude>
    {
    public:
        //! Base type
        using base_type = CEarthAngle<CLongitude>;

        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CLongitude)
        BLACKMISC_DECLARE_USING_MIXIN_STRING(CLongitude)
        BLACKMISC_DECLARE_USING_MIXIN_INDEX(CLongitude)

        //! To WGS84 string
        QString toWgs84(int withFragmentSecDigits = 3) const
        {
            return CEarthAngle<CLongitude>::toWgs84('E', 'W', withFragmentSecDigits);
        }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const
        {
            QString s(CEarthAngle::convertToQString(i18n));
            if (!this->isZeroEpsilonConsidered())
            {
                s.append(this->isNegativeWithEpsilonConsidered() ? " W" : " E");
            }
            return s;
        }

        //! Default constructor
        CLongitude() = default;

        //! Constructor
        explicit CLongitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

        //! Init by double value
        //! Longitude measurements range from 0° to (+/–)180°.
        CLongitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Geo::CLongitude)

#endif // guard
