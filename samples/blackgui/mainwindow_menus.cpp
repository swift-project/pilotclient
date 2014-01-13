#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avaltitude.h"
#include <QPoint>
#include <QMenu>
#include <QDesktopServices>
#include <QProcess>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

/*
 * Menu clicked
 */
void MainWindow::menuClicked()
{
    QObject *sender = QObject::sender();
    CStatusMessageList msgs;

    if (sender == this->ui->menu_TestLocationsEDRY)
    {
        this->setTestPosition("N 049° 18' 17", "E 008° 27' 05", CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
    }
    else if (sender == this->ui->menu_TestLocationsEDNX)
    {
        this->setTestPosition("N 048° 14′ 22", "E 011° 33′ 41", CAltitude(486, CAltitude::MeanSeaLevel, CLengthUnit::m()));
    }
    else if (sender == this->ui->menu_TestLocationsEDDM)
    {
        this->setTestPosition("N 048° 21′ 14", "E 011° 47′ 10", CAltitude(448, CAltitude::MeanSeaLevel, CLengthUnit::m()));
    }
    else if (sender == this->ui->menu_TestLocationsEDDF)
    {
        this->setTestPosition("N 50° 2′ 0", "E 8° 34′ 14", CAltitude(100, CAltitude::MeanSeaLevel, CLengthUnit::m()));
    }
    else if (sender == this->ui->menu_ReloadSettings)
    {
        this->reloadSettings();
        msgs.insert(CStatusMessage::getInfoMessage("Settings reloaded"));
    }
    else if (sender == this->ui->menu_FileClose)
    {
        msgs.insert(CStatusMessage::getInfoMessage("Closing"));
        this->close();
    }
    if (!msgs.isEmpty()) this->displayStatusMessages(msgs);
}

/*
 * Reload settings
 */
void MainWindow::initContextMenus()
{
    this->ui->lbl_VoiceStatus->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->ui->lbl_VoiceStatus, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(audioIconContextMenu(const QPoint &)));
}

/*
 * Reload settings
 */
void MainWindow::audioIconContextMenu(const QPoint &position)
{
    // for most widgets
    QPoint globalPosition = this->ui->lbl_VoiceStatus->mapToGlobal(position);

    if (!this->m_contextMenuAudio)
    {
        this->m_contextMenuAudio = new QMenu(this);
        if (QSysInfo::WindowsVersion && QSysInfo::WV_NT_based)
        {
            this->m_contextMenuAudio->addAction("Mixer");
        }
    }

    QAction *selectedItem = this->m_contextMenuAudio->exec(globalPosition);
    if (selectedItem)
    {
        // http://forum.technical-assistance.co.uk/sndvol32exe-command-line-parameters-vt1348.html
        const QList<QAction *> actions = this->m_contextMenuAudio->actions();
        QStringList parameterlist;
        if (selectedItem == actions.at(0)) QProcess::startDetached("SndVol.exe", parameterlist);
    }
}
