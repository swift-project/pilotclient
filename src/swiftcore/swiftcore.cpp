// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftcore.h"
#include "blackgui/components/commandinput.h"
#include "blackgui/components/coreinfoareacomponent.h"
#include "blackgui/components/coresettingsdialog.h"
#include "blackgui/components/cockpitcomaudiodialog.h"
#include "blackgui/components/audioadvanceddistributeddialog.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/rawfsdmessagesdialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/corefacade.h"
#include "misc/dbusserver.h"
#include "misc/icons.h"
#include "misc/loghandler.h"
#include "misc/logpattern.h"
#include "ui_swiftcore.h"

#include <QDialog>
#include <QFlags>
#include <QGroupBox>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QMessageBox>

using namespace swift::misc;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Components;

CSwiftCore::CSwiftCore(QWidget *parent) : CSystemTrayWindow(CIcons::swiftCore24(), parent),
                                          CIdentifiable(this),
                                          ui(new Ui::CSwiftCore)
{
    Q_ASSERT(sGui);
    ui->setupUi(this);
    sGui->initMainApplicationWidget(this);
    const QString name(sGui->getApplicationNameAndVersion());
    setSystemTrayMode(MinimizeToTray | QuitOnClose);
    setSystemTrayToolTip(name);

    m_mwaLogComponent = ui->comp_InfoArea->getLogComponent();
    m_mwaOverlayFrame = nullptr;
    m_mwaStatusBar = nullptr;

    connect(ui->pb_Restart, &QPushButton::clicked, this, &CSwiftCore::restart);
    connect(ui->pb_Audio, &QPushButton::clicked, this, &CSwiftCore::audioDialog, Qt::QueuedConnection);
    connect(ui->pb_AudioAdvanced, &QPushButton::clicked, this, &CSwiftCore::audioAdvancedDialog, Qt::QueuedConnection);
    connect(ui->pb_DisconnectNetwork, &QPushButton::clicked, this, &CSwiftCore::disconnectFromNetwork, Qt::QueuedConnection);
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CSwiftCore::onStyleSheetsChanged, Qt::QueuedConnection);

    this->initLogDisplay();
    this->initStyleSheet();
    this->initMenus();

    // log
    CLogMessage(this).info(u"Cmd: %1") << CGuiApplication::arguments().join(" ");

    // command line
    ui->lep_CommandLineInput->setIdentifier(this->identifier());
    connect(ui->lep_CommandLineInput, &CCommandInput::commandEntered, sGui->getCoreFacade(), &CCoreFacade::parseCommandLine);
}

CSwiftCore::~CSwiftCore()
{}

void CSwiftCore::initStyleSheet()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    const QString s = sGui->getStyleSheetUtility().styles(
        { CStyleSheetUtility::fileNameFonts(),
          CStyleSheetUtility::fileNameStandardWidget(),
          CStyleSheetUtility::fileNameSwiftCore() });
    this->setStyleSheet(""); // avoid crash, need to reset before
    this->setStyleSheet(s);
}

void CSwiftCore::showSettingsDialog()
{
    if (!m_settingsDialog)
    {
        m_settingsDialog.reset(new CCoreSettingsDialog(this));
    }
    m_settingsDialog->show();
}

void CSwiftCore::showRawFsdMessageDialog()
{
    if (!m_rawFsdMessageDialog)
    {
        m_rawFsdMessageDialog.reset(new CRawFsdMessagesDialog(this));
    }
    m_rawFsdMessageDialog->show();
}

void CSwiftCore::onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void CSwiftCore::initLogDisplay()
{
    m_mwaLogComponent->showDetails(false);
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    ui->comp_InfoArea->getLogComponent()->showFilterDialog(); // add a filter dialog
}

void CSwiftCore::initMenus()
{
    sGui->addMenuFile(*ui->menu_File);
    sGui->addMenuWindow(*ui->menu_Window);
    sGui->addMenuHelp(*ui->menu_Help);
    connect(ui->menu_SettingsDialog, &QAction::triggered, this, &CSwiftCore::showSettingsDialog);
    connect(ui->menu_RawFsdMessageDialog, &QAction::triggered, this, &CSwiftCore::showRawFsdMessageDialog);
}

void CSwiftCore::restart()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    ui->pb_Restart->setEnabled(false);
    const QStringList args = this->getRestartCmdArgs();
    sGui->restartApplication(args);
}

void CSwiftCore::disconnectFromNetwork()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (!sGui->getIContextNetwork()) { return; }
    if (!sGui->getIContextNetwork()->isConnected()) { return; }

    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Disconnect", "Disconnect from network?", QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) { return; }
    sGui->getIContextNetwork()->disconnectFromNetwork();
}

void CSwiftCore::audioDialog()
{
    if (!m_audioDialog)
    {
        m_audioDialog.reset(new CCockpitComAudioDialog(this));
    }
    m_audioDialog->setModal(false);
    m_audioDialog->show();
}

void CSwiftCore::audioAdvancedDialog()
{
    if (!m_audioAdvDialog)
    {
        m_audioAdvDialog.reset(new CAudioAdvancedDistributedDialog(this));
    }
    m_audioAdvDialog->setModal(false);
    m_audioAdvDialog->show();
}

QStringList CSwiftCore::getRestartCmdArgs() const
{
    const QStringList cmds = ui->comp_DBusSelector->getDBusCmdLineArgs();
    return cmds;
}
