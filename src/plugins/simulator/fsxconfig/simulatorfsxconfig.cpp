#include "simulatorfsxconfig.h"
#include "simulatorfsxconfigwindow.h"

namespace BlackSimPlugin
{
    namespace Fsx
    {

        CSimulatorFsxConfig::CSimulatorFsxConfig(QObject *parent) : QObject(parent)
        {

        }

        BlackGui::CPluginConfigWindow *CSimulatorFsxConfig::createConfigWindow(QWidget *parent)
        {
            return new CSimulatorFsxConfigWindow(parent);
        }
    }
}
