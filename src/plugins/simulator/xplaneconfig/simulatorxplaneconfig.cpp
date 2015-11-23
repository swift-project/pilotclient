#include "simulatorxplaneconfig.h"
#include "simulatorxplaneconfigwindow.h"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlaneConfig::CSimulatorXPlaneConfig(QObject *parent) : QObject(parent)
        {

        }

        BlackGui::CPluginConfigWindow *CSimulatorXPlaneConfig::createConfigWindow(QWidget *parent)
        {
            CSimulatorXPlaneConfigWindow* w = new CSimulatorXPlaneConfigWindow(parent);
            return w;
        }
    }
}
