/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftlauncher.h"
#include "ui_swiftlauncher.h"
#include "blackconfig/buildconfig.h"
#include "blackgui/components/configurationwizard.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/context/contextapplicationproxy.h"
#include "blackcore/vatsim/networkvatlib.h"
#include "blackcore/setupreader.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include <QPixmap>
#include <QBitmap>
#include <QTimer>
#include <QProcess>
#include <QPushButton>
#include <QStringBuilder>
#include <QDesktopServices>
#include <QShortcut>
#include <qcompilerdetection.h>

using namespace BlackConfig;
using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackCore::Data;
using namespace BlackCore::Vatsim;
using namespace BlackMisc;
using namespace BlackMisc::Network;

CSwiftLauncher::CSwiftLauncher(QWidget *parent) :
    QDialog(parent, CEnableForFramelessWindow::modeToWindowFlags(CEnableForFramelessWindow::WindowNormal)),
    CEnableForFramelessWindow(CEnableForFramelessWindow::WindowFrameless, true, "framelessMainWindow", this),
    CIdentifiable(this),
    ui(new Ui::CSwiftLauncher)
{
    ui->setupUi(this);
    this->init();
    connect(ui->tb_SwiftCore, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_SwiftMappingTool, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_SwiftGui, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_Database, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_BackToMain, &QToolButton::pressed, this, &CSwiftLauncher::ps_showMainPage);
    connect(ui->tb_ConfigurationWizard, &QToolButton::pressed, this, &CSwiftLauncher::ps_startWizard);
    connect(ui->tb_Launcher, &QToolBox::currentChanged, this, &CSwiftLauncher::ps_tabChanged);
    connect(ui->comp_DistributionInfo, &CDistributionInfoComponent::distributionInfoAvailable, this, &CSwiftLauncher::ps_distributionInfoAvailable);
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CSwiftLauncher::ps_onStyleSheetsChanged);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this, SLOT(ps_showLogPage()));

    // default from settings
    this->setDefaults();

    // periodically check
    connect(&m_checkTimer, &QTimer::timeout, this, &CSwiftLauncher::ps_checkRunningApplicationsAndCore);
    m_checkTimer.setInterval(2500);
    m_checkTimer.start();

    // auto launch wizard
    if (sGui->isInstallerOptionSet())
    {
        QTimer::singleShot(2500, this, &CSwiftLauncher::ps_startWizard);
    }
}

CSwiftLauncher::~CSwiftLauncher()
{ }

QString CSwiftLauncher::getCmdLine() const
{
    return this->toCmdLine(m_executable, m_executableArgs);
}

bool CSwiftLauncher::startDetached()
{
    if (m_executable.isEmpty()) { return false; }
    const QString cmd = this->getCmdLine();
    CLogMessage(this).info(cmd);
    return QProcess::startDetached(m_executable, m_executableArgs);
}

CEnableForFramelessWindow::WindowMode CSwiftLauncher::getWindowMode() const
{
    if (ui->rb_WindowFrameless->isChecked()) { return CEnableForFramelessWindow::WindowFrameless; }
    return CEnableForFramelessWindow::WindowNormal;
}

CoreModes::CoreMode CSwiftLauncher::getCoreMode() const
{
    if (ui->rb_SwiftStandalone->isChecked()) { return CoreModes::CoreInGuiProcess; }
    if (ui->rb_SwiftCoreAudioOnCore->isChecked()) { return CoreModes::CoreExternalCoreAudio; }
    if (ui->rb_SwiftCoreAudioOnGui->isChecked()) { return CoreModes::CoreExternalAudioGui; }

    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
    return CoreModes::CoreInGuiProcess;
}

void CSwiftLauncher::mouseMoveEvent(QMouseEvent *event)
{
    if (!handleMouseMoveEvent(event)) { QDialog::mouseMoveEvent(event); }
}

