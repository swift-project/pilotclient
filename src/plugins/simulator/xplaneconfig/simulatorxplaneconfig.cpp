/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorxplaneconfig.h"
#include "simulatorxplaneconfigwindow.h"

class QWidget;

namespace BlackSimPlugin::XPlane
{
    CSimulatorXPlaneConfig::CSimulatorXPlaneConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    BlackGui::CPluginConfigWindow *CSimulatorXPlaneConfig::createConfigWindow(QWidget *parent)
    {
        CSimulatorXPlaneConfigWindow *w = new CSimulatorXPlaneConfigWindow(parent);
        return w;
    }
} // ns
