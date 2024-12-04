// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/weather/metar.h"

#include <QtGlobal>

#include "misc/pq/units.h"
#include "misc/weather/cloudlayer.h"
#include "misc/weather/presentweather.h"

using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::weather, CMetar)

namespace swift::misc::weather
{
    CMetar::CMetar() { setCavok(); }

    void CMetar::setMessage(const QString &message) { m_metarMessage = message; }

    bool CMetar::hasMessage() const { return !this->getMessage().isEmpty(); }

    void CMetar::setReportType(ReportType type) { m_reportType = type; }

    void CMetar::setAirportIcaoCode(const CAirportIcaoCode &icao) { m_airport = icao; }

    void CMetar::setDayTime(int reportDay, const physical_quantities::CTime &reportTime)
    {
        m_reportDay = reportDay;
        m_reportTime = reportTime;
    }

    void CMetar::setAutomated(bool isAutomated) { m_isAutomated = isAutomated; }

    void CMetar::setCavok()
    {
        m_visibility = CLength(10000, CLengthUnit::km());
        m_presentWeathers.clear();
        m_cloudLayers.clear();
    }

    void CMetar::setWindLayer(const CWindLayer &windLayer) { m_windLayer = windLayer; }

    void CMetar::setVisibility(const physical_quantities::CLength &visibility) { m_visibility = visibility; }

    physical_quantities::CLength CMetar::getVisibility() const { return m_visibility; }

    void CMetar::addPresentWeather(const CPresentWeather &presentWeather)
    {
        m_presentWeathers.push_back(presentWeather);
    }

    void CMetar::addCloudLayer(const CCloudLayer &cloudLayer) { m_cloudLayers.push_back(cloudLayer); }

    void CMetar::setTemperature(const physical_quantities::CTemperature &temperature) { m_temperature = temperature; }

    physical_quantities::CTemperature CMetar::getTemperature() const { return m_temperature; }

    void CMetar::setDewPoint(const physical_quantities::CTemperature &dewPoint) { m_dewPoint = dewPoint; }

    void CMetar::setAltimeter(const physical_quantities::CPressure &altimeter) { m_altimeter = altimeter; }

    QString CMetar::getMetarText() const
    {
        QString presentWeathers;
        for (const auto &presentWeather : m_presentWeathers)
        {
            if (!presentWeathers.isEmpty()) presentWeathers += ",";
            presentWeathers += u' ' % presentWeather.toQString();
        }

        QString clouds;
        for (const auto &layer : m_cloudLayers)
        {
            if (!clouds.isEmpty()) clouds += ",";
            clouds += u' ' % layer.toQString();
        }

        const QString metarDescription =
            QStringLiteral("Station: %1 \n").arg(m_airport.getIcaoCode()) %
            QStringLiteral("Date/Time: %1 %2 UTC\n").arg(m_reportDay).arg(m_reportTime.formattedHrsMin()) %
            m_windLayer.toQString() % u'\n' % QStringLiteral("Visibility: %1\n").arg(m_visibility.toQString()) %
            u"Weather: " % presentWeathers.simplified() % u'\n' % u"Clouds:" % clouds % u'\n' %
            QStringLiteral("Temperature: %1\n").arg(m_temperature.toQString()) %
            QStringLiteral("Dewpoint: %1\n").arg(m_dewPoint.toQString()) %
            QStringLiteral("Altimeter: %1\n").arg(m_altimeter.toQString());

        return metarDescription;
    }

    QString CMetar::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s(m_airport.getIcaoCode());
        return s;
    }

    CMetar CMetar::CAVOK()
    {
        CMetar metar;
        metar.setCavok();
        return metar;
    }
} // namespace swift::misc::weather
