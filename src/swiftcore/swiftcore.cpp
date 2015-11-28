/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftcore.h"
#include "ui_swiftcore.h"
#include "blackmisc/icon.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/project.h"
#include "blackmisc/dbusserver.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/enableforruntime.h"
#include "blackgui/stylesheetutility.h"
#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Components;

CSwiftCore::CSwiftCore(const SetupInfo &info, QWidget *parent) :
    CSystemTrayWindow(CIcons::swiftNova24(), parent),
    CIdentifiable(this),
    ui(new Ui::CSwiftCore)
{
    ui->setupUi(this);
    const QString name(QCoreApplication::instance()->applicationName() + " " + CProject::version());
    setSystemTrayMode(MinimizeToTray | QuitOnClose);
    setSystemTrayToolTip(name);
    setWindowTitle(name);
    setWindowIcon(CIcons::swiftNova24());
    setWindowIconText(name);

    initLogDisplay();
    initSlots();
    initStyleSheet();
    initDBusMode(info);

    if (info.m_start) { startCore(info); }
}

void CSwiftCore::initStyleSheet()
{
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameStandardWidget(),
        CStyleSheetUtility::fileNameSwiftCore()
    }
    );
    this->setStyleSheet(s);
}

void CSwiftCore::initDBusMode(const CSwiftCore::SetupInfo &setup)
{
    if (setup.m_dbusAddress.startsWith(CDBusServer::sessionDBusServer()))
    {
        this->ui->rb_SessionBus->setChecked(true);
    }
    else if (setup.m_dbusAddress.startsWith(CDBusServer::systemDBusServer()))
    {
        this->ui->rb_SystemBus->setChecked(true);
    }
    else
    {
        this->ui->rb_P2PBus->setChecked(true);
        this->ui->le_P2PAddress->setText(setup.m_dbusAddress);
    }
}

CSwiftCore::~CSwiftCore()
{ }

void CSwiftCore::ps_startCorePressed()
{
    SetupInfo setup;
    setup.m_dbusAddress = getDBusAddress();
    startCore(setup);
}

void CSwiftCore::ps_stopCorePressed()
{
    stopCore();
}

void CSwiftCore::ps_appendLogMessage(const CStatusMessage &message)
{
    ui->comp_InfoArea->getLogComponent()->appendStatusMessageToList(message);
}

void CSwiftCore::ps_p2pModeToggled(bool checked)
{
    if (checked)
    {
        ui->le_P2PAddress->setEnabled(true);
    }
    else
    {
        ui->le_P2PAddress->setText(QString());
        ui->le_P2PAddress->setEnabled(false);
    }
}

void CSwiftCore::ps_onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void CSwiftCore::initSlots()
{
    connect(ui->pb_StartCore, &QPushButton::clicked, this, &CSwiftCore::ps_startCorePressed);
    connect(ui->pb_StopCore, &QPushButton::clicked, this, &CSwiftCore::ps_stopCorePressed);
    connect(ui->rb_P2PBus, &QRadioButton::toggled, this, &CSwiftCore::ps_p2pModeToggled);
    connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CSwiftCore::ps_onStyleSheetsChanged);
}

void CSwiftCore::initLogDisplay()
{
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    auto logHandler = CLogHandler::instance()->handlerForPattern(
                          CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
                      );
    logHandler->subscribe(this, &CSwiftCore::ps_appendLogMessage);
}

void CSwiftCore::startCore(const SetupInfo &setup)
{
    if (getRuntime()) { return; }
    if (setup.m_dbusAddress.isEmpty()) { return; }

    ui->pb_StartCore->setEnabled(false);
    ui->pb_StopCore->setEnabled(true);
    ui->gb_DBusMode->setDisabled(true);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // context
    this->createRuntime(setup.m_coreAudio ?
                        CRuntimeConfig::forCoreAllLocalInDBus(setup.m_dbusAddress) :
                        CRuntimeConfig::forCoreAllLocalInDBusNoAudio(setup.m_dbusAddress),
                        this);
    CEnableForRuntime::setRuntimeForComponents(this->getRuntime(), this);
    connect(ui->le_CommandLineInput, &CCommandInput::commandEntered, getRuntime(), &CRuntime::parseCommandLine);
}

void CSwiftCore::stopCore()
{
    if (!getRuntime()) { return; }

    ui->pb_StartCore->setEnabled(true);
    ui->pb_StopCore->setEnabled(false);
    ui->gb_DBusMode->setDisabled(false);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    getRuntime()->gracefulShutdown();

    // Force quit, since we cannot close the runtime
    qApp->quit();
}

QString CSwiftCore::getDBusAddress() const
{
    if (ui->rb_SessionBus->isChecked()) { return CDBusServer::sessionDBusServer(); }
    if (ui->rb_SystemBus->isChecked()) { return CDBusServer::systemDBusServer(); }
    if (ui->rb_P2PBus->isChecked()) { return CDBusServer::fixAddressToDBusAddress(ui->le_P2PAddress->text()); }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong DBus address");
    return "";
}
