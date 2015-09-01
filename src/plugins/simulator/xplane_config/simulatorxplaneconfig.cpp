#include "simulatorxplaneconfig.h"
#include "simulatorxplaneconfigwindow.h"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlaneConfig::CSimulatorXPlaneConfig(QObject *parent) : QObject(parent)
        {

        }

        QWidget *CSimulatorXPlaneConfig::createConfigWindow()
        {
            CSimulatorXPlaneConfigWindow* w = new CSimulatorXPlaneConfigWindow();
            return w;
        }

    }
}
