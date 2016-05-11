/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/pq/units.h"
#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/weather/presentweather.h"

#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Weather
    {

        CMetar::CMetar()
        {
            setCavok();
        }

        void CMetar::setMessage(const QString &message)
        {
            m_metarMessage = message;
        }

        QString CMetar::getMessage() const
        {
            return m_metarMessage;
        }

        void CMetar::setReportType(ReportType type)
        {
            m_reportType = type;
        }

        CMetar::ReportType CMetar::getReportType() const
        {
            return m_reportType;
        }

        void CMetar::setAirportIcaoCode(const CAirportIcaoCode &icao)
        {
            m_airport = icao;
        }

        CAirportIcaoCode CMetar::getAirportIcaoCode() const
        {
            return m_airport;
        }

        void CMetar::setDayTime(int reportDay, const PhysicalQuantities::CTime &reportTime)
        {
            m_reportDay = reportDay; m_reportTime = reportTime;
        }

        int CMetar::getDay() const
        {
            return m_reportDay;
        }

        PhysicalQuantities::CTime CMetar::getTime() const
        {
            return m_reportTime;
        }

        void CMetar::setAutomated(bool isAutomated)
        {
            m_isAutomated = isAutomated;
        }

        void CMetar::setCavok()
        {
            m_visibility = CLength(10000, CLengthUnit::km());
            m_presentWeathers.clear();
            m_cloudLayers.clear();
        }

        bool CMetar::isCavok() const
        {
            return false;
        }

        void CMetar::setWindLayer(const CWindLayer &windLayer)
        {
            m_windLayer = windLayer;
        }

        CWindLayer CMetar::getWindLayer() const
        {
            return m_windLayer;
        }

        void CMetar::setVisibility(const PhysicalQuantities::CLength &visibility)
        {
            m_visibility = visibility;
        }

        PhysicalQuantities::CLength CMetar::getVisibility() const
        {
            return m_visibility;
        }

        void CMetar::addPresentWeather(const CPresentWeather &presentWeather)
        {
            m_presentWeathers.push_back(presentWeather);
        }

        CPresentWeatherList CMetar::getPresentWeather() const
        {
            return m_presentWeathers;
        }

        void CMetar::addCloudLayer(const CCloudLayer &cloudLayer)
        {
            m_cloudLayers.push_back(cloudLayer);
        }

        CCloudLayerList CMetar::getCloudLayers() const
        {
            return m_cloudLayers;
        }

        void CMetar::setTemperature(const PhysicalQuantities::CTemperature &temperature)
        {
            m_temperature = temperature;
        }

        PhysicalQuantities::CTemperature CMetar::getTemperature() const
        {
            return m_temperature;
        }

        void CMetar::setDewPoint(const PhysicalQuantities::CTemperature &dewPoint)
        {
            m_dewPoint = dewPoint;
        }

        PhysicalQuantities::CTemperature CMetar::getDewPoint() const
        {
            return m_dewPoint;
        }

        void CMetar::setAltimeter(const PhysicalQuantities::CPressure &altimeter)
        {
            m_altimeter = altimeter;
        }

        PhysicalQuantities::CPressure CMetar::getAltimeter() const
        {
            return m_altimeter;
        }

        QString CMetar::getMetarText() const
        {
            QString metarDescription;
            metarDescription += QString("Station: %1 \n").arg(m_airport.getIcaoCode());
            metarDescription += QString("Date/Time: %1 %2 UTC\n").arg(m_reportDay).arg(m_reportTime.formattedHrsMin());
            metarDescription += m_windLayer.toQString();
            metarDescription += "\n";
            metarDescription += QString("Visibility: %1\n").arg(m_visibility.toQString());
            metarDescription += QString("Weather: ");
            QString presentWeathers;
            for (const auto &presentWeather : m_presentWeathers)
            {
                if (!presentWeathers.isEmpty()) presentWeathers += ",";
                presentWeathers += " ";
                presentWeathers += presentWeather.toQString();
            }
            metarDescription += presentWeathers.simplified();
            metarDescription += QString("\n");
            metarDescription += QString("Clouds:");
            QString clouds;
            for (const auto &layer : m_cloudLayers)
            {
                if (!clouds.isEmpty()) clouds += ",";
                clouds += " ";
                clouds += layer.toQString();
            }
            metarDescription += clouds;
            metarDescription += QString("\n");
            metarDescription += QString("Temperature: %1\n").arg(m_temperature.toQString());
            metarDescription += QString("Dewpoint: %1\n").arg(m_dewPoint.toQString());
            metarDescription += QString("Altimeter: %1\n").arg(m_altimeter.toQString());
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

    } // namespace
} // namespace
