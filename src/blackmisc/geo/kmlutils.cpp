/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/geo/kmlutils.h"
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Geo
{
    const QStringList &CKmlUtils::getLogCategories()
    {
        static const QStringList cats { CLogCategories::interpolator() };
        return cats;
    }

    QString CKmlUtils::wrapAsKmlDocument(const QString &content)
    {
        return u"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
               u"<kml xmlns=\"http://www.opengis.net/kml/2.2\" "
               u"xmlns:gx=\"http://www.google.com/kml/ext/2.2\">\n"
               u"<Document>\n" %
               content %
               u"</Document>\n"
               u"</kml>\n";
    }

    QString CKmlUtils::wrapAsKmlCoordinates(const QString &content)
    {
        return u"<coordinates>" % content % u"</coordinates>";
    }

    QString CKmlUtils::asRawCoordinates(const ICoordinateGeodetic &coordinate, bool withAltitude)
    {
        return coordinate.longitude().valueRoundedAsString(CAngleUnit::deg(), 8) % u"," %
               coordinate.latitude().valueRoundedAsString(CAngleUnit::deg(), 8) %
               (withAltitude ? (u"," % coordinate.geodeticHeight().valueRoundedAsString(CLengthUnit::m())) : QString());
    }

    QString CKmlUtils::asPoint(const ICoordinateGeodetic &coordinate, const KMLSettings &settings)
    {
        return u"<Point><coordinates>" %
               CKmlUtils::asRawCoordinates(coordinate, settings.withAltitude) %
               u"</coordinates>" %
               (settings.extrude ? u"<extrude>1</extrude>" : u"") %
               (settings.withAltitude && !settings.altitudeMode.isEmpty() ? u"<altitudeMode>" % settings.altitudeMode % "</altitudeMode>" : QString()) %
               u"</Point>";
    }

    QString CKmlUtils::asPlacemark(
        const QString &name, const QString &description,
        const ICoordinateGeodetic &coordinate, const KMLSettings &settings)
    {
        return u"<Placemark>" %
               asPoint(coordinate, settings) %
               (name.isEmpty() ? QString() : u"<name>" % name.toHtmlEscaped() % u"</name>") %
               (description.isEmpty() ? QString() : u"<description>" % description.toHtmlEscaped() % u"</description>") %
               u"</Placemark>";
    }

    QString CKmlUtils::asLineString(const QString &coordinatesRaw, const CKmlUtils::KMLSettings &settings)
    {
        return u"<LineString>" %
               (settings.withAltitude && !settings.altitudeMode.isEmpty() ? u"<altitudeMode>" % settings.altitudeMode % "</altitudeMode>" : QString()) %
               (settings.tessellate ? u"<tessellate>1</tessellate>" : u"") %
               (settings.extrude ? u"<extrude>1</extrude>" : u"") %
               CKmlUtils::wrapAsKmlCoordinates(coordinatesRaw) %
               u"</LineString>";
    }
} // ns
