/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_WINDOW_H
#define BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_WINDOW_H

#include "blackgui/pluginconfigwindow.h"
#include "blackmisc/simulation/settings/fgswiftbussettings.h"
#include "simulatorflightgearconfig.h"
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CSimulatorFlightgearConfigWindow; }
namespace BlackSimPlugin::Flightgear
{
    /*!
     * A window that shows all the Flightgear plugin options.
     */
    class CSimulatorFlightgearConfigWindow : public BlackGui::CPluginConfigWindow
    {
        Q_OBJECT

    public:
        //! Ctor.
        CSimulatorFlightgearConfigWindow(QWidget *parent);

        //! Dtor.
        virtual ~CSimulatorFlightgearConfigWindow();

    private:
        //! Settings have been accepted
        void onSettingsAccepted();

        //! Settings changed
        void onSettingsChanged();

        //! Get settings object with values from UI
        BlackMisc::Simulation::Settings::CFGSwiftBusSettings getSettingsFromUI() const;

        QScopedPointer<Ui::CSimulatorFlightgearConfigWindow> ui;
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TFGSwiftBusServer> m_fgswiftbusServerSetting { this, &CSimulatorFlightgearConfigWindow::onSettingsChanged };
    };
} // ns

#endif // guard
