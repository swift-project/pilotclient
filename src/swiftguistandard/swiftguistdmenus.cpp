// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/maininfoareacomponent.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/components/autopublishdialog.h"
#include "blackgui/guiactionbind.h"
#include "blackgui/guiapplication.h"
#include "blackgui/foreignwindows.h"
#include "blackgui/copyxswiftbusdialog.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"
#include "swiftguistd.h"
#include "ui_swiftguistd.h"

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QScopedPointer>
#include <QStackedWidget>
#include <QtGlobal>
#include <QDesktopServices>
#include <QMessageBox>

using namespace BlackConfig;
using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

void SwiftGuiStd::onMenuClicked()
{
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
        this->setTestPosition("N 50° 2′ 0", "E 8° 34′ 14",
                              CAltitude(100, CAltitude::MeanSeaLevel, CLengthUnit::m()),
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
        ui->comp_MainInfoArea->selectSettingsTab(BlackGui::Components::CSettingsComponent::SettingTabGui);
    }
    else if (sender == ui->menu_InternalsPage)
    {
        ui->sw_MainMiddle->setCurrentIndex(MainPageInternals);
    }
    else if (sender == ui->menu_AutoPublish)
    {
        this->autoPublishDialog();
    }
    else if (sender == ui->menu_ToggleIncognito)
    {
        if (sGui)
        {
            sGui->toggleIncognito();
            this->displayInOverlayWindow(QStringLiteral("Incognito mode is %1").arg(boolToOnOff(sGui->isIncognito())), 5000);
        }
    }
}

void SwiftGuiStd::attachSimulatorWindow()
{
    this->activateWindow(); // attaching requires active window
    QWindow *w = CForeignWindows::getFirstFoundSimulatorWindow();
    if (!w)
    {
        CLogMessage(this).warning(u"No simulator window found");
        return;
    }
    const bool a = CForeignWindows::setSimulatorAsParent(w, this);
    if (a)
    {
        CLogMessage(this).info(u"Attached to simulator");
    }
    else
    {
        CLogMessage(this).warning(u"No simulator window found");
    }
}

void SwiftGuiStd::detachSimulatorWindow()
{
    if (CForeignWindows::unsetSimulatorAsParent(this))
    {
        CLogMessage(this).info(u"Detached simulator window");
    }
    else
    {
        CLogMessage(this).info(u"No simulator window to detach");
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
    ui->menu_Window->addSeparator();
    QAction *a = ui->menu_Window->addAction("Attach simulator window");
    bool c = connect(a, &QAction::triggered, this, &SwiftGuiStd::attachSimulatorWindow);
    Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed");
    a = ui->menu_Window->addAction("Detach simulator window");
    c = connect(a, &QAction::triggered, this, &SwiftGuiStd::detachSimulatorWindow);
    Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed");

    sGui->addMenuHelp(*ui->menu_Help);
    ui->menu_InfoAreas->addActions(ui->comp_MainInfoArea->getInfoAreaSelectActions(true, ui->menu_InfoAreas));

    if (CBuildConfig::isLocalDeveloperDebugBuild() && ui->menu_File && ui->menu_File->actions().size() > 5)
    {
        QAction *act = new QAction(CIcons::swift16(), "Copy XSwiftBus dialog");
        ui->menu_File->insertAction(ui->menu_File->actions().at(5), act);
        c = connect(
            act, &QAction::triggered, this, [=] {
                this->copyXSwiftBusDialog(false);
            },
            Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed");
    }

    // for hotkeys
    const QString swift(CGuiActionBindHandler::pathSwiftPilotClient());
    static const CActionBind swiftRoot(swift, CIcons::Swift16); // inserts action for root folder
    Q_UNUSED(swiftRoot)
    m_menuHotkeyHandlers.append(CGuiActionBindHandler::bindMenu(ui->menu_InfoAreas, swift + "Info areas"));
    m_menuHotkeyHandlers.append(CGuiActionBindHandler::bindMenu(ui->menu_File, swift + "File"));
    m_menuHotkeyHandlers.append(CGuiActionBindHandler::bindMenu(ui->menu_Window, swift + "Window"));
}

void SwiftGuiStd::copyXSwiftBusDialog(bool checkFileTimestamp)
{
    const QString xPlaneRootDir = ui->comp_MainInfoArea->getSettingsComponent()->getSimulatorSettings(CSimulatorInfo::XPLANE).getSimulatorDirectoryOrDefault();
    const bool xpDirExists = !xPlaneRootDir.isEmpty() && QDir().exists(xPlaneRootDir);
    if (!xpDirExists)
    {
        if (checkFileTimestamp) { return; }
        QMessageBox::warning(this, tr("Copy XSwiftBus"), tr("XPlane directory does not exists!"), QMessageBox::Close);
        return;
    }

    const int c = CCopyXSwiftBusDialog::displayDialogAndCopyBuildFiles(xPlaneRootDir, checkFileTimestamp, this);
    if (c > 0) { CLogMessage(this).info(u"Copied %1 files from build directory") << c; }
}

int SwiftGuiStd::autoPublishDialog()
{
    if (!m_autoPublishDialog)
    {
        m_autoPublishDialog.reset(new CAutoPublishDialog(this));
    }
    m_lastAutoPublish.set(QDateTime::currentMSecsSinceEpoch());
    return m_autoPublishDialog->readAndShow();
}
