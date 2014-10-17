/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "audiosetup.h"
#include "ui_audiosetup.h"
#include "blackmisc/setaudio.h"
#include "blackmisc/logmessage.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Settings;

namespace BlackGui
{
    namespace Components
    {
        CAudioSetup::CAudioSetup(QWidget *parent) :
            QFrame(parent),
            CEnableForRuntime(nullptr, false),
            ui(new Ui::CAudioSetup)
        {
            ui->setupUi(this);
            this->ui->prb_SetupAudioTestProgress->hide();
            this->m_timerAudioTests = new QTimer(this);
        }

        CAudioSetup::~CAudioSetup()
        { }

        /*
         * Runtime set
         */
        void CAudioSetup::runtimeHasBeenSet()
        {
            if (!this->getIContextSettings()) qFatal("Settings missing");
            this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CAudioSetup::ps_changedSettings);
            this->connect(this->m_timerAudioTests, &QTimer::timeout, this, &CAudioSetup::ps_audioTestUpdate);

            // based on audio context
            Q_ASSERT(this->getIContextAudio());
            bool connected = false;
            if (this->getIContextAudio())
            {
                this->initAudioDeviceLists();
                connected = this->connect(this->getIContextAudio(), &IContextAudio::audioTestCompleted, this, &CAudioSetup::ps_audioTestUpdate);
                Q_ASSERT(connected);
                connected = this->connect(this->ui->cb_SetupAudioInputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(ps_audioDeviceSelected(int)));
                Q_ASSERT(connected);
                connected = this->connect(this->ui->cb_SetupAudioOutputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(ps_audioDeviceSelected(int)));
                Q_ASSERT(connected);
                this->connect(this->ui->pb_SetupAudioMicrophoneTest, &QPushButton::clicked, this, &CAudioSetup::ps_startAudioTest);
                this->connect(this->ui->pb_SetupAudioSquelchTest, &QPushButton::clicked, this, &CAudioSetup::ps_startAudioTest);
            }
            this->reloadSettings();
        }

        void CAudioSetup::ps_changedSettings(uint typeValue)
        {
            IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
            this->reloadSettings();
            Q_UNUSED(type);
        }

        /*
         * Reload settings
         */
        void CAudioSetup::reloadSettings()
        {
            // local copy
            CSettingsAudio as = this->getIContextSettings()->getAudioSettings();

            // fake setting for sound notifications
            this->ui->cb_SetupAudioPlayNotificationSounds->setChecked(true);
            this->ui->cb_SetupAudioNotificationTextMessage->setChecked(as.getNotificationFlag(BlackSound::CNotificationSounds::NotificationTextMessagePrivate));
            this->ui->cb_SetupAudioNotificationVoiceRoom->setChecked(as.getNotificationFlag(BlackSound::CNotificationSounds::NotificationVoiceRoomJoined));
        }

        /*
         * Set audio device lists
         */
        void CAudioSetup::initAudioDeviceLists()
        {
            if (!this->getIContextAudio()) return;
            this->ui->cb_SetupAudioOutputDevice->clear();
            this->ui->cb_SetupAudioInputDevice->clear();

            foreach(CAudioDevice device, this->getIContextAudio()->getAudioDevices())
            {
                if (device.getType() == CAudioDevice::InputDevice)
                {
                    this->ui->cb_SetupAudioInputDevice->addItem(device.toQString(true));
                }
                else if (device.getType() == CAudioDevice::OutputDevice)
                {
                    this->ui->cb_SetupAudioOutputDevice->addItem(device.toQString(true));
                }
            }

            foreach(CAudioDevice device, this->getIContextAudio()->getCurrentAudioDevices())
            {
                if (device.getType() == CAudioDevice::InputDevice)
                {
                    this->ui->cb_SetupAudioInputDevice->setCurrentText(device.toQString(true));
                }
                else if (device.getType() == CAudioDevice::OutputDevice)
                {
                    this->ui->cb_SetupAudioOutputDevice->setCurrentText(device.toQString(true));
                }
            }
        }

        /*
         * Notification sounds
         */
        bool CAudioSetup::playNotificationSounds() const
        {
            return this->ui->cb_SetupAudioPlayNotificationSounds->isChecked();
        }

