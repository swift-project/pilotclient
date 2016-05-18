/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/aircrafticaofilter.h"
#include "blackmisc/aviation/aircrafticaocode.h"

using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        CAircraftIcaoFilter::CAircraftIcaoFilter(
            const QString &designator, const QString &name,
            const QString &description, const QString &combinedType) :
            m_designator(designator.trimmed().toUpper()), m_manufacturer(name.trimmed()),
            m_description(description.trimmed()),  m_combinedType(combinedType.trimmed().toUpper())
        {
            this->m_valid = !(this->m_combinedType.isEmpty() && this->m_designator.isEmpty() &&
                              this->m_description.isEmpty() && this->m_manufacturer.isEmpty());
        }

        CAircraftIcaoCodeList CAircraftIcaoFilter::filter(const CAircraftIcaoCodeList &inContainer) const
        {
            if (!this->isValid()) { return inContainer; }
            CAircraftIcaoCodeList outContainer;
            bool filterCombinedCode = !this->m_combinedType.isEmpty() && !this->m_combinedType.contains('-') && CAircraftIcaoCode::isValidCombinedType(this->m_combinedType);

            for (const CAircraftIcaoCode &icao : inContainer)
            {
                if (!m_designator.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(icao.getDesignator(), m_designator)) { continue; }
                }
                if (!m_manufacturer.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(icao.getManufacturer(), m_manufacturer)) { continue; }
                }
                if (!m_description.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(icao.getModelDescription(), m_description)) { continue; }
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
} // namespace
