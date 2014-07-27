/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingscomponent.h"
#include "ui_settingscomponent.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackmisc/hwkeyboardkeylist.h"
#include "blackmisc/setaudio.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Hardware;

namespace BlackGui
{
    namespace Components
    {

        CSettingsComponent::CSettingsComponent(QWidget *parent) :
            QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CSettingsComponent),
            m_audioTestRunning(NoAudioTest)
        {
            ui->setupUi(this);
            this->ui->prb_SettingsAudioTestProgress->setVisible(false);
            this->m_timerAudioTests = new QTimer(this);
        }

        CSettingsComponent::~CSettingsComponent()
        {
            delete ui;
        }

        /*
         * Update own ICAO data from GUI
         */
        void CSettingsComponent::setOwnAircraftIcaoDataFromGui(CAircraftIcao &icao) const
        {
            icao.setAirlineDesignator(this->ui->le_SettingsIcaoAirlineDesignator->text());
            icao.setAircraftDesignator(this->ui->le_SettingsIcaoAircraftDesignator->text());
            icao.setAircraftCombinedType(this->ui->le_SettingsIcaoCombinedType->text());
        }

        void CSettingsComponent::setGuiOpacity(double value)
        {
            this->ui->hs_SettingsGuiOpacity->setValue(value);
        }

        bool CSettingsComponent::loginAsObserver() const
        {
            return this->ui->rb_SettingsLoginStealthMode->isChecked();
        }

        bool CSettingsComponent::loginStealth() const
        {
            return this->ui->rb_SettingsLoginStealthMode->isChecked();
        }

        bool CSettingsComponent::playNotificationSounds() const
        {
            return this->ui->cb_SettingsAudioPlayNotificationSounds->isChecked();
        }