void CSwiftLauncher::ps_displayLatestNews(QNetworkReply *reply)
{
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(reply);
    if (nwReply->error() == QNetworkReply::NoError)
    {
        const QString html = nwReply->readAll().trimmed();
        if (html.isEmpty()) { return; }
        CLogMessage(this).info("Received news from '%1'") << nwReply->url().toString();
        ui->tbr_LatestNews->setHtml(html); // causes QFSFileEngine::open: No file name specified
        constexpr qint64 newNews = 72 * 3600 * 1000;
        const qint64 deltaT = CNetworkUtils::lastModifiedSinceNow(nwReply.data());
        if (deltaT > 0 && deltaT < newNews)
        {
            ui->tb_Launcher->setCurrentWidget(ui->pg_LatestNews);
        }
    }
    else
    {
        CLogMessage(this).warning("Error received news from '%1'") << nwReply->url().toString();
    }
}

void CSwiftLauncher::ps_distributionInfoAvailable()
{
    this->setHeaderInfo(ui->comp_DistributionInfo->getNewAvailableVersionForSelection());
    this->loadLatestNews();
    this->loadAbout();
}

void CSwiftLauncher::mousePressEvent(QMouseEvent *event)
{
    if (!handleMousePressEvent(event)) { QDialog::mousePressEvent(event); }
}

void CSwiftLauncher::init()
{
    sGui->initMainApplicationWindow(this);

    m_mwaOverlayFrame = ui->fr_SwiftLauncherMain;
    m_mwaStatusBar = nullptr;
    m_mwaLogComponent = ui->comp_SwiftLauncherLog;

    this->initStyleSheet();
    this->initLogDisplay();

    ui->lbl_HeaderInfo->setVisible(false);
    ui->sw_SwiftLauncher->setCurrentWidget(ui->pg_SwiftLauncherMain);
    ui->tb_Launcher->setCurrentWidget(ui->pg_CoreMode);
}

void CSwiftLauncher::initStyleSheet()
{
    const QString s = sGui->getStyleSheetUtility().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameStandardWidget(),
        CStyleSheetUtility::fileNameSwiftLauncher()
    }
    );
    this->setStyleSheet(s);
}

void CSwiftLauncher::loadLatestNews()
{
    CFailoverUrlList newsUrls(sGui->getGlobalSetup().getSwiftLatestNewsUrls());
    const CUrl newsUrl(newsUrls.obtainNextWorkingUrl(true, 10 * 1000));
    if (newsUrl.isEmpty())
    {
        CLogMessage(this).warning("No working news URL in %1") << newsUrls.toQString();
        return;
    }
    sGui->getFromNetwork(newsUrl, { this, &CSwiftLauncher::ps_displayLatestNews});
}

void CSwiftLauncher::loadAbout()
{
    // workaround:
    // 1) Only reading as HTML gives proper formatting
    // 2) Reading the file resource fails (likely because of the style sheet)
    static const QString html = CFileUtils::readFileToString(CDirectoryUtils::aboutFilePath());
    static const QString legalDir = sGui->getGlobalSetup().getLegalDirectoryUrl().getFullUrl();

    // make links absolute
    static const QString htmlFixed = QString(html).
                                     replace(QLatin1String("href=\"./"), "href=\"" + legalDir);
    ui->tbr_About->setHtml(htmlFixed);
}

void CSwiftLauncher::initLogDisplay()
{
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    auto logHandler = CLogHandler::instance()->handlerForPattern(
                          CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
                      );
    logHandler->subscribe(this, &CSwiftLauncher::ps_appendLogMessage);

    ui->comp_SwiftLauncherLog->showFilterBar();
    ui->comp_SwiftLauncherLog->filterUseRadioButtonDescriptiveIcons(false);
}

void CSwiftLauncher::setHeaderInfo(const QString &newVersionAvailable)
{
    ui->lbl_HeaderInfo->setVisible(!newVersionAvailable.isEmpty());
    if (!newVersionAvailable.isEmpty())
    {
        ui->lbl_HeaderInfo->setText("New version '" + newVersionAvailable + "' available");
        ui->lbl_HeaderInfo->setStyleSheet("background: red; color: yellow;");
    }
}

void CSwiftLauncher::startSwiftCore()
{
    this->saveSetup();
    QStringList args = ui->comp_DBusSelector->getDBusCmdLineArgs();
    if (ui->rb_SwiftCoreAudioOnCore->isChecked())
    {
        args.append("--coreaudio");
    }

    m_executableArgs = sGui->argumentsJoined(args);
    m_executable = CDirectoryUtils::executableFilePath(CBuildConfig::swiftCoreExecutableName());
    this->startDetached();
}

