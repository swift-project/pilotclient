#include "simulatorsetup.h"

namespace BlackSim
{
    void CSimulatorSetup::setSettings(const BlackMisc::CIndexVariantMap &map)
    {
        this->m_setup = map;
    }

    void CSimulatorSetup::init()
    {
        // void
    }
}
