// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_WINDOW_H
#define SWIFT_SIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_WINDOW_H

#include <QObject>
#include <QScopedPointer>

#include "simulatorflightgearconfig.h"

#include "gui/pluginconfigwindow.h"
#include "misc/simulation/settings/fgswiftbussettings.h"

namespace Ui
{
    class CSimulatorFlightgearConfigWindow;
}
namespace swift::simplugin::flightgear
{
    /*!
     * A window that shows all the Flightgear plugin options.
     */
    class CSimulatorFlightgearConfigWindow : public swift::gui::CPluginConfigWindow
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
        swift::misc::CSetting<swift::misc::simulation::settings::TFGSwiftBusServer> m_fgswiftbusServerSetting {
            this, &CSimulatorFlightgearConfigWindow::onSettingsChanged
        };
    };
} // namespace swift::simplugin::flightgear

#endif // SWIFT_SIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_WINDOW_H
