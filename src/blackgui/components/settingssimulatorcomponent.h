/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSSIMULATORCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSSIMULATORCOMPONENT_H

#include "blackcore/application/applicationsettings.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/settingscache.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui
{
    class CSettingsSimulatorComponent;
}
namespace BlackCore
{
    class CPluginManagerSimulator;
}
namespace BlackMisc::Simulation
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
        void simulatorPluginChanged(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        //! Open plugin details window
        void showPluginDetails(const QString &identifier);

        //! Show plugin config
        void showPluginConfig(const QString &identifier);

        //! Select/deselect enabled/disabled plugins
        void reloadPluginConfig(const BlackMisc::Simulation::CSimulatorPluginInfoList &plugins);

        //! Check plugins again
        void checkSimulatorPlugins();

        //! Set the GUI values
        void setGuiValues();

        //! Available plugins, auto pseudo plugin added
        static BlackMisc::Simulation::CSimulatorPluginInfoList getAvailablePlugins();

        //! Get the simulator settings
        static BlackMisc::Simulation::Settings::CSimulatorSettings getSimulatorSettings(bool &ok);

        //! Set the simulator settings
        static void setSimulatorSettings(BlackMisc::Simulation::Settings::CSimulatorSettings &settings);

        QScopedPointer<Ui::CSettingsSimulatorComponent> ui; //!< UI
        bool m_pluginLoaded = false; //!< plugin loaded?
        BlackCore::CPluginManagerSimulator *m_plugins = nullptr;
        BlackMisc::CSetting<BlackCore::Application::TEnabledSimulators> m_enabledSimulators { this, &CSettingsSimulatorComponent::onEnabledSimulatorsChanged };
    };
} // namespace

#endif // guard
