/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include <QColor>
#include <QObject>
#include <QScopedPointer>
#include <QTabWidget>

class QWidget;

namespace Ui
{
    class CSettingsComponent;
}
namespace BlackGui::Components
{
    //! Settings component
    class BLACKGUI_EXPORT CSettingsComponent : public QTabWidget
    {
        Q_OBJECT

    public:
        //! Tabs
        //! \remark needs to be in sync with tab order
        enum SettingTab
        {
            SettingTabOverview,
            SettingTabServers,
            SettingTabGui,
            SettingTabNetwork,
            SettingTabHotkeys,
            SettingTabAudio,
            SettingTabDataAndCaches,
            SettingTabSimulator,
            SettingTabSimulatorBasics,
            SettingTabSimulatorMessages,
            SettingTabMatching,
            SettingTabAdvanced
        };

        //! Constructor
        explicit CSettingsComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsComponent();

        //! \copydoc CAudioNotificationComponent::playNotificationSounds
        bool playNotificationSounds() const;

        //! Settings for given simulator
        BlackMisc::Simulation::Settings::CSpecializedSimulatorSettings getSimulatorSettings(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

        //! Set the tab
        void setTab(SettingTab tab);

        //! Set the overview tab
        void setSettingsOverviewTab();

        //! GUI Opacity 0-100%
        void setGuiOpacity(double value);

    signals:
        //! Change the windows opacity 0..100
        void changedWindowsOpacity(int opacity);

        //! Update interval changed (ATC)
        void changedAtcStationsUpdateInterval(int seconds);

        //! Update interval changed (aircrafts)
        void changedAircraftUpdateInterval(int seconds);

        //! Update interval changed (users)
        void changedUsersUpdateInterval(int seconds);

    private:
        //! An overview button was clicked
        void onOverviewButtonClicked();

        //! Action triggered
        void onActionTriggered();

        //! Init actions
        void initActions();

        QScopedPointer<Ui::CSettingsComponent> ui;
    };
} // namespace

#endif // guard
