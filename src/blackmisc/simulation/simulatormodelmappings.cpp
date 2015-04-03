/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatormodelmappings.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {

        ISimulatorModelMappings::ISimulatorModelMappings(QObject *parent) : QObject(parent) {}

        int ISimulatorModelMappings::size() const
        {
            return this->m_mappings.size();
        }

        bool ISimulatorModelMappings::isEmpty() const
        {
            return this->m_mappings.isEmpty();
        }

        const CAircraftMappingList &ISimulatorModelMappings::getMappingList() const
        {
            return this->m_mappings;
        }

        int ISimulatorModelMappings::synchronizeWithExistingModels(const QStringList &modelNames, Qt::CaseSensitivity cs)
        {
            if (modelNames.isEmpty() || this->m_mappings.isEmpty()) { return this->m_mappings.size(); }
            CAircraftMappingList newList;
            for (const CAircraftMapping &mapping : this->m_mappings)
            {
                QString modelString = mapping.getModel().getModelString();
                if (modelString.isEmpty()) { continue; }
                if (modelNames.contains(modelString, cs))
                {
                    newList.push_back(mapping);
                }
            }
            this->m_mappings = newList;
            return this->m_mappings.size();
        }
    } // namespace
} // namespace
