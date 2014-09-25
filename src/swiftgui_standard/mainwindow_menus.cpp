#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avaltitude.h"
#include "blackmisc/logmessage.h"
#include <QPoint>
#include <QMenu>
#include <QDesktopServices>
#include <QProcess>
#include <QFontDialog>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

/*
 * Menu clicked
 */
void MainWindow::ps_onMenuClicked()
{
    QObject *sender = QObject::sender();

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
        this->ui->comp_MainInfoArea->getSettingsComponent()->reloadSettings();
        this->ps_displayStatusMessageInGui(CLogMessage().info(this, "Settings reloaded"));
    }
    else if (sender == this->ui->menu_FileReloadStyleSheets)
    {
        CStyleSheetUtility::instance().read();
    }
    else if (sender == this->ui->menu_FileFont)
    {
        this->ps_setMainPage(MainPageFoo);
        this->ui->comp_MainInfoArea->selectSettingsTab(BlackGui::Components::CSettingsComponent::SettingTabGui);
    }
    else if (sender == this->ui->menu_FileClose)
    {
        this->ps_displayStatusMessageInGui(CLogMessage().info(this, "Closing"));
        this->close();
    }
    else if (sender == this->ui->menu_FileSettingsDirectory)
    {
        QString path = QDir::toNativeSeparators(BlackMisc::Settings::CSettingUtilities::getSettingsDirectory());
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
    else if (sender == this->ui->menu_FileResetSettings)
    {
        Q_ASSERT(this->getIContextSettings());
        this->getIContextSettings()->reset(true);
    }
}
