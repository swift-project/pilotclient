/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_TEMPERATURELAYER_H
#define BLACKMISC_WEATHER_TEMPERATURELAYER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/temperature.h"

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object for a temperature layer
         */
        class BLACKMISC_EXPORT CTemperatureLayer : public CValueObject<CTemperatureLayer>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexTemperatureLayer = BlackMisc::CPropertyIndex::GlobalIndexCTemperatureLayer,
                IndexLevel,
                IndexTemperature,
                IndexRelativeHumidity
            };

            //! Default constructor.
            CTemperatureLayer() = default;

            //! Constructor
            CTemperatureLayer(const BlackMisc::Aviation::CAltitude &level,
                              const PhysicalQuantities::CTemperature &temperature,
                              const PhysicalQuantities::CTemperature &dewPoint,
                              double relativeHumidity);

            //! Set level
            void setLevel(const BlackMisc::Aviation::CAltitude &level) { m_level = level; }

            //! Get level
            BlackMisc::Aviation::CAltitude getLevel() const { return m_level; }

            //! Set temperature
            void setTemperature(const PhysicalQuantities::CTemperature &value) { m_temperature = value; }

            //! Get temperature
            PhysicalQuantities::CTemperature getTemperature() const { return m_temperature; }

            //! Set dew point
            void setDewPoint(const PhysicalQuantities::CTemperature &value) { m_dewPoint = value; }

            //! Get dew point
            PhysicalQuantities::CTemperature getDewPoint() const { return m_dewPoint; }

            //! Set relative humidity
            void setRelativeHumidity(double value) { m_relativeHumidity = value; }

            //! Get relative humidity
            double getRelativeHumidity() const { return m_relativeHumidity; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTemperatureLayer)
            BlackMisc::Aviation::CAltitude m_level;
            PhysicalQuantities::CTemperature m_temperature;
            PhysicalQuantities::CTemperature m_dewPoint;
            double m_relativeHumidity = 0;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CTemperatureLayer)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Weather::CTemperatureLayer, (
                                   attr(o.m_level),
                                   attr(o.m_temperature),
                                   attr(o.m_dewPoint),
                                   attr(o.m_relativeHumidity)
))

#endif // guard
