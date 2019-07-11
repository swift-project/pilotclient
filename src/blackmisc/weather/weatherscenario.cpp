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

        CWeatherScenario::CWeatherScenario(ScenarioIndex index, const QString &name, const QString &description) :
            m_scenarioIndex(index),
            m_scenarioName(name),
            m_scenarioDescription(description)
        { }

        CVariant CWeatherScenario::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexScenarioIndex:       return CVariant::fromValue(m_scenarioIndex);
            case IndexScenarioName:        return CVariant::fromValue(m_scenarioName);
            case IndexScenarioDescription: return CVariant::fromValue(m_scenarioDescription);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CWeatherScenario::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CWeatherScenario>(); return; }
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

        QString CWeatherScenario::convertToQString(bool /** i18n **/) const
        {
            return m_scenarioName;
        }

    } // namespace
} // namespace
