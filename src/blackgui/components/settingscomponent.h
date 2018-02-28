/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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

namespace Ui { class CSettingsComponent; }
namespace BlackGui
{
    namespace Components
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
                SettingTabNetwork,
                SettingTabAudio,
                SettingTabSimulator,
                SettingTabSimulatorBasics,
                SettingTabSimulatorMessages,
                SettingTabGui,
                SettingTabHotkeys,
                SettingTabDataAndCaches,
                SettingTabAdvanced
            };

            //! Constructor
            explicit CSettingsComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CSettingsComponent();

            //! \copydoc CAudioSetupComponent::playNotificationSounds
            bool playNotificationSounds() const;

            //! Settings for given simulator
            BlackMisc::Simulation::Settings::CSpecializedSimulatorSettings getSimulatorSettings(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

        signals:
            //! Change the windows opacity 0..100
            void changedWindowsOpacity(int opacity);

            //! Update interval changed (ATC)
            void changedAtcStationsUpdateInterval(int seconds);

            //! Update interval changed (aircrafts)
            void changedAircraftUpdateInterval(int seconds);

            //! Update interval changed (users)
            void changedUsersUpdateInterval(int seconds);

        public slots:
            //! Set the tab
            void setSettingsTab(SettingTab tab);

            //! GUI Opacity 0-100%
            void setGuiOpacity(double value);

        private slots:
            //! An overview button was clicked
            void ps_overviewButtonClicked();

            //! Action triggered
            void ps_actionTriggered();

        private:
            //! Init actions
            void initActions();

            QScopedPointer<Ui::CSettingsComponent> ui;
        };
    }
} // namespace

#endif // guard
