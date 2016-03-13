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
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/components/logcomponent.h"
#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Components;

CSwiftCore::CSwiftCore(QWidget *parent) :
    CSystemTrayWindow(CIcons::swiftNova24(), parent),
    CIdentifiable(this),
    ui(new Ui::CSwiftCore)
{
    ui->setupUi(this);
    sGui->initMainApplicationWindow(this);
    const QString name(sGui->getApplicationNameAndVersion());
    setSystemTrayMode(MinimizeToTray | QuitOnClose);
    setSystemTrayToolTip(name);

    this->m_mwaLogComponent = this->ui->comp_InfoArea->getLogComponent();
    this->m_mwaOverlayFrame = nullptr;
    this->m_mwaStatusBar = nullptr;

    initLogDisplay();
    initSlots();
    initStyleSheet();
    initDBusMode();

    if (sGui->isParserOptionSet("start")) { startCore(sGui->getCmdDBusAddressValue()); }
}

void CSwiftCore::initStyleSheet()
{
    const QString s = sGui->getStyleSheetUtility().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameStandardWidget(),
        CStyleSheetUtility::fileNameSwiftCore()
    }
    );
    this->setStyleSheet(s);
}

void CSwiftCore::initDBusMode()
{
    const QString dBusAddress(sGui->getCmdDBusAddressValue());
    if (dBusAddress.startsWith(CDBusServer::sessionBusAddress()))
    {
        this->ui->rb_SessionBus->setChecked(true);
    }
    else if (dBusAddress.startsWith(CDBusServer::systemBusAddress()))
    {
        this->ui->rb_SystemBus->setChecked(true);
    }
    else
    {
        this->ui->rb_P2PBus->setChecked(true);
        this->ui->le_P2PAddress->setText(dBusAddress);
    }
}

CSwiftCore::~CSwiftCore()
{ }

void CSwiftCore::ps_startCorePressed()
{
    startCore(getDBusAddress());
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
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CSwiftCore::ps_onStyleSheetsChanged);
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

void CSwiftCore::startCore(const QString &dBusAdress)
{
    if (dBusAdress.isEmpty()) { return; }

    ui->pb_StartCore->setEnabled(false);
    ui->pb_StopCore->setEnabled(true);
    ui->gb_DBusMode->setDisabled(true);
    sGui->processEventsToRefreshGui();

    // context
    connect(ui->le_CommandLineInput, &CCommandInput::commandEntered, sGui->getCoreFacade(), &CCoreFacade::parseCommandLine);
}

void CSwiftCore::stopCore()
{
    ui->pb_StartCore->setEnabled(true);
    ui->pb_StopCore->setEnabled(false);
    ui->gb_DBusMode->setDisabled(false);
    sGui->processEventsToRefreshGui();

    sGui->exit();
}

QString CSwiftCore::getDBusAddress() const
{
    if (ui->rb_SessionBus->isChecked()) { return CDBusServer::sessionBusAddress(); }
    if (ui->rb_SystemBus->isChecked()) { return CDBusServer::systemBusAddress(); }
    if (ui->rb_P2PBus->isChecked()) { return CDBusServer::normalizeAddress(ui->le_P2PAddress->text()); }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong DBus address");
    return "";
}
