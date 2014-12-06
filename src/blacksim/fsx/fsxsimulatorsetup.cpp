#include "fsxsimulatorsetup.h"

#include "../../blackmisc/project.h"
#include "../fsx/simconnectutilities.h"
#include <QSettings>

using namespace BlackMisc;

namespace BlackSim
{
    namespace Fsx
    {
        void CFsxSimulatorSetup::init()
        {
            CSimulatorSetup::init();
            this->m_setup.addValue(SetupSimConnectCfgFile, CSimConnectUtilities::getLocalSimConnectCfgFilename());

            if (CProject::isCompiledWithFsxSupport())
            {
                // set FSX path
                QSettings fsxRegistry("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0", QSettings::NativeFormat);
                QString fsxPath = fsxRegistry.value("AppPath").toString();
                if (fsxPath.isEmpty())
                {
                    // another trial
                    QSettings fsxRegistry("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0", QSettings::NativeFormat);
                    fsxPath = fsxRegistry.value("SetupPath").toString();
                }
                if (!fsxPath.isEmpty()) this->m_setup.value(CSimulatorSetup::SetupSimPath, CVariant(fsxPath));
            }
        }
    } // namespace
} // namespace
