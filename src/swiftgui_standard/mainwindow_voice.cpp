#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackmisc/icons.h"
#include "blackgui/models/atcstationlistmodel.h"
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
 * Select audio device
 */
void MainWindow::ps_setAudioVolumes()
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
        if (this->getIContextAudio()->isMuted())
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
        this->ui->comp_Cockpit->setCom1Volume(100);
        this->ui->comp_Cockpit->setCom2Volume(100);
    }
    else if (this->ui->comp_Cockpit->isCockpitVolumeWidget(sender))
    {
        muted = false;
        com1.setVolumeOutput(this->ui->comp_Cockpit->getCom1Volume());
        com2.setVolumeOutput(this->ui->comp_Cockpit->getCom2Volume());
    }
    else
    {
        return;
    }

    // mute / unmute
    com1.setEnabled(!muted);
    com2.setEnabled(!muted);
    this->ui->pb_SoundMute->setText(muted ? "Unmute" : "Mute");
    this->ui->comp_Cockpit->setCockpitVoiceStatusPixmap(muted ? CIcons::volumeMuted16() : CIcons::volumeHigh16());
    this->ui->pb_SoundMute->setStyleSheet(muted ? "background-color: red;" : "");
    if (muted) this->m_compInfoWindow->displayStringMessage("Sound is muted!");

    // update own aircraft, also set volume/mute in voice
    this->m_ownAircraft.setCom1System(com1);
    this->m_ownAircraft.setCom2System(com2);
    this->getIContextAudio()->setVolumes(this->m_ownAircraft.getCom1System(), this->m_ownAircraft.getCom2System());
}

/*
 * Notification
 */
void MainWindow::playNotifcationSound(CNotificationSounds::Notification notification) const
{
    if (!this->m_contextAudioAvailable) return;
    if (!this->ui->comp_MainInfoArea->getSettingsComponent()->playNotificationSounds()) return;
    this->getIContextAudio()->playNotification(static_cast<uint>(notification), true);
}
