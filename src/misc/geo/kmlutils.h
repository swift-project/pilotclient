// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_GEO_KMLUTILS_H
#define SWIFT_MISC_GEO_KMLUTILS_H

#include "misc/geo/coordinategeodetic.h"
#include "misc/logcategories.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::geo
{
    /*!
     * KML utils
     */
    class SWIFT_MISC_EXPORT CKmlUtils
    {
    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! KML settings
        struct KMLSettings
        {
            //! Ctor
            KMLSettings(bool withAltitude, bool extrude) : withAltitude(withAltitude), extrude(extrude)
            {}

            //! @{
            //! Setting members
            bool withAltitude = true;
            bool extrude = false;
            bool tessellate = true;
            QString altitudeMode = "absolute";
            //! @}
        };

        //! Wrap as KML document
        static QString wrapAsKmlDocument(const QString &content);

        //! Wrap as KML coordinates
        static QString wrapAsKmlCoordinates(const QString &content);

        //! As raw coordinates
        static QString asRawCoordinates(const geo::ICoordinateGeodetic &coordinate, bool withAltitude);

        //! As KML point
        static QString asPoint(const geo::ICoordinateGeodetic &coordinate, const KMLSettings &settings);

        //! As KML placemark
        static QString asPlacemark(const QString &name, const QString &description, const geo::ICoordinateGeodetic &coordinate, const KMLSettings &settings);

        //! As KML line
        static QString asLineString(const QString &coordinatesRaw, const KMLSettings &settings);
    };
} // namespace swift::misc::geo

#endif // guard
