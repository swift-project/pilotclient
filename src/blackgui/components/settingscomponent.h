/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGSCOMPONENT_H
#define BLACKGUI_SETTINGSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enableforruntime.h"
#include "blackmisc/network/server.h"
#include "blackmisc/aviation/aircrafticao.h"
#include <QTabWidget>
#include <QModelIndex>
#include <QTimer>
#include <QScopedPointer>

namespace Ui { class CSettingsComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! Settings component
        class BLACKGUI_EXPORT CSettingsComponent :
            public QTabWidget,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:

            //! Tabs
            enum SettingTab
            {
                SettingTabNetworkServers = 0,
                SettingTabNetwork,
                SettingTabAircraftAudio,
                SettingTabSimulator,
                SettingTabGui,
                SettingTabMisc
            };

            //! Constructor
            explicit CSettingsComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsComponent();

            //! \copydoc CAudioSetupComponent::playNotificationSounds
            bool playNotificationSounds() const;

            //! GUI Opacity 0-100%
            void setGuiOpacity(double value);

            //! ATC refresh time
            int getAtcUpdateIntervalSeconds() const;

            //! Aircraft refresh time
            int getAircraftUpdateIntervalSeconds() const;

            //! Aircraft refresh time
            int getUsersUpdateIntervalSeconds() const;

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
            //! Reload settings
            void reloadSettings();

            //! Set the tab
            void setSettingsTab(SettingTab tab);

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private slots:

            //! Settings have been changed
            void ps_changedSettings(uint typeValue);

            //! Save the Hotkeys
            void ps_saveHotkeys();

            //! Clear single hotkey
            void ps_clearHotkey();

            //! Font has been changed
            void ps_fontChanged();

            //! Font color dialof
            void ps_fontColorDialog();

        private:
            QScopedPointer<Ui::CSettingsComponent> ui;
            QColor m_fontColor;
        };
    }
} // namespace

#endif // guard
