// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_GEO_LATITUDE_H
#define SWIFT_MISC_GEO_LATITUDE_H

#include "misc/geo/earthangle.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::geo
{
    //! Latitude
    class SWIFT_MISC_EXPORT CLatitude :
        public CEarthAngle<CLatitude>,
        public mixin::MetaType<CLatitude>,
        public mixin::String<CLatitude>,
        public mixin::DBusOperators<CLatitude>,
        public mixin::DataStreamOperators<CLatitude>,
        public mixin::Index<CLatitude>
    {
    public:
        //! Base type
        using base_type = CEarthAngle<CLatitude>;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CLatitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_STRING(CLatitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(CLatitude)

        //! To WGS84 string
        QString toWgs84(int fractionalDigits = 3) const
        {
            return CEarthAngle<CLatitude>::toWgs84('N', 'S', fractionalDigits);
        }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const
        {
            QString s(CEarthAngle::convertToQString(i18n));
            if (!this->isZeroEpsilonConsidered()) { s.append(this->isNegativeWithEpsilonConsidered() ? " S" : " N"); }
            return s;
        }

        //! Default constructor
        CLatitude() = default;

        //! Constructor
        explicit CLatitude(const swift::misc::physical_quantities::CAngle &angle) : CEarthAngle(angle) {}

        //! Init by double value
        //! \remark Latitude measurements range from 0° to (+/–)90°
        CLatitude(double value, const swift::misc::physical_quantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}
    };
} // namespace swift::misc::geo

Q_DECLARE_METATYPE(swift::misc::geo::CLatitude)

#endif // SWIFT_MISC_GEO_LATITUDE_H
