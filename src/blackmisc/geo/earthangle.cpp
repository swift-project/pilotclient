/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/geo/earthangle.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"

#include <QRegularExpression>
#include <Qt>
#include <QtGlobal>
#include <QtMath>

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Geo
    {
        template <class LATorLON>
        CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator +=(const CEarthAngle &latOrLon)
        {
            this->PhysicalQuantities::CAngle::operator +=(latOrLon);
            return *this;
        }

        template <class LATorLON>
        CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator -=(const CEarthAngle &latOrLon)
        {
            this->PhysicalQuantities::CAngle::operator -=(latOrLon);
            return *this;
        }

        template <class LATorLON>
        CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator *=(double multiply)
        {
            this->PhysicalQuantities::CAngle::operator *=(multiply);
            return *this;
        }

        template <class LATorLON>
        LATorLON CEarthAngle<LATorLON>::operator +(const CEarthAngle &latOrLon) const
        {
            LATorLON l(*this);
            l += latOrLon;
            return l;
        }

        template <class LATorLON>
        LATorLON CEarthAngle<LATorLON>::operator -(const CEarthAngle &latOrLon) const
        {
            LATorLON l(*this);
            l -= latOrLon;
            return l;
        }

        template <class LATorLON>
        LATorLON CEarthAngle<LATorLON>::operator *(double multiply) const
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

            PhysicalQuantities::CAngle a(deg, min, sec);
            return LATorLON(a);
        }

        template <class LATorLON>
        CEarthAngle<LATorLON>::CEarthAngle()
            : PhysicalQuantities::CAngle(0.0, BlackMisc::PhysicalQuantities::CAngleUnit::deg())
        { }

        template <class LATorLON>
        CEarthAngle<LATorLON>::CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit)
            : PhysicalQuantities::CAngle(value, unit)
        { }

        template <class LATorLON>
        CEarthAngle<LATorLON>::CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle)
            : PhysicalQuantities::CAngle(angle)
        { }

        template <class LATorLON>
        QString CEarthAngle<LATorLON>::convertToQString(bool i18n) const
        {
            return this->valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CAngleUnit::deg(), 6, i18n);
        }

        template <class LATorLON>
        LATorLON const *CEarthAngle<LATorLON>::derived() const { return static_cast<LATorLON const *>(this); }

        template <class LATorLON>
        LATorLON *CEarthAngle<LATorLON>::derived() { return static_cast<LATorLON *>(this); }

        template <class LATorLON>
        CIcons::IconIndex CEarthAngle<LATorLON>::toIcon() const
        {
            return CIcons::GeoPosition;
        }

        template<class LATorLON>
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
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CEarthAngle<CLatitude>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CEarthAngle<CLongitude>;
        //! \endcond

    } // namespace
} // namespace
