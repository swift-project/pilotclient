// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircrafticaofilter.h"
#include "misc/aviation/aircrafticaocode.h"

using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CAircraftIcaoFilter::CAircraftIcaoFilter(
        int id,
        const QString &designator, const QString &family, const QString &manufacturer,
        const QString &description, const QString &combinedType) : m_id(id),
                                                                   m_designator(designator.trimmed().toUpper()),
                                                                   m_family(family.trimmed().toUpper()),
                                                                   m_manufacturer(manufacturer.trimmed()),
                                                                   m_description(description.trimmed()),
                                                                   m_combinedType(combinedType.trimmed().toUpper())
    {
        m_valid = !(m_id < 0 && m_combinedType.isEmpty() && m_designator.isEmpty() &&
                    m_family.isEmpty() && m_description.isEmpty() && m_manufacturer.isEmpty());
    }

    CAircraftIcaoCodeList CAircraftIcaoFilter::filter(const CAircraftIcaoCodeList &inContainer) const
    {
        if (!this->isValid()) { return inContainer; }
        CAircraftIcaoCodeList outContainer;
        const bool filterCombinedCode = !m_combinedType.isEmpty() && !m_combinedType.contains('-') && CAircraftIcaoCode::isValidCombinedType(m_combinedType);

        for (const CAircraftIcaoCode &icao : inContainer)
        {
            if (m_id >= 0)
            {
                // search only for id
                if (icao.isLoadedFromDb() && icao.getDbKey() == m_id)
                {
                    outContainer.push_back(icao);
                    break;
                }
                continue;
            }
            if (!m_designator.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(icao.getDesignator(), m_designator)) { continue; }
            }
            if (!m_family.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(icao.getFamily(), m_family)) { continue; }
            }
            if (!m_manufacturer.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(icao.getManufacturer(), m_manufacturer)) { continue; }
            }
            if (!m_description.isEmpty())
            {
                const bool ignore =
                    !this->stringMatchesFilterExpression(icao.getModelDescription(), m_description) &&
                    !this->stringMatchesFilterExpression(icao.getModelSwiftDescription(), m_description) &&
                    !this->stringMatchesFilterExpression(icao.getModelIataDescription(), m_description);
                if (ignore) { continue; }
            }
            if (filterCombinedCode)
            {
                if (icao.getCombinedType() != m_combinedType) { continue; }
            }
            outContainer.push_back(icao);
        }
        return outContainer;
    }
} // namespace
