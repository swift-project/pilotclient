/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatormodelmappings.h"

namespace BlackSim
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

    const BlackMisc::Network::CAircraftMappingList &ISimulatorModelMappings::getMappingList() const
    {
        return this->m_mappings;
    }

    BlackMisc::Network::CAircraftMappingList ISimulatorModelMappings::findByIcaoWildcard(const BlackMisc::Aviation::CAircraftIcao &icao) const
    {
        return this->m_mappings.findByIcaoCodeWildcard(icao);
    }

    BlackMisc::Network::CAircraftMappingList ISimulatorModelMappings::findByIcaoExact(const BlackMisc::Aviation::CAircraftIcao &icao) const
    {
        return this->m_mappings.findByIcaoCodeExact(icao);
    }

} // namespace
