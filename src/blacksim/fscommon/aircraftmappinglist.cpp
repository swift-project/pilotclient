#include "aircraftmappinglist.h"

namespace BlackSim
{
    namespace FsCommon
    {
        /*
         * Register metadata
         */
        void CAircraftMappingList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftMapping>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftMapping>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftMapping>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftMapping>>();
            qRegisterMetaType<CAircraftMappingList>();
            qDBusRegisterMetaType<CAircraftMappingList>();
        }
    }
} // namespace
