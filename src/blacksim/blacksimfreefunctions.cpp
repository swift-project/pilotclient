#include "blacksimfreefunctions.h"
#include "fscommon/aircraftcfgentrieslist.h"
#include "fscommon/aircraftmappinglist.h"

namespace BlackSim
{

    void registerMetadata()
    {
        BlackSim::FsCommon::CAircraftCfgEntries::registerMetadata();
        BlackSim::FsCommon::CAircraftMapping::registerMetadata();
        BlackSim::FsCommon::CAircraftCfgEntriesList::registerMetadata();
        BlackSim::FsCommon::CAircraftMappingList::registerMetadata();
    }

} // namespace
