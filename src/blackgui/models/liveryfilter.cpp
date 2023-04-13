/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/liveryfilter.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"

using namespace BlackMisc::Aviation;

namespace BlackGui::Models
{
    CLiveryFilter::CLiveryFilter(int id, const QString &combinedCode, const QString &descriptiom,
                                 const QString &airlineDesignator,
                                 const BlackMisc::CRgbColor &fuselageColor, const BlackMisc::CRgbColor &tailColor, double maxColorDistance, bool colorLiveries, bool airlineLiveries) : m_id(id),
                                                                                                                                                                                        m_combinedCode(combinedCode.trimmed().toUpper()), m_description(descriptiom),
                                                                                                                                                                                        m_airlineIcaoDesignator(airlineDesignator.trimmed().toUpper()),
                                                                                                                                                                                        m_fuselageColor(fuselageColor), m_tailColor(tailColor), m_maxColorDistance(maxColorDistance),
                                                                                                                                                                                        m_colorLiveries(colorLiveries), m_airlineLiveries(airlineLiveries)
    {
        m_valid = valid();
    }

    CLiveryList CLiveryFilter::filter(const CLiveryList &inContainer) const
    {
        if (!this->isEnabled()) { return inContainer; }
        CLiveryList outContainer;
        bool checkLiveryType = filterByLiveryType();
        for (const CLivery &livery : inContainer)
        {
            if (m_id >= 0)
            {
                // search only for id
                if (livery.isLoadedFromDb() && livery.getDbKey() == m_id)
                {
                    outContainer.push_back(livery);
                    break;
                }
                continue;
            }
            if (checkLiveryType)
            {
                if (!m_colorLiveries && livery.isColorLivery()) { continue; }
                if (!m_airlineLiveries && livery.isAirlineLivery()) { continue; }
            }
            if (!m_combinedCode.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(livery.getCombinedCode(), m_combinedCode)) { continue; }
            }
            if (!m_description.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(livery.getDescription(), m_description)) { continue; }
            }
            if (!m_airlineIcaoDesignator.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(livery.getAirlineIcaoCode().getVDesignator(), m_airlineIcaoDesignator)) { continue; }
            }
            if (m_fuselageColor.isValid())
            {
                if (livery.getColorFuselage().colorDistance(m_fuselageColor) > m_maxColorDistance) { continue; }
            }
            if (m_tailColor.isValid())
            {
                if (livery.getColorTail().colorDistance(m_tailColor) > m_maxColorDistance) { continue; }
            }
            outContainer.push_back(livery);
        }
        return outContainer;
    }

    bool CLiveryFilter::valid() const
    {
        if (filterByLiveryType()) { return true; }
        if (m_fuselageColor.isValid() || m_tailColor.isValid()) { return true; }
        return !(m_id < 0 && m_combinedCode.isEmpty() && m_description.isEmpty() &&
                 m_airlineIcaoDesignator.isEmpty());
    }

    bool CLiveryFilter::filterByLiveryType() const
    {
        return (m_colorLiveries && !m_airlineLiveries) || (!m_colorLiveries && m_airlineLiveries);
    }

} // namespace
