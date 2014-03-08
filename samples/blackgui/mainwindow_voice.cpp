#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackcore/context_audio.h"
#include "blacksound/soundgenerator.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackSound;
using namespace BlackMisc::Network;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Math;

/*
 * Set audio device lists
 */
void MainWindow::setAudioDeviceLists()
{
    if (!this->isContextAudioAvailableCheck()) return;
    this->ui->cb_SettingsAudioOutputDevice->clear();
    this->ui->cb_SettingsAudioInputDevice->clear();

    foreach(CAudioDevice device, this->m_contextAudio->getAudioDevices())
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

    foreach(CAudioDevice device, this->m_contextAudio->getCurrentAudioDevices())
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
 * Select audio device
 */
void MainWindow::audioDeviceSelected(int index)
{
    if (!this->m_init) return; // not during init
    if (!this->isContextAudioAvailableCheck()) return;
    if (index < 0)return;
    CAudioDeviceList devices = this->m_contextAudio->getAudioDevices();
    if (devices.isEmpty()) return;
    CAudioDevice selectedDevice;
    QObject *sender = QObject::sender();
    if (sender == this->ui->cb_SettingsAudioInputDevice)
    {
        CAudioDeviceList inputDevices = devices.getInputDevices();
        if (index >= inputDevices.size()) return;
        selectedDevice = inputDevices[index];
        this->m_contextAudio->setCurrentAudioDevice(selectedDevice);
    }
    else if (sender == this->ui->cb_SettingsAudioOutputDevice)
    {
        CAudioDeviceList outputDevices = devices.getOutputDevices();
        if (index >= outputDevices.size()) return;
        selectedDevice = outputDevices[index];
        this->m_contextAudio->setCurrentAudioDevice(selectedDevice);
    }
}

/*
 * Select audio device
 */
void MainWindow::audioVolumes()
{
    if (!this->m_contextAudioAvailable)
    {
        this->ui->pb_SoundMute->setEnabled(false);
        this->ui->pb_SoundMaxVolume->setEnabled(false);
        return;
    }

    // enable the buttons, as we have a voice context
    this->ui->pb_SoundMute->setEnabled(true);
    this->ui->pb_SoundMaxVolume->setEnabled(true);
    QObject *sender = QObject::sender();

    CComSystem com1 = this->m_ownAircraft.getCom1System();
    CComSystem com2 = this->m_ownAircraft.getCom2System();
    bool muted;

    if (sender == this->ui->pb_SoundMute)
    {
        if (this->m_contextAudio->isMuted())
        {
            // muted right now, now unmute
            muted = false;
        }
        else
        {
            // unmuted right now, now mute
            muted = true;
        }
    }
    else if (sender == this->ui->pb_SoundMaxVolume)
    {
        muted = false;
        com1.setVolumeOutput(100);
        com2.setVolumeOutput(100);
        this->ui->di_CockpitCom1Volume->setValue(100);
        this->ui->di_CockpitCom2Volume->setValue(100);
    }
    else if (sender == this->ui->di_CockpitCom1Volume ||
             sender == this->ui->di_CockpitCom2Volume)
    {
        muted = false;
        com1.setVolumeOutput(this->ui->di_CockpitCom1Volume->value());
        com2.setVolumeOutput(this->ui->di_CockpitCom2Volume->value());
    }
    else
    {
        return;
    }

    // mute / unmute
    com1.setEnabled(!muted);
    com2.setEnabled(!muted);
    this->ui->pb_SoundMute->setText(muted ? "Unmute" : "Mute");
    this->ui->lbl_StatusVoiceStatus->setPixmap(muted ? this->m_resPixmapVoiceMuted : this->m_resPixmapVoiceHigh);
    this->ui->lbl_CockpitVoiceStatus->setPixmap(muted ? this->m_resPixmapVoiceMuted : this->m_resPixmapVoiceHigh);
    this->ui->pb_SoundMute->setStyleSheet(muted ? "background-color: red;" : "");
    if (muted) this->displayOverlayInfo("Sound is muted!");

    // update own aircraft, also set volume/mute in voice
    this->m_ownAircraft.setCom1System(com1);
    this->m_ownAircraft.setCom2System(com2);
    this->m_contextAudio->setVolumes(this->m_ownAircraft.getCom1System(), this->m_ownAircraft.getCom2System());
}

/*
 * Start the voice tests
 */
void MainWindow::startAudioTest()
{
    if (!this->m_contextAudioAvailable)
    {
        CStatusMessage m(CStatusMessage::TypeAudio, CStatusMessage::SeverityError, "voice context not available");
        this->displayStatusMessage(m);
        return;
    }
    if (this->m_timerAudioTests->isActive())
    {
        CStatusMessage m(CStatusMessage::TypeAudio, CStatusMessage::SeverityError, "test running, wait until completed");
        this->displayStatusMessage(m);
        return;
    }

    QObject *sender = QObject::sender();
    this->m_timerAudioTests->start(600); // I let this run for <x>ms, so there is enough overhead to really complete it
    this->ui->prb_SettingsAudioTestProgress->setValue(0);
    this->ui->pte_SettingsAudioTestActionAndResult->clear();
    if (sender == this->ui->pb_SettingsAudioMicrophoneTest)
    {
        this->m_audioTestRunning = MicrophoneTest;
        this->m_contextAudio->runMicrophoneTest();
        this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText("Speak normally for 5 seconds");
    }
    else if (sender == this->ui->pb_SettingsAudioSquelchTest)
    {
        this->m_audioTestRunning = SquelchTest;
        this->m_contextAudio->runSquelchTest();
        this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText("Silence for 5 seconds");
    }
    this->ui->prb_SettingsAudioTestProgress->setVisible(true);
    this->ui->pb_SettingsAudioMicrophoneTest->setEnabled(false);
    this->ui->pb_SettingsAudioSquelchTest->setEnabled(false);
}

/*
 * Start the voice tests
 */
void MainWindow::audioTestUpdate()
{
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
        if (this->m_contextAudioAvailable)
        {
            if (this->m_audioTestRunning == SquelchTest)
            {
                double s = this->m_contextAudio->getSquelchValue();
                this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText(QString::number(s));
            }
            else if (this->m_audioTestRunning == MicrophoneTest)
            {
                QString m = this->m_contextAudio->getMicrophoneTestResult();
                this->ui->pte_SettingsAudioTestActionAndResult->appendPlainText(m);
            }
        }
        this->m_audioTestRunning = NoAudioTest;
        this->m_timerAudioTests->stop();
        this->ui->pb_SettingsAudioMicrophoneTest->setEnabled(true);
        this->ui->pb_SettingsAudioSquelchTest->setEnabled(true);
        this->ui->prb_SettingsAudioTestProgress->setVisible(false);
    }
}

/*
 * Notification
 */
void MainWindow::playNotifcationSound(CSoundGenerator::Notification notification) const
{
    if (!this->m_contextAudioAvailable) return;
    if (!this->ui->cb_SettingsAudioPlayNotificationSounds->isChecked()) return;
    if (notification == CSoundGenerator::NotificationTextMessage && !this->ui->cb_SettingsAudioNotificationTextMessage->isChecked()) return;
    this->m_contextAudio->playNotification(static_cast<uint>(notification));
}
