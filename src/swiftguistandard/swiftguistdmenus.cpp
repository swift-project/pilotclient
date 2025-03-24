// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <QAction>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QScopedPointer>
#include <QStackedWidget>
#include <QtGlobal>

#include "swiftguistd.h"
#include "ui_swiftguistd.h"

#include "config/buildconfig.h"
#include "gui/components/autopublishdialog.h"
#include "gui/components/maininfoareacomponent.h"
#include "gui/components/settingscomponent.h"
#include "gui/guiactionbind.h"
#include "gui/guiapplication.h"
#include "misc/aviation/altitude.h"
#include "misc/logmessage.h"
#include "misc/pq/units.h"

using namespace swift::config;
using namespace swift::gui;
using namespace swift::gui::components;
using namespace swift::core;
using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::simulation;

void SwiftGuiStd::onMenuClicked()
{
    using namespace std::chrono_literals;
    QObject *sender = QObject::sender();
    if (sender == ui->menu_TestLocationsEDRY)
    {
        this->setTestPosition("N 049° 18' 17", "E 008° 27' 05",
                              CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft()),
                              CAltitude(312, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::ft()));
    }
    else if (sender == ui->menu_TestLocationsEDNX)
    {
        this->setTestPosition("N 048° 14′ 22", "E 011° 33′ 41",
                              CAltitude(486, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                              CAltitude(486, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m()));
    }
    else if (sender == ui->menu_TestLocationsEDDM)
    {
        this->setTestPosition("N 048° 21′ 14", "E 011° 47′ 10",
                              CAltitude(448, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                              CAltitude(448, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m()));
    }
    else if (sender == ui->menu_TestLocationsEDDF)
    {
        this->setTestPosition("N 50° 2′ 0", "E 8° 34′ 14", CAltitude(100, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                              CAltitude(100, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m()));
    }
    else if (sender == ui->menu_TestLocationsLOWW)
    {
        this->setTestPosition("N 48° 7′ 6.3588", "E 16° 33′ 39.924",
                              CAltitude(100, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                              CAltitude(100, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m()));
    }
    else if (sender == ui->menu_WindowFont)
    {
        this->setMainPageToInfoArea();
        ui->comp_MainInfoArea->selectSettingsTab(swift::gui::components::CSettingsComponent::SettingTabGui);
    }
    else if (sender == ui->menu_InternalsPage) { ui->sw_MainMiddle->setCurrentIndex(MainPageInternals); }
    else if (sender == ui->menu_AutoPublish) { this->autoPublishDialog(); }
    else if (sender == ui->menu_ToggleIncognito)
    {
        if (sGui)
        {
            sGui->toggleIncognito();
            this->displayInOverlayWindow(QStringLiteral("Incognito mode is %1").arg(boolToOnOff(sGui->isIncognito())),
                                         5s);
        }
    }
}

void SwiftGuiStd::initMenus()
{
    Q_ASSERT_X(ui->menu_InfoAreas, Q_FUNC_INFO, "No menu");
    Q_ASSERT_X(ui->menu_Window, Q_FUNC_INFO, "No menu");
    Q_ASSERT_X(ui->comp_MainInfoArea, Q_FUNC_INFO, "no main area");
    sGui->addMenuFile(*ui->menu_File);
    sGui->addMenuInternals(*ui->menu_Internals);
    sGui->addMenuWindow(*ui->menu_Window);

    sGui->addMenuHelp(*ui->menu_Help);
    ui->menu_InfoAreas->addActions(ui->comp_MainInfoArea->getInfoAreaSelectActions(true, ui->menu_InfoAreas));

    // for hotkeys
    const QString swift(CGuiActionBindHandler::pathSwiftPilotClient());
    static const CActionBind swiftRoot(swift, CIcons::Swift16); // inserts action for root folder
    Q_UNUSED(swiftRoot)
    m_menuHotkeyHandlers.append(CGuiActionBindHandler::bindMenu(ui->menu_InfoAreas, swift + "Info areas"));
    m_menuHotkeyHandlers.append(CGuiActionBindHandler::bindMenu(ui->menu_File, swift + "File"));
    m_menuHotkeyHandlers.append(CGuiActionBindHandler::bindMenu(ui->menu_Window, swift + "Window"));
}

int SwiftGuiStd::autoPublishDialog()
{
    if (!m_autoPublishDialog) { m_autoPublishDialog.reset(new CAutoPublishDialog(this)); }
    m_lastAutoPublish.set(QDateTime::currentMSecsSinceEpoch());
    return m_autoPublishDialog->readAndShow();
}
