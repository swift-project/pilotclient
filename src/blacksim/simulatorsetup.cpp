#include "simulatorsetup.h"

namespace BlackSim
{
    void CSimulatorSetup::setSettings(const BlackMisc::CPropertyIndexVariantMap &map)
    {
        this->m_setup = map;
    }

    void CSimulatorSetup::init()
    {
        // void
    }
}
