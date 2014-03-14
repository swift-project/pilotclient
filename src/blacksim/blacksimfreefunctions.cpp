#include "blacksimfreefunctions.h"
#include "fscommon/aircraftcfgentrieslist.h"
#include "fscommon/aircraftmappinglist.h"
#include "fsx/simconnectutilities.h"

namespace BlackSim
{

    void registerMetadata()
    {
        BlackSim::FsCommon::CAircraftCfgEntries::registerMetadata();
        BlackSim::FsCommon::CAircraftMapping::registerMetadata();
        BlackSim::FsCommon::CAircraftCfgEntriesList::registerMetadata();
        BlackSim::FsCommon::CAircraftMappingList::registerMetadata();
        BlackSim::Fsx::CSimConnectUtilities::registerMetadata();
    }

} // namespace
