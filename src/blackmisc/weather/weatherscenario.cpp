/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/weather/weatherscenario.h"

namespace BlackMisc
{
    namespace Weather
    {
        void CWeatherScenario::registerMetadata()
        {
            CValueObject<CWeatherScenario>::registerMetadata();
            qRegisterMetaType<ScenarioIndex>();
        }

        CWeatherScenario::CWeatherScenario(CWeatherScenario::ScenarioIndex index) :
            CWeatherScenario::CWeatherScenario(index, enumToString(index), enumToDescription(index))
        { }

        CWeatherScenario::CWeatherScenario(ScenarioIndex index, const QString &name, const QString &description) :
            m_scenarioIndex(index),
            m_scenarioName(name),
            m_scenarioDescription(description)
        { }

        QVariant CWeatherScenario::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexScenarioIndex:       return QVariant::fromValue(m_scenarioIndex);
            case IndexScenarioName:        return QVariant::fromValue(m_scenarioName);
            case IndexScenarioDescription: return QVariant::fromValue(m_scenarioDescription);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CWeatherScenario::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CWeatherScenario>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexScenarioIndex: setIndex(variant.value<ScenarioIndex>()); break;
            case IndexScenarioName:  setName(variant.value<QString>()); break;
            case IndexScenarioDescription: setDescription(variant.value<QString>()); break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CWeatherScenario::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n)
            return m_scenarioName;
        }

        const QString &CWeatherScenario::enumToString(CWeatherScenario::ScenarioIndex index)
        {
            static const QString cs("Clear Sky");
            static const QString thunder("Thunderstorm");
            static const QString real("Realtime Weather");
            switch (index)
            {
            case ClearSky:     return cs;
            case Thunderstorm: return thunder;
            case RealWeather:  return real;
            default: break;
            }

            static const QString unknown("???");
            return unknown;
        }

        const QString &CWeatherScenario::enumToDescription(CWeatherScenario::ScenarioIndex index)
        {
            static const QString cs("Clear sky, no clouds");
            static const QString thunder("Raining, lightning, several cloud layers");
            static const QString real("As real as it gets...");
            switch (index)
            {
            case ClearSky:     return cs;
            case Thunderstorm: return thunder;
            case RealWeather:  return real;
            default: break;
            }

            static const QString unknown("???");
            return unknown;
        }

    } // namespace
} // namespace
