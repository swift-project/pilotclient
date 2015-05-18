/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftcorectrl.h"
#include "ui_swiftcorectrl.h"
#include "blackmisc/icon.h"
#include "blackmisc/loghandler.h"
#include "blackcore/dbus_server.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/components/enableforruntime.h"
#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Components;

CSwiftCoreCtrl::CSwiftCoreCtrl(const SetupInfo &info, QWidget *parent) :
    CSystemTrayWindow(BlackMisc::CIcons::swiftNova24(), parent),
    ui(new Ui::CSwiftCoreCtrl)
{
    ui->setupUi(this);

    SystemTrayMode mode = MinimizeToTray | QuitOnClose;
    setSystemTrayMode(mode);
    setToolTip(QStringLiteral("swiftcore"));

    setupLogDisplay();

    connectSlots();
    ps_onStyleSheetsChanged();
    startCore(info);
}

CSwiftCoreCtrl::~CSwiftCoreCtrl()
{
}

void CSwiftCoreCtrl::ps_startCorePressed()
{
    SetupInfo setup;
    setup.m_dbusAddress = getDBusAddress();
    startCore(setup);
}

void CSwiftCoreCtrl::ps_stopCorePressed()
{
    stopCore();
}

void CSwiftCoreCtrl::ps_appendLogMessage(const CStatusMessage &message)
{
    ui->comp_log->appendStatusMessageToList(message);
}

void CSwiftCoreCtrl::ps_p2pModeToggled(bool checked)
{
    if (checked)
    {
        ui->le_p2pAddress->setEnabled(true);
    }
    else
    {
        ui->le_p2pAddress->setText(QStringLiteral(""));
        ui->le_p2pAddress->setEnabled(false);
    }
}

void CSwiftCoreCtrl::ps_onStyleSheetsChanged()
{
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameSwiftCoreCtrl()
    }
    );
    setStyleSheet(s);
}

void CSwiftCoreCtrl::connectSlots()
{
    connect(ui->pb_startCore, &QPushButton::clicked, this, &CSwiftCoreCtrl::ps_startCorePressed);
    connect(ui->pb_stopCore, &QPushButton::clicked, this, &CSwiftCoreCtrl::ps_stopCorePressed);
    connect(ui->rb_p2pBus, &QRadioButton::toggled, this, &CSwiftCoreCtrl::ps_p2pModeToggled);
    connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CSwiftCoreCtrl::ps_onStyleSheetsChanged);
}

void CSwiftCoreCtrl::setupLogDisplay()
{
    CLogHandler::instance()->install();
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    auto logHandler = CLogHandler::instance()->handlerForPattern(
                          CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
                      );

    logHandler->subscribe(this, &CSwiftCoreCtrl::ps_appendLogMessage);
}

void CSwiftCoreCtrl::startCore(const SetupInfo &setup)
{
    if (getRuntime()) { return; }
    if (setup.m_dbusAddress.isEmpty()) { return; }

    ui->pb_startCore->setEnabled(false);
    ui->pb_stopCore->setEnabled(true);
    ui->gb_dbusMode->setDisabled(true);

    // context
    createRuntime(CRuntimeConfig::forCoreAllLocalInDBus(setup.m_dbusAddress), this);
    CEnableForRuntime::setRuntimeForComponents(this->getRuntime(), this);
    connect(ui->le_CommandLineInput, &CCommandInput::commandEntered, getRuntime(), &CRuntime::parseCommandLine);
}

void CSwiftCoreCtrl::stopCore()
{
    if (!getRuntime()) { return; }

    ui->pb_startCore->setEnabled(true);
    ui->pb_stopCore->setEnabled(false);
    ui->gb_dbusMode->setDisabled(false);

    // Force quit, since we cannot close the runtime
    qApp->quit();
}

QString CSwiftCoreCtrl::getDBusAddress() const
{
    if (ui->rb_sessionBus->isChecked()) { return CDBusServer::sessionDBusServer(); }
    if (ui->rb_systemBus->isChecked()) { return CDBusServer::systemDBusServer(); }
    if (ui->rb_p2pBus->isChecked()) { return CDBusServer::fixAddressToDBusAddress(ui->le_p2pAddress->text()); }

    Q_ASSERT_X(false, "CSwiftCoreCtrl::getDBusAddress()", "The impossible happended!");
    return "";
}
