/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_KMLUTILS_H
#define BLACKMISC_GEO_KMLUTILS_H

#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Geo
{
    /*!
     * KML utils
     */
    class BLACKMISC_EXPORT CKmlUtils
    {
    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! KML settings
        struct KMLSettings
        {
            //! Ctor
            KMLSettings(bool withAltitude, bool extrude) :
                withAltitude(withAltitude), extrude(extrude)
            { }

            //! Setting members
            //! @{
            bool withAltitude = true;
            bool extrude      = false;
            bool tessellate   = true;
            QString altitudeMode = "absolute";
            //! @}
        };

        //! Wrap as KML document
        static QString wrapAsKmlDocument(const QString &content);

        //! Wrap as KML coordinates
        static QString wrapAsKmlCoordinates(const QString &content);

        //! As raw coordinates
        static QString asRawCoordinates(const Geo::ICoordinateGeodetic &coordinate, bool withAltitude);

        //! As KML point
        static QString asPoint(const Geo::ICoordinateGeodetic &coordinate, const KMLSettings &settings);

        //! As KML placemark
        static QString asPlacemark(const QString &name, const QString &description, const Geo::ICoordinateGeodetic &coordinate, const KMLSettings &settings);

        //! As KML line
        static QString asLineString(const QString &coordinatesRaw, const KMLSettings &settings);
    };
} // ns

#endif // guard
