/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H

#include "blackgui/pluginconfigwindow.h"
#include "blackmisc/simulation/settings/xswiftbussettings.h"
#include "simulatorxplaneconfig.h"
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CSimulatorXPlaneConfigWindow; }
namespace BlackSimPlugin::XPlane
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
