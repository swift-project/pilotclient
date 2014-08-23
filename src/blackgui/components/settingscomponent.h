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

#include "blackgui/components/runtimebasedcomponent.h"
#include <QTabWidget>
#include <QModelIndex>
#include <QTimer>

namespace Ui { class CSettingsComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! Settings component
        class CSettingsComponent : public QTabWidget, public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:

            //! Tabs
            enum SettingTab
            {
                SettingTabNetwork = 0,
                SettingTabAircraft,
                SettingTabAircraftAudio,
                SettingTabSimulator,
                SettingTabGui,
                SettingTabMisc
            };

            //! Constructor
            explicit CSettingsComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsComponent();

            //! ICAO data from GUI
            void setOwnAircraftIcaoDataFromGui(BlackMisc::Aviation::CAircraftIcao &icao) const;

            //! GUI Opacity 0-100%
            void setGuiOpacity(double value);

            //! Login as observer
            bool loginAsObserver() const;

            //! Login as observer
            bool loginStealth() const;

            //! Play notification sounds (at all)
            bool playNotificationSounds() const;

            //! ATC refresh time
            int getAtcUpdateIntervalSeconds() const;

            //! Aircraft refresh time
            int getAircraftUpdateIntervalSeconds() const;

            //! Aircraft refresh time
            int getUsersUpdateIntervalSeconds() const;

            //! Own callsign
            QString getOwnCallsignFromGui() const;

        signals:
            //! Change the windows opacity 0..100
            void changedWindowsOpacity(int opacity);

            //! Update interval changed (ATC)
            void changedAtcStationsUpdateInterval(int seconds);

            //! Update interval changed (aircrafts)
            void changedAircraftsUpdateInterval(int seconds);

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

            //! Network server selected
            void ps_networkServerSelected(QModelIndex index);

            //! Alter traffic server
            void ps_alterTrafficServer();

            /*!
             * \brief Update the selected server textboxes
             * \param server to be displayed
             */
            void ps_updateGuiSelectedServerTextboxes(const BlackMisc::Network::CServer &server);

            //! Selected server from textboxes
            BlackMisc::Network::CServer ps_selectedServerFromTextboxes() const;

            //! Settings have been changed
            void ps_changedSettings(uint typeValue);

            //! Save the Hotkeys
            void ps_saveHotkeys();

            //! Clear single hotkey
            void ps_clearHotkey();

            //! start the MIC tests (Squelch)
            void ps_startAudioTest();

            //! Audio test updates (timer) for progressbar and fetching results
            void ps_audioTestUpdate();

            /*!
             * \brief Audio device selected
             * \param index audio device index (COM1, COM2)
             */
            void ps_audioDeviceSelected(int index);

            //! Font has been changed
            void ps_fontChanged();

            //! Font color dialof
            void ps_fontColorDialog();

        private:
            //! Audio test modes
            enum AudioTest
            {
                NoAudioTest,
                SquelchTest,
                MicrophoneTest
            };

            Ui::CSettingsComponent *ui;
            QTimer *m_timerAudioTests; //!< audio tests: progress bar, disable/enable buttons
            AudioTest m_audioTestRunning;
            QColor m_fontColor;

            //! Audio device lists from settings
            void initAudioDeviceLists();

        };
    }
} // namespace

#endif // guard
