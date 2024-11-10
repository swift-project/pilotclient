// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_WINDOW_H
#define BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_WINDOW_H

#include "blackgui/pluginconfigwindow.h"
#include "misc/simulation/settings/fgswiftbussettings.h"
#include "simulatorflightgearconfig.h"
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSimulatorFlightgearConfigWindow;
}
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
        swift::misc::simulation::settings::CFGSwiftBusSettings getSettingsFromUI() const;

        QScopedPointer<Ui::CSimulatorFlightgearConfigWindow> ui;
        swift::misc::CSetting<swift::misc::simulation::settings::TFGSwiftBusServer> m_fgswiftbusServerSetting { this, &CSimulatorFlightgearConfigWindow::onSettingsChanged };
    };
} // ns

#endif // guard
