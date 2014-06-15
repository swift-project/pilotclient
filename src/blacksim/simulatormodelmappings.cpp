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

    BlackMisc::Network::CAircraftMappingList ISimulatorModelMappings::findByIcao(const BlackMisc::Aviation::CAircraftIcao &icao, bool emptyMeansWildCard) const
    {
        return this->m_mappings.findByIcaoCode(icao, emptyMeansWildCard);
    }

} // namespace
