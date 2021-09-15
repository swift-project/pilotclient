/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorp3dconfig.h"
#include "../fsxcommon/simulatorfsxconfigwindow.h"

namespace BlackSimPlugin::P3D
{
    CSimulatorP3DConfig::CSimulatorP3DConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    BlackGui::CPluginConfigWindow *CSimulatorP3DConfig::createConfigWindow(QWidget *parent)
    {
        return new FsxCommon::CSimulatorFsxConfigWindow("P3D", parent);
    }
}
