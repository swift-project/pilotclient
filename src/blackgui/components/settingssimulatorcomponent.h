// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSSIMULATORCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSSIMULATORCOMPONENT_H

#include "core/application/applicationsettings.h"
#include "blackgui/blackguiexport.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/simulatorplugininfolist.h"
#include "misc/settingscache.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui
{
    class CSettingsSimulatorComponent;
}
namespace swift::core
{
    class CPluginManagerSimulator;
}
namespace swift::misc::simulation
{
    class CSimulatorPluginInfo;
}
namespace BlackGui::Components
{
    //! All simulator settings component (GUI)
    class BLACKGUI_EXPORT CSettingsSimulatorComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsSimulatorComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsSimulatorComponent() override;

    private:
        //! Driver plugin enabled/disabled
        void pluginStateChanged(const QString &identifier, bool enabled);

        //! Apply max.aircraft
        void onApplyMaxRenderedAircraft();

        //! Apply max.distance
        void onApplyMaxRenderedDistance();

        //! Apply disable rendering
        void onApplyDisableRendering();

        //! Apply time synchronization
        void onApplyTimeSync();

        //! Apply COM sync
        void onApplyComSync();

        //! Apply CG source
        void onApplyCGSource();

        //! Record GND
        void onApplyRecordGnd();

        //! Reload settings
        void onReload();

        //! Simulators changed
        void onEnabledSimulatorsChanged();

        //! Clear restricted rendering
        void clearRestricedRendering();

        //! Simulator plugin changed
        void simulatorPluginChanged(const swift::misc::simulation::CSimulatorPluginInfo &info);

        //! Open plugin details window
        void showPluginDetails(const QString &identifier);

        //! Show plugin config
        void showPluginConfig(const QString &identifier);

        //! Select/deselect enabled/disabled plugins
        void reloadPluginConfig(const swift::misc::simulation::CSimulatorPluginInfoList &plugins);

        //! Check plugins again
        void checkSimulatorPlugins();

        //! Set the GUI values
        void setGuiValues();

        //! Available plugins, auto pseudo plugin added
        static swift::misc::simulation::CSimulatorPluginInfoList getAvailablePlugins();

        //! Get the simulator settings
        static swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings(bool &ok);

        //! Set the simulator settings
        static void setSimulatorSettings(swift::misc::simulation::settings::CSimulatorSettings &settings);

        QScopedPointer<Ui::CSettingsSimulatorComponent> ui; //!< UI
        bool m_pluginLoaded = false; //!< plugin loaded?
        swift::core::CPluginManagerSimulator *m_plugins = nullptr;
        swift::misc::CSetting<swift::core::application::TEnabledSimulators> m_enabledSimulators { this, &CSettingsSimulatorComponent::onEnabledSimulatorsChanged };
    };
} // namespace

#endif // guard
