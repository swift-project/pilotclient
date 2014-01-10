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
    this->ui->cb_VoiceOutputDevice->clear();
    this->ui->cb_VoiceInputDevice->clear();

    foreach(CAudioDevice device, this->m_contextVoice->getAudioDevices())
    {
        if (device.getType() == CAudioDevice::InputDevice)
        {
            this->ui->cb_VoiceInputDevice->addItem(device.toQString(true));
        }
        else if (device.getType() == CAudioDevice::OutputDevice)
        {
            this->ui->cb_VoiceOutputDevice->addItem(device.toQString(true));
        }
    }

    foreach(CAudioDevice device, this->m_contextVoice->getCurrentAudioDevices())
    {
        if (device.getType() == CAudioDevice::InputDevice)
        {
            this->ui->cb_VoiceInputDevice->setCurrentText(device.toQString(true));
        }
        else if (device.getType() == CAudioDevice::OutputDevice)
        {
            this->ui->cb_VoiceOutputDevice->setCurrentText(device.toQString(true));
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
    if (sender == this->ui->cb_VoiceInputDevice)
    {
        CAudioDeviceList inputDevices = devices.getInputDevices();
        if (index >= inputDevices.size()) return;
        selectedDevice = inputDevices[index];
        this->m_contextVoice->setCurrentAudioDevice(selectedDevice);
    }
    else if (sender == this->ui->cb_VoiceOutputDevice)
    {
        CAudioDeviceList outputDevices = devices.getOutputDevices();
        if (index >= outputDevices.size()) return;
        selectedDevice = outputDevices[index];
        this->m_contextVoice->setCurrentAudioDevice(selectedDevice);
    }
}
