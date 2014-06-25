#include "blacksimfreefunctions.h"

#include "blacksim/simulatorinfo.h"
#include "blacksim/setsimulator.h"

#include "fsx/simconnectutilities.h"
#include "fscommon/aircraftcfgentrieslist.h"
#include "fscommon/aircraftmappinglist.h"


namespace BlackSim
{

    void registerMetadata()
    {
        BlackSim::CSimulatorInfo::registerMetadata();
        BlackSim::Settings::CSettingsSimulator::registerMetadata();
        BlackSim::FsCommon::CAircraftCfgEntries::registerMetadata();
        BlackSim::FsCommon::CAircraftMapping::registerMetadata();
        BlackSim::FsCommon::CAircraftCfgEntriesList::registerMetadata();
        BlackSim::FsCommon::CAircraftMappingList::registerMetadata();
        BlackSim::Fsx::CSimConnectUtilities::registerMetadata();
    }

} // namespace
