// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSCOMPONENT_H

#include <QColor>
#include <QObject>
#include <QScopedPointer>
#include <QTabWidget>

#include "gui/swiftguiexport.h"
#include "misc/simulation/settings/simulatorsettings.h"

class QWidget;

namespace Ui
{
    class CSettingsComponent;
}
namespace swift::gui::components
{
    //! Settings component
    class SWIFT_GUI_EXPORT CSettingsComponent : public QTabWidget
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
        ~CSettingsComponent() override;

        //! Settings for given simulator
        swift::misc::simulation::settings::CSpecializedSimulatorSettings
        getSimulatorSettings(const swift::misc::simulation::CSimulatorInfo &simulator) const;

        //! Set the tab
        void setTab(SettingTab tab);

        //! Set the overview tab
        void setSettingsOverviewTab();

        //! GUI Opacity 0-100%
        void setGuiOpacity(double value);

    signals:
        //! Change the windows opacity 0..100
        void changedWindowsOpacity(int opacity);

    private:
        //! An overview button was clicked
        void onOverviewButtonClicked();

        QScopedPointer<Ui::CSettingsComponent> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_SETTINGSCOMPONENT_H
