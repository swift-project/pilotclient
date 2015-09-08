#include "simulatorxplaneconfig.h"
#include "simulatorxplaneconfigwindow.h"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlaneConfig::CSimulatorXPlaneConfig(QObject *parent) : QObject(parent)
        {

        }

        BlackGui::CPluginConfigWindow *CSimulatorXPlaneConfig::createConfigWindow()
        {
            CSimulatorXPlaneConfigWindow* w = new CSimulatorXPlaneConfigWindow();
            return w;
        }
    }
}
