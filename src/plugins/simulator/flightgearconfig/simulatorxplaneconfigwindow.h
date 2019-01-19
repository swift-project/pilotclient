/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H

#include "blackgui/pluginconfigwindow.h"
#include "blackmisc/simulation/settings/mysettings.h"
#include "simulatorxplaneconfig.h"
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CSimulatorXPlaneConfigWindow; }
namespace BlackSimPlugin
{
    namespace XPlane
    {
        /**
         * A window that shows all the X-Plane plugin options.
         */
        class CSimulatorXPlaneConfigWindow : public BlackGui::CPluginConfigWindow
        {
            Q_OBJECT

        public:
            //! Ctor.
            CSimulatorXPlaneConfigWindow(QWidget *parent);

            //! Dtor.
            virtual ~CSimulatorXPlaneConfigWindow();

        private:
            //! Settings have been accepted
            void onSettingsAccepted();

            QScopedPointer<Ui::CSimulatorXPlaneConfigWindow> ui;
            BlackMisc::CSetting<BlackMisc::Simulation::Settings::TXSwiftBusServer> m_xswiftbusServerSetting { this };
        };
    } // ns
} // ns

#endif // guard
