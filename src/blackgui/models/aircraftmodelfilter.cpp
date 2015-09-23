/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelfilter.h"
#include "blackmisc/simulation/aircraftmodel.h"

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Models
    {

        CAircraftModelFilter::CAircraftModelFilter(
            const QString &modelKey, const QString &description,
            const QString &aircraftIcao, const QString &aircraftManufacturer,
            const QString &airlineIcao, const QString &airlineName,
            const QString &liveryCode,
            const CSimulatorInfo &simInfo) :
            m_modelKey(modelKey.trimmed()), m_description(description.trimmed()),
            m_aircraftIcao(aircraftIcao.trimmed().toUpper()), m_aircraftManufacturer(aircraftManufacturer.trimmed().toUpper()),
            m_airlineIcao(airlineIcao.trimmed().toUpper()), m_airlineName(airlineName.trimmed().toUpper()),
            m_liveryCode(liveryCode.trimmed().toUpper()),
            m_simulatorInfo(simInfo)
        {  }

        BlackMisc::Simulation::CAircraftModelList CAircraftModelFilter::filter(const CAircraftModelList &inContainer) const
        {
            if (!this->isEnabled()) { return inContainer; }
            CAircraftModelList outContainer;
            for (const CAircraftModel &model : inContainer)
            {
                if (!m_simulatorInfo.isAllSimulators())
                {
                    if (!this->m_simulatorInfo.matchesAny(model.getSimulatorInfo())) { continue; }
                }

                if (!this->m_modelKey.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getModelString(), this->m_modelKey)) { continue; }
                }

                if (!this->m_description.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getDescription(), this->m_description)) { continue; }
                }

                if (!this->m_aircraftIcao.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAircraftIcaoCodeDesignator(), this->m_aircraftIcao)) { continue; }
                }

                if (!this->m_aircraftManufacturer.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAircraftIcaoCode().getManufacturer(), this->m_aircraftManufacturer)) { continue; }
                }

                if (!this->m_airlineIcao.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAirlineIcaoCodeDesignator(), this->m_airlineIcao)) { continue; }
                }

                if (!this->m_airlineName.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAirlineIcaoCode().getName(), this->m_airlineName)) { continue; }
                }

                if (!this->m_liveryCode.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getLivery().getCombinedCode(), this->m_liveryCode)) { continue; }
                }
                outContainer.push_back(model);
            }
            return outContainer;
        }

        bool CAircraftModelFilter::isValid() const
        {
            return !(this->m_modelKey.isEmpty() && this->m_description.isEmpty() &&
                     this->m_aircraftManufacturer.isEmpty() && this->m_aircraftIcao.isEmpty() &&
                     this->m_airlineIcao.isEmpty() && this->m_airlineName.isEmpty() &&
                     this->m_liveryCode.isEmpty() &&
                     this->m_simulatorInfo.isNoSimulator()
                    );
        }

    } // namespace
} // namespace
