/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/airlineicaofilter.h"
#include "blackmisc/aviation/airlineicaocode.h"

using namespace BlackMisc::Aviation;

namespace BlackGui::Models
{
    CAirlineIcaoFilter::CAirlineIcaoFilter(int id, const QString &vDesignator, const QString &name,
                                            const QString &countryIso, bool isReal, bool isVa) :
        m_id(id), m_vDesignator(vDesignator.trimmed().toUpper()), m_name(name.trimmed()),
        m_countryIso(countryIso.trimmed().toUpper()), m_real(isReal), m_va(isVa)
    {
        this->m_valid =  !(m_id < 0 && this->m_countryIso.isEmpty() && this->m_vDesignator.isEmpty() &&
                            this->m_name.isEmpty() && !this->m_va && !this->m_real);
    }

    CAirlineIcaoCodeList CAirlineIcaoFilter::filter(const CAirlineIcaoCodeList &inContainer) const
    {
        if (!this->isValid()) { return inContainer; }
        CAirlineIcaoCodeList outContainer;
        for (const CAirlineIcaoCode &icao : inContainer)
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

            if (!m_vDesignator.isEmpty())
            {
                // based on T72, also find VLHA based on LHA
                if (m_vDesignator.length() == 4)
                {
                    if (!this->stringMatchesFilterExpression(icao.getVDesignator(), m_vDesignator)) { continue; }
                }
                else
                {
                    if (!this->stringMatchesFilterExpression(icao.getDesignator(), m_vDesignator)) { continue; }
                }
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
        }
        return outContainer;
    }
} // namespace
