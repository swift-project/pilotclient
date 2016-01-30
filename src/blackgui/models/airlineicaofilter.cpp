/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airlineicaofilter.h"
#include "blackmisc/aviation/airlineicaocode.h"

using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        CAirlineIcaoFilter::CAirlineIcaoFilter(const QString &vDesignator, const QString &name,
                                               const QString &countryIso, bool isReal, bool isVa) :
            m_vDesignator(vDesignator.trimmed().toUpper()), m_name(name.trimmed()),
            m_countryIso(countryIso.trimmed().toUpper()), m_real(isReal), m_va(isVa)
        {  }

        CAirlineIcaoCodeList CAirlineIcaoFilter::filter(const CAirlineIcaoCodeList &inContainer) const
        {
            if (!this->isValid()) { return inContainer; }
            CAirlineIcaoCodeList outContainer;
            bool end = false;
            for (const CAirlineIcaoCode &icao : inContainer)
            {
                if (!m_vDesignator.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(icao.getVDesignator(), m_vDesignator)) { continue; }
                }

                if (!this->m_name.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(icao.getName(), this->m_name)) { continue; }
                }

                if (!this->m_countryIso.isEmpty())
                {
                    if (icao.getCountryIso() != m_countryIso) { continue; }
                }

                if (!m_real && m_va)
                {
                    if (!icao.isVirtualAirline()) { continue; }
                }

                if (m_real && !m_va)
                {
                    if (icao.isVirtualAirline()) { continue; }
                }

                outContainer.push_back(icao);
                if (end) { break; }
            }
            return outContainer;
        }

        bool CAirlineIcaoFilter::isValid() const
        {
            return !(this->m_countryIso.isEmpty() && this->m_vDesignator.isEmpty() &&
                     this->m_name.isEmpty() &&
                     !m_va && !m_real);
        }

    } // namespace
} // namespace
