// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/countryfilter.h"
#include "misc/country.h"

using namespace swift::misc;

namespace swift::gui::models
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
