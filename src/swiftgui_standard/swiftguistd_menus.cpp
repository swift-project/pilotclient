/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistd.h"
#include "ui_swiftguistd.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/aviation/altitude.h"
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
void SwiftGuiStd::ps_onMenuClicked()
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
    else if (sender == this->ui->menu_TestLocationsLOWW)
    {
        this->setTestPosition("N 40° 7′ 6.3588", "E 16° 33′ 39.924", CAltitude(100, CAltitude::MeanSeaLevel, CLengthUnit::m()));
    }
    else if (sender == this->ui->menu_ReloadSettings)
    {
        this->ui->comp_MainInfoArea->getSettingsComponent()->reloadSettings();
        CLogMessage(this).info("Settings reloaded");
    }
    else if (sender == this->ui->menu_TestInternals)
    {
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageInternals);
    }
    else if (sender == this->ui->menu_FileReloadStyleSheets)
    {
        CStyleSheetUtility::instance().read();
    }
    else if (sender == this->ui->menu_WindowFont)
    {
        this->ps_setMainPageToInfoArea();
        this->ui->comp_MainInfoArea->selectSettingsTab(BlackGui::Components::CSettingsComponent::SettingTabGui);
    }
    else if (sender == this->ui->menu_WindowMinimize)
    {
        this->showMinimized();
    }
    else if (sender == this->ui->menu_WindowToggleOnTop)
    {
        this->ps_toogleWindowStayOnTop();
    }
    else if (sender == this->ui->menu_FileExit)
    {
        CLogMessage(this).info("Closing");
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
    else if (sender == this->ui->menu_DebugMetaTypes)
    {
        QString metadata;
        QTextStream stream(&metadata);
        BlackMisc::displayAllUserMetatypesTypes(stream);
        this->ui->comp_MainInfoArea->getLogComponent()->appendPlainTextToConsole(metadata);
    }
}

void SwiftGuiStd::initDynamicMenus()
{
    Q_ASSERT(this->ui->menu_InfoAreas);
    Q_ASSERT(this->ui->comp_MainInfoArea);
    this->ui->menu_InfoAreas->addActions(this->ui->comp_MainInfoArea->getInfoAreaSelectActions(this->ui->menu_InfoAreas));
}

void SwiftGuiStd::initMenuIcons()
{
    this->ui->menu_WindowMinimize->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarMinButton));
}

