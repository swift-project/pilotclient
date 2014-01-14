/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/geoearthangle.h"
#include "blackmisc/geolatitude.h"
#include "blackmisc/geolongitude.h"

namespace BlackMisc
{
    namespace Geo
    {

        /*
         * Register metadata
         */
        template <class LATorLON> void CEarthAngle<LATorLON>::registerMetadata()
        {
            qRegisterMetaType<LATorLON>();
            qDBusRegisterMetaType<LATorLON>();
        }

        /*
         * Latitude or Longitude from a WGS string
         */
        template <class LATorLON> LATorLON CEarthAngle<LATorLON>::fromWgs84(const QString &wgsCoordinate)
        {
            // http://www.regular-expressions.info/floatingpoint.html
            QRegExp rx("([-+]?[0-9]*\\.?[0-9]+)");
            qint32 deg = 0;
            qint32 min = 0;
            double sec = 0.0;
            double secFragment = 0.0;
            int fragmentLength = 0;
            int c = 0;
            int pos = 0;
            while ((pos = rx.indexIn(wgsCoordinate, pos)) != -1)
            {
                QString cap = rx.cap(1);
                pos += rx.matchedLength();
                switch (c++)
                {
                case 0:
                    deg = cap.toInt();
                    break;
                case 1:
                    min = cap.toInt();
                    break;
                case 2:
                    sec = cap.toDouble();
                    break;
                case 3:
                    secFragment = cap.toDouble();
                    fragmentLength = cap.length();
                    break;
                default:
                    break;
                }
            }
            if (fragmentLength > 0)
            {
                // we do have given ms
                sec += secFragment / qPow(10, fragmentLength);
            }

            if (wgsCoordinate.contains('S', Qt::CaseInsensitive) ||
                    wgsCoordinate.contains('W', Qt::CaseInsensitive)) deg *= -1;

            CAngle a(deg, min, sec);
            return LATorLON(a);
        }

        /*
         * metaTypeId
         */
        template <class LATorLON> int CEarthAngle<LATorLON>::getMetaTypeId() const
        {
            return qMetaTypeId<LATorLON>();
        }

        /*
         * is a
         */
        template <class LATorLON> bool CEarthAngle<LATorLON>::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<LATorLON>()) { return true; }

            return this->CAngle::isA(metaTypeId);
        }

        /*
         * Compare
         */
        template <class LATorLON> int CEarthAngle<LATorLON>::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const LATorLON &>(otherBase);

            if (*this < other) { return -1; }
            else if (*this > other) { return 1; }
            else { return 0; }
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CEarthAngle<CLatitude>;
        template class CEarthAngle<CLongitude>;

    } // namespace
} // namespace
