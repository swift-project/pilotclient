// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_GEO_EARTHANGLE_H
#define SWIFT_MISC_GEO_EARTHANGLE_H

#include "misc/pq/angle.h"

namespace swift::misc::geo
{
    class CLatitude;
    class CLongitude;

    /*!
     * Base class for latitude / longitude
     */
    template <class LATorLON>
    class CEarthAngle : public physical_quantities::CAngle
    {
    public:
        //! Plus operator +=
        CEarthAngle &operator+=(const CEarthAngle &latOrLon);

        //! Minus operator-=
        CEarthAngle &operator-=(const CEarthAngle &latOrLon);

        //! Multiply operator *=
        CEarthAngle &operator*=(double multiply);

        //! Plus operator +
        LATorLON operator+(const CEarthAngle &latOrLon) const;

        //! Minus operator -
        LATorLON operator-(const CEarthAngle &latOrLon) const;

        //! Multiply operator *
        LATorLON operator*(double multiply) const;

        //! \copydoc swift::misc::mixin::Icon::toIcon
        CIcons::IconIndex toIcon() const;

        /*!
         * Latitude / Longitude from a WGS string such as
         * \param wgsCoordinate 50° 2′ 0″ N / 8° 34′ 14″ E
         * \return
         */
        static LATorLON fromWgs84(const QString &wgsCoordinate);

    protected:
        //! Default constructor
        CEarthAngle();

        //! Init by double value
        CEarthAngle(double value, const swift::misc::physical_quantities::CAngleUnit &unit);

        //! Init by CAngle value
        CEarthAngle(const swift::misc::physical_quantities::CAngle &angle);

        //! To WGS84 string
        QString toWgs84(const QChar pos, const QChar neg, int fractionalDigits = 3) const;

    public:
        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        //! Easy access to derived class (CRTP template parameter)
        LATorLON const *derived() const;

        //! Easy access to derived class (CRTP template parameter)
        LATorLON *derived();
    };

    //! \cond PRIVATE
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CEarthAngle<CLatitude>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CEarthAngle<CLongitude>;
    //! \endcond
} // namespace swift::misc::geo

#endif // guard
