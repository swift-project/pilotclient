/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/countryfilter.h"
#include "blackmisc/country.h"

using namespace BlackMisc;

namespace BlackGui::Models
{
    CCountryFilter::CCountryFilter(const QString &isoCode, const QString &name) : m_isoCode(isoCode.trimmed().toUpper()), m_name(name.trimmed())
    {
        this->m_valid = !(this->m_isoCode.isEmpty() && this->m_name.isEmpty());
    }

    CCountryList CCountryFilter::filter(const CCountryList &inContainer) const
    {
        if (!this->isValid()) { return inContainer; }
        CCountryList outContainer;
        bool end = false;
        for (const CCountry &country : inContainer)
        {
            if (!m_isoCode.isEmpty())
            {
                if (country.getIsoCode() != m_isoCode) { continue; }
                end = true; // there should be only one designator
            }

            if (!this->m_name.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(country.getName(), this->m_name)) { continue; }
            }

            outContainer.push_back(country);
            if (end) { break; }
        }
        return outContainer;
    }
} // namespace
