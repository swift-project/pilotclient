// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H

#include "blackgui/pluginconfigwindow.h"
#include "blackmisc/simulation/settings/xswiftbussettings.h"
#include "simulatorxplaneconfig.h"
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSimulatorXPlaneConfigWindow;
}
namespace BlackSimPlugin::XPlane
{
    /*!
     * A window that shows all the X-Plane plugin options.
     */
    class CSimulatorXPlaneConfigWindow : public BlackGui::CPluginConfigWindow
    {
        Q_OBJECT

    public:
        //! Ctor.
        CSimulatorXPlaneConfigWindow(QWidget *parent);

        //! Dtor.
        virtual ~CSimulatorXPlaneConfigWindow() override;

    private:
        //! Settings from UI
        BlackMisc::Simulation::Settings::CXSwiftBusSettings getSettingsFromUI() const;

        //! Set settings
        void setUiValues(const BlackMisc::Simulation::Settings::CXSwiftBusSettings &settings);

        //! Settings have been accepted
        void onSettingsAccepted();

        //! Settings changed
        void onSettingsChanged();

        //! Margin value to int
        static int marginToInt(const QString &text, int defaultValue);

        QScopedPointer<Ui::CSimulatorXPlaneConfigWindow> ui;
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TXSwiftBusSettings> m_xSwiftBusServerSettings { this, &CSimulatorXPlaneConfigWindow::onSettingsChanged };
    };
} // ns

#endif // guard
