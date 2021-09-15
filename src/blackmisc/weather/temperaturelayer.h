/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_TEMPERATURELAYER_H
#define BLACKMISC_WEATHER_TEMPERATURELAYER_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc::Weather
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
            IndexTemperatureLayer = BlackMisc::CPropertyIndexRef::GlobalIndexCTemperatureLayer,
            IndexLevel,
            IndexTemperature,
            IndexDewPoint,
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
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        BlackMisc::Aviation::CAltitude m_level;
        PhysicalQuantities::CTemperature m_temperature = { 15.0, PhysicalQuantities::CTemperatureUnit::C() };
        PhysicalQuantities::CTemperature m_dewPoint;
        double m_relativeHumidity = 0;

        BLACK_METACLASS(
            CTemperatureLayer,
            BLACK_METAMEMBER(level),
            BLACK_METAMEMBER(temperature),
            BLACK_METAMEMBER(dewPoint),
            BLACK_METAMEMBER(relativeHumidity)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CTemperatureLayer)

#endif // guard
