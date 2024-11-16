// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_GEO_LONGITUDE_H
#define SWIFT_MISC_GEO_LONGITUDE_H

#include "misc/geo/earthangle.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::geo
{
    //! Longitude
    class SWIFT_MISC_EXPORT CLongitude :
        public CEarthAngle<CLongitude>,
        public mixin::MetaType<CLongitude>,
        public mixin::String<CLongitude>,
        public mixin::DBusOperators<CLongitude>,
        public mixin::DataStreamOperators<CLatitude>,
        public mixin::Index<CLongitude>
    {
    public:
        //! Base type
        using base_type = CEarthAngle<CLongitude>;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CLongitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_STRING(CLongitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(CLongitude)

        //! To WGS84 string
        QString toWgs84(int withFragmentSecDigits = 3) const
        {
            return CEarthAngle<CLongitude>::toWgs84('E', 'W', withFragmentSecDigits);
        }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const
        {
            QString s(CEarthAngle::convertToQString(i18n));
            if (!this->isZeroEpsilonConsidered()) { s.append(this->isNegativeWithEpsilonConsidered() ? " W" : " E"); }
            return s;
        }

        //! Default constructor
        CLongitude() = default;

        //! Constructor
        explicit CLongitude(const swift::misc::physical_quantities::CAngle &angle) : CEarthAngle(angle) {}

        //! Init by double value
        //! Longitude measurements range from 0° to (+/–)180°.
        CLongitude(double value, const swift::misc::physical_quantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}
    };
} // namespace swift::misc::geo

Q_DECLARE_METATYPE(swift::misc::geo::CLongitude)

#endif // guard
