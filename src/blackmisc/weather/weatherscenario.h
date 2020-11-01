/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_WEATHERSCENARIO_H
#define BLACKMISC_WEATHER_WEATHERSCENARIO_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QVector>

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object for fixed weather scenario
         */
        class BLACKMISC_EXPORT CWeatherScenario : public CValueObject<CWeatherScenario>
        {
        public:
            //! Scenario Index
            enum ScenarioIndex
            {
                ClearSky,
                Thunderstorm,
                RealWeather
            };

            //! Properties by index
            enum ColumnIndex
            {
                IndexScenarioIndex = CPropertyIndexRef::GlobalIndexCWeatherScenario,
                IndexScenarioName,
                IndexScenarioDescription
            };

            //! \copydoc BlackMisc::CValueObject::registerMetadata
            static void registerMetadata();

            //! Default constructor.
            CWeatherScenario() = default;

            //! Constructor
            CWeatherScenario(ScenarioIndex index);

            //! Constructor
            CWeatherScenario(ScenarioIndex index, const QString &name, const QString &description);

            //! Set scenario index
            void setIndex(ScenarioIndex index) { m_scenarioIndex = index; }

            //! Get scenario index
            ScenarioIndex getIndex() const { return m_scenarioIndex; }

            //! Set scenario name
            void setName(const QString &name) { m_scenarioName = name; }

            //! Get scenario name
            QString getName() const { return m_scenarioName; }

            //! Set scenario description
            void setDescription(const QString &description) { m_scenarioDescription = description; }

            //! Get scenario description
            QString getDescription() const { return m_scenarioDescription; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Is scenario the real weather scenario?
            static bool isRealWeatherScenario(const CWeatherScenario &scenario) { return scenario.getIndex() == RealWeather; }

            //! As string
            static const QString &enumToString(ScenarioIndex index);

            //! As string
            static const QString &enumToDescription(ScenarioIndex index);

        private:
            ScenarioIndex m_scenarioIndex = ClearSky;
            QString m_scenarioName        = enumToString(ClearSky);
            QString m_scenarioDescription = enumToDescription(ClearSky);

            BLACK_METACLASS(
                CWeatherScenario,
                BLACK_METAMEMBER(scenarioIndex),
                BLACK_METAMEMBER(scenarioName),
                BLACK_METAMEMBER(scenarioDescription)
            );
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CWeatherScenario)
Q_DECLARE_METATYPE(BlackMisc::Weather::CWeatherScenario::ScenarioIndex)

#endif //guard