        int CSettingsComponent::getAtcUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiAtcRefreshTime->value();
        }

        int CSettingsComponent::getAircraftUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiAircraftRefreshTime->value();
        }

        int CSettingsComponent::getUsersUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiUserRefreshTime->value();
        }

        QString CSettingsComponent::getOwnCallsignFromGui() const
        {
            return this->ui->le_SettingsAircraftCallsign->text();
        }

        /*
         * Reload settings
         */
        void CSettingsComponent::reloadSettings()
        {
            // local copy
            CSettingsNetwork nws = this->getIContextSettings()->getNetworkSettings();
            CSettingsAudio as = this->getIContextSettings()->getAudioSettings();

            // update servers
            this->ui->tvp_SettingsTnServers->setSelectedServer(nws.getCurrentTrafficNetworkServer());
            this->ui->tvp_SettingsTnServers->update(nws.getTrafficNetworkServers());

            // update hot keys
            this->ui->tvp_SettingsMiscHotkeys->update(this->getIContextSettings()->getHotkeys());

            // fake setting for sound notifications
            this->ui->cb_SettingsAudioPlayNotificationSounds->setChecked(true);
            this->ui->cb_SettingsAudioNotificationTextMessage->setChecked(as.getNotificationFlag(BlackSound::CNotificationSounds::NotificationTextMessagePrivate));
            this->ui->cb_SettingsAudioNotificationVoiceRoom->setChecked(as.getNotificationFlag(BlackSound::CNotificationSounds::NotificationVoiceRoomJoined));
        }

        void CSettingsComponent::runtimeHasBeenSet()
        {
            if (!this->getIContextSettings()) qFatal("Settings missing");

            this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CSettingsComponent::changedSettings);
            this->connect(this->m_timerAudioTests, &QTimer::timeout, this, &CSettingsComponent::audioTestUpdate);

            // based on audio context
            Q_ASSERT(this->getIContextAudio());
            if (this->getIContextAudio())
            {
                this->initAudioDeviceLists();
                bool connected = this->connect(this->getIContextAudio(), &IContextAudio::audioTestCompleted, this, &CSettingsComponent::audioTestUpdate);
                Q_ASSERT(connected);
                connected = this->connect(this->ui->cb_SettingsAudioInputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(audioDeviceSelected(int)));
                Q_ASSERT(connected);
                connected = this->connect(this->ui->cb_SettingsAudioOutputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(audioDeviceSelected(int)));
                Q_ASSERT(connected);
                this->connect(this->ui->pb_SettingsAudioMicrophoneTest, &QPushButton::clicked, this, &CSettingsComponent::startAudioTest);
                this->connect(this->ui->pb_SettingsAudioSquelchTest, &QPushButton::clicked, this, &CSettingsComponent::startAudioTest);
            }

            // Opacity, intervals
            this->connect(this->ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &CSettingsComponent::changedWindowsOpacity);
            this->connect(this->ui->hs_SettingsGuiAircraftRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAircraftsUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiAtcRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAtcStationsUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiUserRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedUsersUpdateInterval);

            // Settings server
            this->connect(this->ui->pb_SettingsTnCurrentServer, &QPushButton::released, this, &CSettingsComponent::alterTrafficServer);
            this->connect(this->ui->pb_SettingsTnRemoveServer, &QPushButton::released, this, &CSettingsComponent::alterTrafficServer);
            this->connect(this->ui->pb_SettingsTnSaveServer, &QPushButton::released, this, &CSettingsComponent::alterTrafficServer);
            this->connect(this->ui->tvp_SettingsTnServers, &QTableView::clicked, this, &CSettingsComponent::networkServerSelected);

            // Settings hotkeys
            this->connect(this->ui->pb_SettingsMiscCancel, &QPushButton::clicked, this, &CSettingsComponent::reloadSettings);
            this->connect(this->ui->pb_SettingsMiscSave, &QPushButton::clicked, this, &CSettingsComponent::saveHotkeys);
            this->connect(this->ui->pb_SettingsMiscRemove, &QPushButton::clicked, this, &CSettingsComponent::clearHotkey);
        }

        /*
         * Network has been selected
         */
        void CSettingsComponent::networkServerSelected(QModelIndex index)
        {
            const CServer clickedServer = this->ui->tvp_SettingsTnServers->at<CServer>(index);
            this->updateGuiSelectedServerTextboxes(clickedServer);
        }

        /*
         * Alter server
         */
        void CSettingsComponent::alterTrafficServer()
        {
            CServer server = this->selectedServerFromTextboxes();
            if (!server.isValidForLogin())
            {
                const CStatusMessage validation = CStatusMessage::getValidationError("Wrong settings for server");
                this->sendStatusMessage(validation);
                return;
            }

            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathNetworkSettings(), CSettingsNetwork::ValueTrafficServers());
            QObject *sender = QObject::sender();
            CStatusMessageList msgs;
            if (sender == this->ui->pb_SettingsTnCurrentServer)
            {
                msgs = this->getIContextSettings()->value(path, CSettingsNetwork::CmdSetCurrentServer(), server.toQVariant());
            }
            else if (sender == this->ui->pb_SettingsTnRemoveServer)
            {
                msgs = this->getIContextSettings()->value(path, CSettingUtilities::CmdRemove(), server.toQVariant());
            }
            else if (sender == this->ui->pb_SettingsTnSaveServer)
            {
                msgs = this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), server.toQVariant());
            }

            // status messages
            this->sendStatusMessages(msgs);
        }

        /*
         * Settings did changed
         */
        void CSettingsComponent::changedSettings(uint typeValue)
        {
            IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
            this->reloadSettings();
            Q_UNUSED(type);
        }

        /*
         * Textboxes from server
         */
        void CSettingsComponent::updateGuiSelectedServerTextboxes(const CServer &server)
        {
            this->ui->le_SettingsTnCsName->setText(server.getName());
            this->ui->le_SettingsTnCsDescription->setText(server.getDescription());
            this->ui->le_SettingsTnCsAddress->setText(server.getAddress());
            this->ui->le_SettingsTnCsPort->setText(QString::number(server.getPort()));
            this->ui->le_SettingsTnCsRealName->setText(server.getUser().getRealName());
            this->ui->le_SettingsTnCsNetworkId->setText(server.getUser().getId());
            this->ui->le_SettingsTnCsPassword->setText(server.getUser().getPassword());
        }

        /*
         * Server settings from textboxes
         */
        CServer CSettingsComponent::selectedServerFromTextboxes() const
        {
            CServer server;
            bool portOk = false;
            server.setName(this->ui->le_SettingsTnCsName->text());
            server.setDescription(this->ui->le_SettingsTnCsDescription->text());
            server.setAddress(this->ui->le_SettingsTnCsAddress->text());
            server.setPort(this->ui->le_SettingsTnCsPort->text().toInt(&portOk));
            if (!portOk) server.setPort(-1);

            CUser user;
            user.setRealName(this->ui->le_SettingsTnCsRealName->text());
            user.setId(this->ui->le_SettingsTnCsNetworkId->text());
            user.setPassword(this->ui->le_SettingsTnCsPassword->text());
            server.setUser(user);

            return server;
        }

        /*
         * Save the hotkeys
         */
        void CSettingsComponent::saveHotkeys()
        {
            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathRoot(), IContextSettings::PathHotkeys());
            CStatusMessageList msgs = this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), this->ui->tvp_SettingsMiscHotkeys->derivedModel()->getContainer().toQVariant());

            // status messages
            this->sendStatusMessages(msgs);
        }

        /*
         * Clear particular hotkey
         */
        void CSettingsComponent::clearHotkey()
        {
            QModelIndex i = this->ui->tvp_SettingsMiscHotkeys->currentIndex();
            if (i.row() < 0 || i.row() >= this->ui->tvp_SettingsMiscHotkeys->rowCount()) return;
            BlackMisc::Hardware::CKeyboardKey key = this->ui->tvp_SettingsMiscHotkeys->at<BlackMisc::Hardware::CKeyboardKey>(i);
            BlackMisc::Hardware::CKeyboardKey defaultKey;
            defaultKey.setFunction(key.getFunction());
            this->ui->tvp_SettingsMiscHotkeys->derivedModel()->update(i, defaultKey);
        }

        /*
         * Set audio device lists
         */
        void CSettingsComponent::initAudioDeviceLists()
        {
            if (!this->getIContextAudio()) return;
            this->ui->cb_SettingsAudioOutputDevice->clear();
            this->ui->cb_SettingsAudioInputDevice->clear();

            foreach(CAudioDevice device, this->getIContextAudio()->getAudioDevices())
            {
                if (device.getType() == CAudioDevice::InputDevice)
                {
                    this->ui->cb_SettingsAudioInputDevice->addItem(device.toQString(true));
                }
                else if (device.getType() == CAudioDevice::OutputDevice)
                {
                    this->ui->cb_SettingsAudioOutputDevice->addItem(device.toQString(true));
                }
            }

            foreach(CAudioDevice device, this->getIContextAudio()->getCurrentAudioDevices())
            {
                if (device.getType() == CAudioDevice::InputDevice)
                {
                    this->ui->cb_SettingsAudioInputDevice->setCurrentText(device.toQString(true));
                }
                else if (device.getType() == CAudioDevice::OutputDevice)
                {
                    this->ui->cb_SettingsAudioOutputDevice->setCurrentText(device.toQString(true));
                }
            }
        }

        /*
         * Start the voice tests
         */
        void CSettingsComponent::startAudioTest()
        {
            if (!this->getIContextAudio())
            {
                CStatusMessage m(CStatusMessage::TypeAudio, CStatusMessage::SeverityError, "voice context not available");
                this->sendStatusMessage(m);
                return;
            }
            if (this->m_timerAudioTests->isActive())
            {
                CStatusMessage m(CStatusMessage::TypeAudio, CStatusMessage::SeverityError, "test running, wait until completed");
                this->sendStatusMessage(m);
                return;
            }

            QObject *sender = QObject::sender();
            this->m_timerAudioTests->start(600); // I let this run for <x>ms, so there is enough overhead to really complete it
            this->ui->prb_SettingsAudioTestProgress->setValue(0);
            this->ui->pte_SettingsAudioTestActionAndResult->clear();
            if (sender == this->ui->pb_SettingsAudioMicrophoneTest)
            {
                this->m_audioTestRunning = MicrophoneTest;
                this->getIContextAudio()->runMicrophoneTest();
                this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText("Speak normally for 5 seconds");
            }
            else if (sender == this->ui->pb_SettingsAudioSquelchTest)
            {
                this->m_audioTestRunning = SquelchTest;
                this->getIContextAudio()->runSquelchTest();
                this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText("Silence for 5 seconds");
            }
            this->ui->prb_SettingsAudioTestProgress->setVisible(true);
            this->ui->pb_SettingsAudioMicrophoneTest->setEnabled(false);
            this->ui->pb_SettingsAudioSquelchTest->setEnabled(false);
        }

        /*
         * Start the voice tests
         */
        void CSettingsComponent::audioTestUpdate()
        {
            Q_ASSERT(this->getIContextAudio());
            if (!this->getIContextAudio()) return;
            int v = this->ui->prb_SettingsAudioTestProgress->value();
            QObject *sender = this->sender();

            if (v < 100 && (sender == m_timerAudioTests))
            {
                // timer update, increasing progress
                this->ui->prb_SettingsAudioTestProgress->setValue(v + 10);
            }
            else
            {
                this->m_timerAudioTests->stop();
                this->ui->prb_SettingsAudioTestProgress->setValue(100);
                if (sender == m_timerAudioTests) return; // just timer update

                // getting here we assume the audio test finished signal
                // fetch results
                this->ui->pte_SettingsAudioTestActionAndResult->clear();
                if (this->m_audioTestRunning == SquelchTest)
                {
                    double s = this->getIContextAudio()->getSquelchValue();
                    this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText(QString::number(s));
                }
                else if (this->m_audioTestRunning == MicrophoneTest)
                {
                    QString m = this->getIContextAudio()->getMicrophoneTestResult();
                    this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText(m);
                }
                this->m_audioTestRunning = NoAudioTest;
                this->m_timerAudioTests->stop();
                this->ui->pb_SettingsAudioMicrophoneTest->setEnabled(true);
                this->ui->pb_SettingsAudioSquelchTest->setEnabled(true);
                this->ui->prb_SettingsAudioTestProgress->setVisible(false);
            }
        }

        /*
         * Select audio device
         */
        void CSettingsComponent::audioDeviceSelected(int index)
        {
            if (!this->getIContextAudio()) return;
            if (index < 0)return;

            CAudioDeviceList devices = this->getIContextAudio()->getAudioDevices();
            if (devices.isEmpty()) return;
            CAudioDevice selectedDevice;
            QObject *sender = QObject::sender();
            if (sender == this->ui->cb_SettingsAudioInputDevice)
            {
                CAudioDeviceList inputDevices = devices.getInputDevices();
                if (index >= inputDevices.size()) return;
                selectedDevice = inputDevices[index];
                this->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
            else if (sender == this->ui->cb_SettingsAudioOutputDevice)
            {
                CAudioDeviceList outputDevices = devices.getOutputDevices();
                if (index >= outputDevices.size()) return;
                selectedDevice = outputDevices[index];
                this->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
        }
    }
} // namespace
