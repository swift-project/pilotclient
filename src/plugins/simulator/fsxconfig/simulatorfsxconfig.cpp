/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorfsxconfig.h"
#include "../fsxcommon/simulatorfsxconfigwindow.h"

using namespace BlackGui;
using namespace BlackSimPlugin::FsxCommon;

namespace BlackSimPlugin::Fsx
{
    CSimulatorFsxConfig::CSimulatorFsxConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    CPluginConfigWindow *CSimulatorFsxConfig::createConfigWindow(QWidget *parent)
    {
        return new CSimulatorFsxConfigWindow("FSX", parent);
    }
}
