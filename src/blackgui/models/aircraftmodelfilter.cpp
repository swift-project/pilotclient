/* Copyright (C) 2013
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

        CAircraftModelFilter::CAircraftModelFilter(const QString &modelString, const QString &description) :
            m_model(modelString.trimmed()), m_description(description.trimmed())
        {  }

        BlackMisc::Simulation::CAircraftModelList CAircraftModelFilter::filter(const CAircraftModelList &inContainer) const
        {
            if (!this->isValid()) { return inContainer; }
            CAircraftModelList outContainer;
            for (const CAircraftModel &model : inContainer)
            {
                if (!this->m_model.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getModelString(), this->m_model)) { continue; }
                }

                if (!this->m_description.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getDescription(), this->m_description)) { continue; }
                }
                outContainer.push_back(model);
            }
            return outContainer;
        }

        bool CAircraftModelFilter::isValid() const
        {
            return !(this->m_model.isEmpty() && this->m_description.isEmpty());
        }

    } // namespace
} // namespace