void CSwiftLauncher::setSwiftDataExecutable()
{
    m_executable = CDirectoryUtils::executableFilePath(CBuildConfig::swiftDataExecutableName());
    m_executableArgs = sGui->argumentsJoined({}, CNetworkVatlib::vatlibArguments());
}

bool CSwiftLauncher::setSwiftGuiExecutable()
{
    m_executable = CDirectoryUtils::executableFilePath(CBuildConfig::swiftGuiExecutableName());
    QStringList args
    {
        "--core", CoreModes::coreModeToString(getCoreMode()),
        "--window", CEnableForFramelessWindow::windowModeToString(getWindowMode())
    };

    this->saveSetup();
    if (!this->isStandaloneGuiSelected())
    {
        const QString dBus(ui->comp_DBusSelector->getDBusAddress());
        args.append(ui->comp_DBusSelector->getDBusCmdLineArgs());
        this->saveSetup();

        QString msg;
        if (!CSwiftLauncher::canConnectSwiftOnDBusServer(dBus, msg))
        {
            const CStatusMessage m(this, CStatusMessage::SeverityError,
                                   "DBus server for '" + dBus + "' can not be connected.\n\n" +
                                   "Likely the core is not running or is not reachable.\n\n" +
                                   "Details: " + msg, true);
            this->ps_showStatusMessage(m);
            return false;
        }
    }
    m_executableArgs = sGui->argumentsJoined(args);
    return true;
}

bool CSwiftLauncher::canConnectSwiftOnDBusServer(const QString &dBusAddress, QString &msg) const
{
    if (this->isStandaloneGuiSelected()) { return true; } // do not mind here
    return CContextApplicationProxy::isContextResponsive(dBusAddress, msg);
}

bool CSwiftLauncher::isStandaloneGuiSelected() const
{
    return ui->rb_SwiftStandalone->isChecked();
}

void CSwiftLauncher::setDefaults()
{
    const CLauncherSetup setup(m_setup.get());
    const QString dbus(setup.getDBusAddress().toLower().trimmed());
    ui->comp_DBusSelector->set(dbus);
    if (setup.useFramelessWindow())
    {
        ui->rb_WindowFrameless->setChecked(true);
    }
    else
    {
        ui->rb_WindowNormal->setChecked(true);
    }
    switch (setup.getCoreMode())
    {
    case CLauncherSetup::Standalone: ui->rb_SwiftStandalone->setChecked(true); break;
    case CLauncherSetup::CoreWithAudioOnCore: ui->rb_SwiftCoreAudioOnCore->setChecked(true); break;
    case CLauncherSetup::CoreWithAudioOnGui: ui->rb_SwiftCoreAudioOnGui->setChecked(true); break;
    default:
        break;
    }
}

void CSwiftLauncher::saveSetup()
{
    CLauncherSetup setup = m_setup.get();
    const QString dBus(ui->comp_DBusSelector->getDBusAddress());
    if (!dBus.isEmpty()) { setup.setDBusAddress(dBus); }
    setup.setFramelessWindow(ui->rb_WindowFrameless->isChecked());
    setup.setCoreMode(CLauncherSetup::Standalone);
    if (ui->rb_SwiftCoreAudioOnCore->isChecked())
    {
        setup.setCoreMode(CLauncherSetup::CoreWithAudioOnCore);
    }
    else if (ui->rb_SwiftCoreAudioOnGui->isChecked())
    {
        setup.setCoreMode(CLauncherSetup::CoreWithAudioOnGui);
    }
    m_setup.set(setup);
}

bool CSwiftLauncher::warnAboutOtherSwiftApplications()
{
    CApplicationInfoList running = CGuiApplication::getRunningApplications();
    running.removeApplication(CApplicationInfo::Laucher);
    if (running.isEmpty()) { return true; }

    // getting here means another application is running
    const QString msg =
        QStringLiteral("While using the wizard no other application should run.\nClose applications and try again.\nCurrently running: ") %
        running.processNames().join(',');
    QMessageBox::question(this, "Wizard", msg, QMessageBox::Close);
    return false;
}

QString CSwiftLauncher::toCmdLine(const QString &exe, const QStringList &exeArgs)
{
    if (exeArgs.isEmpty()) { return exe; }
    const QString exeArgsString = exeArgs.join(' ');
    const QString cmd(exe + " " + exeArgsString);
    return cmd;
}

