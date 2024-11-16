// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H
#define SWIFT_SIMPLUGIN_SIMULATOR_XPLANE_CONFIG_WINDOW_H

#include <QObject>
#include <QScopedPointer>

#include "simulatorxplaneconfig.h"

#include "gui/pluginconfigwindow.h"
#include "misc/simulation/settings/xswiftbussettings.h"

namespace Ui
{
    class CSimulatorXPlaneConfigWindow;
}
namespace swift::simplugin::xplane
{
    /*!
     * A window that shows all the X-Plane plugin options.
     */
    class CSimulatorXPlaneConfigWindow : public swift::gui::CPluginConfigWindow
    {
        Q_OBJECT

    public:
        //! Ctor.
        CSimulatorXPlaneConfigWindow(QWidget *parent);

        //! Dtor.
        virtual ~CSimulatorXPlaneConfigWindow() override;

    private:
        //! Settings from UI
        swift::misc::simulation::settings::CXSwiftBusSettings getSettingsFromUI() const;

        //! Set settings
        void setUiValues(const swift::misc::simulation::settings::CXSwiftBusSettings &settings);

        //! Settings have been accepted
        void onSettingsAccepted();

        //! Settings changed
        void onSettingsChanged();

        //! Margin value to int
        static int marginToInt(const QString &text, int defaultValue);

        QScopedPointer<Ui::CSimulatorXPlaneConfigWindow> ui;
        swift::misc::CSetting<swift::misc::simulation::settings::TXSwiftBusSettings> m_xSwiftBusServerSettings { this, &CSimulatorXPlaneConfigWindow::onSettingsChanged };
    };
} // namespace swift::simplugin::xplane

#endif // guard