        /*
         * Start the voice tests
         */
        void CAudioSetup::ps_startAudioTest()
        {
            if (!this->getIContextAudio())
            {
                CLogMessage(this).error("voice context not available");
                return;
            }
            if (this->m_timerAudioTests->isActive())
            {
                CLogMessage(this).error("test running, wait until completed");
                return;
            }

            QObject *sender = QObject::sender();
            this->m_timerAudioTests->start(600); // I let this run for <x>ms, so there is enough overhead to really complete it
            this->ui->prb_SetupAudioTestProgress->setValue(0);
            this->ui->pte_SetupAudioTestActionAndResult->clear();
            if (sender == this->ui->pb_SetupAudioMicrophoneTest)
            {
                this->m_audioTestRunning = MicrophoneTest;
                this->getIContextAudio()->runMicrophoneTest();
                this->ui->pte_SetupAudioTestActionAndResult->appendPlainText("Speak normally for 5 seconds");
            }
            else if (sender == this->ui->pb_SetupAudioSquelchTest)
            {
                this->m_audioTestRunning = SquelchTest;
                this->getIContextAudio()->runSquelchTest();
                this->ui->pte_SetupAudioTestActionAndResult->appendPlainText("Silence for 5 seconds");
            }
            this->ui->prb_SetupAudioTestProgress->setVisible(true);
            this->ui->pb_SetupAudioMicrophoneTest->setEnabled(false);
            this->ui->pb_SetupAudioSquelchTest->setEnabled(false);
        }

        /*
         * Start the voice tests
         */
        void CAudioSetup::ps_audioTestUpdate()
        {
            Q_ASSERT(this->getIContextAudio());
            if (!this->getIContextAudio()) return;
            int v = this->ui->prb_SetupAudioTestProgress->value();
            QObject *sender = this->sender();

            if (v < 100 && (sender == m_timerAudioTests))
            {
                // timer update, increasing progress
                this->ui->prb_SetupAudioTestProgress->setValue(v + 10);
            }
            else
            {
                this->m_timerAudioTests->stop();
                this->ui->prb_SetupAudioTestProgress->setValue(100);
                if (sender == m_timerAudioTests) return; // just timer update

                // getting here we assume the audio test finished signal
                // fetch results
                this->ui->pte_SetupAudioTestActionAndResult->clear();
                if (this->m_audioTestRunning == SquelchTest)
                {
                    double s = this->getIContextAudio()->getSquelchValue();
                    this->ui->pte_SetupAudioTestActionAndResult->appendPlainText(QString::number(s));
                }
                else if (this->m_audioTestRunning == MicrophoneTest)
                {
                    QString m = this->getIContextAudio()->getMicrophoneTestResult();
                    this->ui->pte_SetupAudioTestActionAndResult->appendPlainText(m);
                }
                this->m_audioTestRunning = NoAudioTest;
                this->m_timerAudioTests->stop();
                this->ui->pb_SetupAudioMicrophoneTest->setEnabled(true);
                this->ui->pb_SetupAudioSquelchTest->setEnabled(true);
                this->ui->prb_SetupAudioTestProgress->setVisible(false);
            }
        }

        /*
         * Select audio device
         */
        void CAudioSetup::ps_audioDeviceSelected(int index)
        {
            if (!this->getIContextAudio()) return;
            if (index < 0)return;

            CAudioDeviceList devices = this->getIContextAudio()->getAudioDevices();
            if (devices.isEmpty()) return;
            CAudioDevice selectedDevice;
            QObject *sender = QObject::sender();
            if (sender == this->ui->cb_SetupAudioInputDevice)
            {
                CAudioDeviceList inputDevices = devices.getInputDevices();
                if (index >= inputDevices.size()) return;
                selectedDevice = inputDevices[index];
                this->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
            else if (sender == this->ui->cb_SetupAudioOutputDevice)
            {
                CAudioDeviceList outputDevices = devices.getOutputDevices();
                if (index >= outputDevices.size()) return;
                selectedDevice = outputDevices[index];
                this->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
        }

    } // namespace
} // namespace

