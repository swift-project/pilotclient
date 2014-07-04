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
