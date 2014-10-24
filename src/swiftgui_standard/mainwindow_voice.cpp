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
 * Notification
 */
void MainWindow::playNotifcationSound(CNotificationSounds::Notification notification) const
{
    if (!this->m_contextAudioAvailable) return;
    if (!this->ui->comp_MainInfoArea->getSettingsComponent()->playNotificationSounds()) return;
    this->getIContextAudio()->playNotification(static_cast<uint>(notification), true);
}
