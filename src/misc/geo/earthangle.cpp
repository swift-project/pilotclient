// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/geo/earthangle.h"
#include "misc/pq/units.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"

#include <QRegularExpression>
#include <Qt>
#include <QtGlobal>

using namespace swift::misc::physical_quantities;

namespace swift::misc::geo
{
    template <class LATorLON>
    CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator+=(const CEarthAngle &latOrLon)
    {
        this->physical_quantities::CAngle::operator+=(latOrLon);
        return *this;
    }

    template <class LATorLON>
    CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator-=(const CEarthAngle &latOrLon)
    {
        this->physical_quantities::CAngle::operator-=(latOrLon);
        return *this;
    }

    template <class LATorLON>
    CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator*=(double multiply)
    {
        this->physical_quantities::CAngle::operator*=(multiply);
        return *this;
    }

    template <class LATorLON>
    LATorLON CEarthAngle<LATorLON>::operator+(const CEarthAngle &latOrLon) const
    {
        LATorLON l(*this);
        l += latOrLon;
        return l;
    }

    template <class LATorLON>
    LATorLON CEarthAngle<LATorLON>::operator-(const CEarthAngle &latOrLon) const
    {
        LATorLON l(*this);
        l -= latOrLon;
        return l;
    }

    template <class LATorLON>
    LATorLON CEarthAngle<LATorLON>::operator*(double multiply) const
    {
        LATorLON l(*this);
        l *= multiply;
        return l;
    }

    template <class LATorLON>
    LATorLON CEarthAngle<LATorLON>::fromWgs84(const QString &wgsCoordinate)
    {
        const QString wgs = wgsCoordinate.simplified().trimmed();
        if (wgs.isEmpty()) { return LATorLON(); }

        // support for 5deg, 1.2rad
        if (CAngleUnit::deg().endsStringWithNameOrSymbol(wgs) || CAngleUnit::rad().endsStringWithNameOrSymbol(wgs))
        {
            LATorLON latOrLon;
            latOrLon.parseFromString(wgs);
            return latOrLon;
        }

        // number only -> parsed as degrees
        bool isDouble;
        const double valueDegrees = wgs.toDouble(&isDouble);
        if (isDouble)
        {
            CAngle a(valueDegrees, CAngleUnit::deg());
            return LATorLON(a);
        }

        // http://www.regular-expressions.info/floatingpoint.html
        thread_local const QRegularExpression rx("[+-]?\\d+(?:\\.\\d+)?");
        int deg = 0;
        int min = 0;
        double sec = 0.0;
        int c = 0;
        QRegularExpressionMatchIterator i = rx.globalMatch(wgs);
        while (i.hasNext() && c < 3)
        {
            const QRegularExpressionMatch match = i.next();
            bool ok;
            if (match.hasMatch())
            {
                const QString cap = match.captured(0);
                switch (c++)
                {
                case 0:
                    deg = cap.toInt(&ok);
                    break;
                case 1:
                    min = cap.toInt(&ok);
                    break;
                case 2:
                    sec = cap.toDouble(&ok);
                    break;
                default:
                    break;
                }
            }
            Q_UNUSED(ok); // ok for debugging purposes
        }

        if (wgs.contains('S', Qt::CaseInsensitive) || wgs.contains('W', Qt::CaseInsensitive))
        {
            deg *= -1;
            min *= -1;
            sec *= -1;
        }

        physical_quantities::CAngle a(deg, min, sec);
        return LATorLON(a);
    }

    template <class LATorLON>
    CEarthAngle<LATorLON>::CEarthAngle()
        : physical_quantities::CAngle(0.0, swift::misc::physical_quantities::CAngleUnit::deg())
    {}

    template <class LATorLON>
    CEarthAngle<LATorLON>::CEarthAngle(double value, const swift::misc::physical_quantities::CAngleUnit &unit)
        : physical_quantities::CAngle(value, unit)
    {}

    template <class LATorLON>
    CEarthAngle<LATorLON>::CEarthAngle(const swift::misc::physical_quantities::CAngle &angle)
        : physical_quantities::CAngle(angle)
    {}

    template <class LATorLON>
    QString CEarthAngle<LATorLON>::convertToQString(bool i18n) const
    {
        return this->valueRoundedWithUnit(swift::misc::physical_quantities::CAngleUnit::deg(), 6, i18n);
    }

    template <class LATorLON>
    LATorLON const *CEarthAngle<LATorLON>::derived() const
    {
        return static_cast<LATorLON const *>(this);
    }

    template <class LATorLON>
    LATorLON *CEarthAngle<LATorLON>::derived()
    {
        return static_cast<LATorLON *>(this);
    }

    template <class LATorLON>
    CIcons::IconIndex CEarthAngle<LATorLON>::toIcon() const
    {
        return CIcons::GeoPosition;
    }

    template <class LATorLON>
    QString CEarthAngle<LATorLON>::toWgs84(const QChar pos, const QChar neg, int fractionalDigits) const
    {
        const CAngle::DegMinSecFractionalSec v = this->asSexagesimalDegMinSec(true);
        const QChar pn = v.sign < 0 ? neg : pos;

        static const QString vs("%1° %2' %3\" %4");
        if (fractionalDigits < 1) { return vs.arg(v.deg).arg(v.min).arg(v.sec).arg(pn); }

        static const QString vsf("%1° %2' %3.%4\" %5");
        return vsf.arg(v.deg).arg(v.min).arg(v.sec).arg(v.fractionalSecAsString(fractionalDigits)).arg(pn);
    }

    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    //! \cond PRIVATE
    template class CEarthAngle<CLatitude>;
    template class CEarthAngle<CLongitude>;
    //! \endcond

} // namespace
