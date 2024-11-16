// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/geo/kmlutils.h"

#include <QStringBuilder>

using namespace swift::misc;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;

namespace swift::misc::geo
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
} // namespace swift::misc::geo
