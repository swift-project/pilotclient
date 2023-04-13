/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_EARTHANGLE_H
#define BLACKMISC_GEO_EARTHANGLE_H

#include "blackmisc/pq/angle.h"

namespace BlackMisc::Geo
{
    class CLatitude;
    class CLongitude;

    /*!
     * Base class for latitude / longitude
     */
    template <class LATorLON>
    class CEarthAngle : public PhysicalQuantities::CAngle
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

        //! \copydoc BlackMisc::Mixin::Icon::toIcon
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
        CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit);

        //! Init by CAngle value
        CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle);

        //! To WGS84 string
        QString toWgs84(const QChar pos, const QChar neg, int fractionalDigits = 3) const;

    public:
        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        //! Easy access to derived class (CRTP template parameter)
        LATorLON const *derived() const;

        //! Easy access to derived class (CRTP template parameter)
        LATorLON *derived();
    };

    //! \cond PRIVATE
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CEarthAngle<CLatitude>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CEarthAngle<CLongitude>;
    //! \endcond
} // ns

#endif // guard
