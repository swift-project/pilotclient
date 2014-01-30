#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackcore/context_voice.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Voice;
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
    if (!this->isContextVoiceAvailableCheck()) return;
    this->ui->cb_SettingsAudioOutputDevice->clear();
    this->ui->cb_SettingsAudioInputDevice->clear();

    foreach(CAudioDevice device, this->m_contextVoice->getAudioDevices())
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

    foreach(CAudioDevice device, this->m_contextVoice->getCurrentAudioDevices())
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
    if (!this->isContextVoiceAvailableCheck()) return;
    if (index < 0)return;
    CAudioDeviceList devices = this->m_contextVoice->getAudioDevices();
    if (devices.isEmpty()) return;
    CAudioDevice selectedDevice;
    QObject *sender = QObject::sender();
    if (sender == this->ui->cb_SettingsAudioInputDevice)
    {
        CAudioDeviceList inputDevices = devices.getInputDevices();
        if (index >= inputDevices.size()) return;
        selectedDevice = inputDevices[index];
        this->m_contextVoice->setCurrentAudioDevice(selectedDevice);
    }
    else if (sender == this->ui->cb_SettingsAudioOutputDevice)
    {
        CAudioDeviceList outputDevices = devices.getOutputDevices();
        if (index >= outputDevices.size()) return;
        selectedDevice = outputDevices[index];
        this->m_contextVoice->setCurrentAudioDevice(selectedDevice);
    }
}

/*
 * Select audio device
 */
void MainWindow::audioVolumes()
{
    if (!this->m_contextVoiceAvailable)
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
        if (this->m_contextVoice->isMuted())
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

    // mute / umute
    com1.setEnabled(!muted);
    com2.setEnabled(!muted);
    this->ui->pb_SoundMute->setText(muted ? "Unmute" : "Mute");
    this->ui->lbl_VoiceStatus->setPixmap(muted ? this->m_resPixmapVoiceMuted : this->m_resPixmapVoiceHigh);
    this->ui->pb_SoundMute->setStyleSheet(muted ? "background-color: red;" : "");
    if (muted) this->displayOverlayInfo("Sound is muted!");

    // update own aircraft, also set volume/mute in voice
    this->m_ownAircraft.setCom1System(com1);
    this->m_ownAircraft.setCom2System(com2);
    this->m_contextVoice->setVolumes(this->m_ownAircraft.getCom1System(), this->m_ownAircraft.getCom2System());
}