void CSwiftLauncher::ps_startButtonPressed()
{
    const QObject *sender = QObject::sender();
    if (sender == ui->tb_SwiftGui)
    {
        if (this->setSwiftGuiExecutable())
        {
            this->accept();
        }
    }
    else if (sender == ui->tb_SwiftMappingTool)
    {
        ui->tb_SwiftMappingTool->setEnabled(false);
        m_startMappingToolWaitCycles = 2;
        this->setSwiftDataExecutable();
        this->accept();
    }
    else if (sender == ui->tb_SwiftCore)
    {
        if (this->isStandaloneGuiSelected()) { ui->rb_SwiftCoreAudioOnGui->setChecked(true); }
        ui->tb_SwiftCore->setEnabled(false);
        m_startCoreWaitCycles = 2;
        this->startSwiftCore();
    }
    else if (sender == ui->tb_Database)
    {
        const CUrl homePage(sApp->getGlobalSetup().getDbHomePageUrl());
        QDesktopServices::openUrl(homePage);
    }
}

void CSwiftLauncher::ps_dbusServerModeSelected(bool selected)
{
    if (!selected) { return; }
    if (!this->isStandaloneGuiSelected()) { return; }
    ui->rb_SwiftCoreAudioOnGui->setChecked(true);
}

void CSwiftLauncher::ps_showStatusMessage(const CStatusMessage &msg)
{
    ui->fr_SwiftLauncherMain->showOverlayMessage(msg, 5000);
}

void CSwiftLauncher::ps_appendLogMessage(const CStatusMessage &message)
{
    ui->comp_SwiftLauncherLog->appendStatusMessageToList(message);
    if (message.getSeverity() == CStatusMessage::SeverityError)
    {
        this->ps_showStatusMessage(message);
    }
}

void CSwiftLauncher::ps_appendLogMessages(const CStatusMessageList &messages)
{
    ui->comp_SwiftLauncherLog->appendStatusMessagesToList(messages);
    if (messages.hasErrorMessages())
    {
        this->ps_showStatusMessage(messages.getErrorMessages().toSingleMessage());
    }
}

void CSwiftLauncher::ps_showMainPage()
{
    ui->sw_SwiftLauncher->setCurrentWidget(ui->pg_SwiftLauncherMain);
}

void CSwiftLauncher::ps_tabChanged(int current)
{
    if (current == static_cast<int>(PageUpdates))
    {
        ui->comp_DataUpdates->display();
    }
}

void CSwiftLauncher::ps_showLogPage()
{
    ui->sw_SwiftLauncher->setCurrentWidget(ui->pg_SwiftLauncherLog);
}

void CSwiftLauncher::ps_checkRunningApplicationsAndCore()
{
    // wait some time before buttons are enabled (allows startup)
    if (m_startCoreWaitCycles > 0) { m_startCoreWaitCycles--; }
    if (m_startMappingToolWaitCycles > 0) { m_startMappingToolWaitCycles--; }
    if (m_startGuiWaitCycles > 0) { m_startGuiWaitCycles--; }

    const CApplicationInfoList runningApps = sGui->getRunningApplications();
    const bool foundLocalCore = runningApps.containsApplication(CApplicationInfo::PilotClientCore);
    const bool foundLocalMappingTool = runningApps.containsApplication(CApplicationInfo::MappingTool);
    const bool foundLocalPilotClientGui = runningApps.containsApplication(CApplicationInfo::PilotClientGui);

    ui->tb_SwiftCore->setEnabled(!foundLocalCore && m_startCoreWaitCycles < 1);
    ui->tb_SwiftMappingTool->setEnabled(!foundLocalMappingTool && m_startMappingToolWaitCycles < 1);
    ui->tb_SwiftGui->setEnabled(!foundLocalPilotClientGui && m_startGuiWaitCycles < 1);
}

void CSwiftLauncher::ps_startWizard()
{
    const bool show = this->warnAboutOtherSwiftApplications();
    if (!show) { return; }
    if (!m_wizard)
    {
        m_wizard.reset(new CConfigurationWizard(this));
    }
    m_wizard->show();
}

void CSwiftLauncher::ps_onStyleSheetsChanged()
{
    this->initStyleSheet();
}
