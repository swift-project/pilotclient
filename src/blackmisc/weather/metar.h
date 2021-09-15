/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_METAR_H
#define BLACKMISC_AVIATION_METAR_H

#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/presentweatherlist.h"
#include "blackmisc/weather/windlayer.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc::Weather
{
    class CCloudLayer;
    class CPresentWeather;

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
        const QString &getMessage() const { return m_metarMessage; }

        //! Has METAR message
        bool hasMessage() const;

        //! Set report type
        void setReportType(ReportType type);

        //! Get report type
        ReportType getReportType() const { return m_reportType; }

        //! Set airport icao code
        void setAirportIcaoCode(const BlackMisc::Aviation::CAirportIcaoCode &icao);

        //! Get airport icao code
        const Aviation::CAirportIcaoCode &getAirportIcaoCode() const { return m_airport; }

        //! Set day and time
        void setDayTime(int reportDay, const PhysicalQuantities::CTime &reportTime);

        //! Get report day
        int getDay() const { return m_reportDay; }

        //! Get report time
        const PhysicalQuantities::CTime &getTime() const { return m_reportTime; }

        //! Set the station to automated
        void setAutomated(bool isAutomated);

        //! Is the station automated?
        bool isAutomated() const { return m_isAutomated; }

        //! Set the weather to CAVOK
        void setCavok();

        //! Set wind information
        void setWindLayer(const CWindLayer &windLayer);

        //! Get wind layer
        CWindLayer getWindLayer() const { return m_windLayer; }

        //! Set visibility information
        void setVisibility(const PhysicalQuantities::CLength &visibility);

        //! Get visibility
        PhysicalQuantities::CLength getVisibility() const;

        //! Add information about present weather
        void addPresentWeather(const CPresentWeather &presentWeather);

        //! Get present weather list
        const CPresentWeatherList &getPresentWeather() const { return m_presentWeathers; }

        //! Add cloud layer
        void addCloudLayer(const CCloudLayer &cloudLayer);

        //! Get all cloud layers
        const CCloudLayerList &getCloudLayers() const { return m_cloudLayers; }

        //! Remove all cloud layers
        void removeAllClouds() { m_cloudLayers.clear(); }

        //! Set temperature
        void setTemperature(const PhysicalQuantities::CTemperature &temperature);

        //! Get temperature
        PhysicalQuantities::CTemperature getTemperature() const;

        //! Set dew point
        void setDewPoint(const PhysicalQuantities::CTemperature &dewPoint);

        //! Get dew point
        const PhysicalQuantities::CTemperature &getDewPoint() const { return m_dewPoint; }

        //! Set altimeter
        void setAltimeter(const PhysicalQuantities::CPressure &altimeter);

        //! Get altimeter
        const PhysicalQuantities::CPressure &getAltimeter() const { return m_altimeter; }

        //! Returns the METAR in a descriptive text
        QString getMetarText() const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Return CAVOK metar
        static CMetar CAVOK();

    private:
        QString                          m_metarMessage;
        ReportType                       m_reportType = METAR;
        Aviation::CAirportIcaoCode       m_airport;
        int                              m_reportDay = 0;
        PhysicalQuantities::CTime        m_reportTime;
        bool                             m_isAutomated = false;
        CWindLayer                       m_windLayer;
        PhysicalQuantities::CLength      m_visibility;
        CPresentWeatherList              m_presentWeathers;
        CCloudLayerList                  m_cloudLayers;
        PhysicalQuantities::CTemperature m_temperature;
        PhysicalQuantities::CTemperature m_dewPoint;
        PhysicalQuantities::CPressure    m_altimeter;

        BLACK_METACLASS(
            CMetar,
            BLACK_METAMEMBER(metarMessage),
            BLACK_METAMEMBER(reportType),
            BLACK_METAMEMBER(airport),
            BLACK_METAMEMBER(reportDay),
            BLACK_METAMEMBER(reportTime),
            BLACK_METAMEMBER(isAutomated),
            BLACK_METAMEMBER(windLayer),
            BLACK_METAMEMBER(visibility),
            BLACK_METAMEMBER(presentWeathers),
            BLACK_METAMEMBER(cloudLayers),
            BLACK_METAMEMBER(temperature),
            BLACK_METAMEMBER(dewPoint),
            BLACK_METAMEMBER(altimeter)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CMetar)

#endif // guard
