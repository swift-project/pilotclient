/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_METAR_H
#define BLACKMISC_AVIATION_METAR_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/presentweatherlist.h"
#include "blackmisc/weather/windlayer.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/pressure.h"

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object encapsulating information about METAR
         * FIXME: runway visibilities
         * FIXME: runway wind shear
         * FIXME: remarks
         */
        class BLACKMISC_EXPORT CMetar : public CValueObject<CMetar>
        {
        public:
            //! Report type
            enum ReportType
            {
                METAR,
                SPECI
            };

            //! Default constructor
            CMetar();

            //! Set METAR message
            void setMessage(const QString &message);

            //! Get METAR message
            QString getMessage() const;

            //! Set report type
            void setReportType(ReportType type);

            //! Get report type
            ReportType getReportType() const;

            //! Set airport icao code
            void setAirportIcaoCode(const BlackMisc::Aviation::CAirportIcaoCode &icao);

            //! Get airport icao code
            BlackMisc::Aviation::CAirportIcaoCode getAirportIcaoCode() const;

            //! Set day and time
            void setDayTime(int reportDay, const PhysicalQuantities::CTime &reportTime);

            //! Get report day
            int getDay() const;

            //! Get report time
            PhysicalQuantities::CTime getTime() const;

            //! Set the station to automated
            void setAutomated(bool isAutomated);

            //! Is the station automated?
            bool isAutomated() const { return m_isAutomated; }

            //! Set the weather to CAVOK
            void setCavok();

            //! Is CAVOK?
            bool isCavok() const;

            //! Set wind information
            void setWindLayer(const CWindLayer &windLayer);

            //! Get wind layer
            CWindLayer getWindLayer() const;

            //! Set visibility information
            void setVisibility(const PhysicalQuantities::CLength &visibility);

            //! Get visibility
            PhysicalQuantities::CLength getVisibility() const;

            //! Add information about present weather
            void addPresentWeather(const CPresentWeather &presentWeather);

            //! Get present weather list
            CPresentWeatherList getPresentWeather() const;

            //! Add cloud layer
            void addCloudLayer(const CCloudLayer &cloudLayer);

            //! Get all cloud layers
            CCloudLayerList getCloudLayers() const;

            //! Remove all cloud layers
            void removeAllClouds() { m_cloudLayers.clear(); }

            //! Set temperature
            void setTemperature(const PhysicalQuantities::CTemperature &temperature);

            //! Get temperature
            PhysicalQuantities::CTemperature getTemperature() const;

            //! Set dew point
            void setDewPoint(const PhysicalQuantities::CTemperature &dewPoint);

            //! Get dew point
            PhysicalQuantities::CTemperature getDewPoint() const;

            //! Set altimeter
            void setAltimeter(const PhysicalQuantities::CPressure &altimeter);

            //! Get altimeter
            PhysicalQuantities::CPressure getAltimeter() const;

            //! Returns the metar in a descriptive text
            QString getMetarText() const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Return CAVOK metar
            static CMetar CAVOK();

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CMetar)
            QString m_metarMessage;
            ReportType m_reportType = METAR;
            BlackMisc::Aviation::CAirportIcaoCode m_airport;
            int m_reportDay = 0;
            PhysicalQuantities::CTime m_reportTime;
            bool m_isAutomated = false;
            CWindLayer m_windLayer;
            PhysicalQuantities::CLength m_visibility;
            CPresentWeatherList m_presentWeathers;
            CCloudLayerList m_cloudLayers;
            PhysicalQuantities::CTemperature m_temperature;
            PhysicalQuantities::CTemperature m_dewPoint;
            PhysicalQuantities::CPressure m_altimeter;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CMetar)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Weather::CMetar, (
    attr(o.m_metarMessage),
    attr(o.m_reportType),
    attr(o.m_airport),
    attr(o.m_reportDay),
    attr(o.m_reportTime),
    attr(o.m_isAutomated),
    attr(o.m_windLayer),
    attr(o.m_visibility),
    attr(o.m_presentWeathers),
    attr(o.m_cloudLayers),
    attr(o.m_temperature),
    attr(o.m_dewPoint),
    attr(o.m_altimeter)
))

#endif // guard
