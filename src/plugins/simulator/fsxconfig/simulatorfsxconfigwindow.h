/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FSX_CONFIG_WINDOW_H
#define BLACKSIMPLUGIN_SIMULATOR_FSX_CONFIG_WINDOW_H

#include "simulatorfsxconfig.h"
#include "blackgui/pluginconfigwindow.h"
#include <QScopedPointer>

namespace Ui {
class CSimulatorFsxConfigWindow;
}

namespace BlackSimPlugin
{
    namespace Fsx
    {

        /**
         * A window the lets user set up the FSX plugin.
         */
        class CSimulatorFsxConfigWindow : public BlackGui::CPluginConfigWindow
        {
            Q_OBJECT

        public:
            //! Ctor.
            CSimulatorFsxConfigWindow(QWidget *parent);

            //! Dtor.
            virtual ~CSimulatorFsxConfigWindow();

        private:
            QScopedPointer<Ui::CSimulatorFsxConfigWindow> ui;
        };
    }
}

#endif // guard
