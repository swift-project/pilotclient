#include "fsxsimulatorsetup.h"
#include "../fsx/simconnectutilities.h"

namespace BlackSim
{
    namespace Fsx
    {
        void CFsxSimulatorSetup::init()
        {
            CSimulatorSetup::init();
            this->m_setup.addValue(SetupSimConnectCfgFile, CSimConnectUtilities::getLocalSimConnectCfgFilename());
        }
    }
}
