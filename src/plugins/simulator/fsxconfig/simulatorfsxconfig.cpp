#include "simulatorfsxconfig.h"
#include "simulatorfsxconfigwindow.h"

namespace BlackSimPlugin
{
    namespace Fsx
    {
        CSimulatorFsxConfig::CSimulatorFsxConfig(QObject *parent) : QObject(parent)
        {
            // void
        }

        BlackGui::CPluginConfigWindow *CSimulatorFsxConfig::createConfigWindow(QWidget *parent)
        {
            return new CSimulatorFsxConfigWindow(parent);
        }
    }
}
