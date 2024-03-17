// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

        QScopedPointer<Ui::CSettingsComponent> ui;
    };
} // namespace

#endif // guard
