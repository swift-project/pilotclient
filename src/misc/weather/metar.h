// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_METAR_H
#define SWIFT_MISC_AVIATION_METAR_H

#include <QMetaType>
#include <QString>

#include "misc/aviation/airporticaocode.h"
#include "misc/metaclass.h"
#include "misc/pq/length.h"
#include "misc/pq/pressure.h"
#include "misc/pq/temperature.h"
#include "misc/pq/time.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"
#include "misc/weather/cloudlayerlist.h"
#include "misc/weather/presentweatherlist.h"
#include "misc/weather/windlayer.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::weather, CMetar)

namespace swift::misc::weather
{
    class CCloudLayer;
    class CPresentWeather;

    /*!
     * Value object encapsulating information about METAR
     * FIXME: runway visibilities
     * FIXME: runway wind shear
     * FIXME: remarks
     */
    class SWIFT_MISC_EXPORT CMetar : public CValueObject<CMetar>
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
        void setAirportIcaoCode(const swift::misc::aviation::CAirportIcaoCode &icao);

        //! Get airport icao code
        const aviation::CAirportIcaoCode &getAirportIcaoCode() const { return m_airport; }

        //! Set day and time
        void setDayTime(int reportDay, const physical_quantities::CTime &reportTime);

        //! Get report day
        int getDay() const { return m_reportDay; }

        //! Get report time
        const physical_quantities::CTime &getTime() const { return m_reportTime; }

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
        void setVisibility(const physical_quantities::CLength &visibility);

        //! Get visibility
        physical_quantities::CLength getVisibility() const;

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
        void setTemperature(const physical_quantities::CTemperature &temperature);

        //! Get temperature
        physical_quantities::CTemperature getTemperature() const;

        //! Set dew point
        void setDewPoint(const physical_quantities::CTemperature &dewPoint);

        //! Get dew point
        const physical_quantities::CTemperature &getDewPoint() const { return m_dewPoint; }

        //! Set altimeter
        void setAltimeter(const physical_quantities::CPressure &altimeter);

        //! Get altimeter
        const physical_quantities::CPressure &getAltimeter() const { return m_altimeter; }

        //! Returns the METAR in a descriptive text
        QString getMetarText() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Return CAVOK metar
        static CMetar CAVOK();

    private:
        QString m_metarMessage;
        ReportType m_reportType = METAR;
        aviation::CAirportIcaoCode m_airport;
        int m_reportDay = 0;
        physical_quantities::CTime m_reportTime;
        bool m_isAutomated = false;
        CWindLayer m_windLayer;
        physical_quantities::CLength m_visibility;
        CPresentWeatherList m_presentWeathers;
        CCloudLayerList m_cloudLayers;
        physical_quantities::CTemperature m_temperature;
        physical_quantities::CTemperature m_dewPoint;
        physical_quantities::CPressure m_altimeter;

        SWIFT_METACLASS(
            CMetar,
            SWIFT_METAMEMBER(metarMessage),
            SWIFT_METAMEMBER(reportType),
            SWIFT_METAMEMBER(airport),
            SWIFT_METAMEMBER(reportDay),
            SWIFT_METAMEMBER(reportTime),
            SWIFT_METAMEMBER(isAutomated),
            SWIFT_METAMEMBER(windLayer),
            SWIFT_METAMEMBER(visibility),
            SWIFT_METAMEMBER(presentWeathers),
            SWIFT_METAMEMBER(cloudLayers),
            SWIFT_METAMEMBER(temperature),
            SWIFT_METAMEMBER(dewPoint),
            SWIFT_METAMEMBER(altimeter));
    };
} // namespace swift::misc::weather

Q_DECLARE_METATYPE(swift::misc::weather::CMetar)

#endif // SWIFT_MISC_AVIATION_METAR_H
