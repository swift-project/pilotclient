/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#include <QMessageBox>
#include <QPixmap>
#include <QBitmap>
#include <QTimer>
#include <QProcess>
#include <QPushButton>
#include <QStringBuilder>
#include <QDesktopServices>
#include <QShortcut>
#include <QPointer>
#include <qcompilerdetection.h>

using namespace BlackConfig;
using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackCore::Data;
using namespace BlackCore::Vatsim;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;

CSwiftLauncher::CSwiftLauncher(QWidget *parent) :
    QDialog(parent, CEnableForFramelessWindow::modeToWindowFlags(CEnableForFramelessWindow::WindowNormal)),
    CEnableForFramelessWindow(CEnableForFramelessWindow::WindowFrameless, true, "framelessMainWindow", this),
    CIdentifiable(this),
    ui(new Ui::CSwiftLauncher)
{
    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
    sGui->registerMainApplicationWidget(this);
    ui->setupUi(this);
    this->init();
    connect(ui->tb_SwiftCore, &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_SwiftMappingTool, &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_SwiftGui,   &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_Database,   &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_BackToMain, &QToolButton::pressed, this, &CSwiftLauncher::showMainPage);
    connect(ui->tb_ConfigurationWizard, &QToolButton::pressed, this, &CSwiftLauncher::startWizard);
    connect(ui->tb_Launcher, &QToolBox::currentChanged, this, &CSwiftLauncher::tabChanged);

    connect(ui->rb_SwiftCoreAudioOnCore, &QRadioButton::released, this, &CSwiftLauncher::onCoreModeReleased);
    connect(ui->rb_SwiftCoreAudioOnGui,  &QRadioButton::released, this, &CSwiftLauncher::onCoreModeReleased);
    connect(ui->rb_SwiftStandalone,      &QRadioButton::released, this, &CSwiftLauncher::onCoreModeReleased);

    connect(ui->comp_UpdateInfo, &CUpdateInfoComponent::updateInfoAvailable,       this, &CSwiftLauncher::updateInfoAvailable,   Qt::QueuedConnection);
    connect(ui->comp_UpdateInfo, &CUpdateInfoComponent::newerPilotClientAvailable, this, &CSwiftLauncher::setHeaderInfo,         Qt::QueuedConnection);
    connect(ui->comp_DBusSelector, &CDBusServerAddressSelector::editingFinished,   this, &CSwiftLauncher::onDBusEditingFinished, Qt::QueuedConnection);
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CSwiftLauncher::onStyleSheetsChanged, Qt::QueuedConnection);

    const QShortcut *logPageShortCut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this, SLOT(showLogPage()));
    Q_UNUSED(logPageShortCut);

    // default from settings
    this->setDefaults();

    // periodically check
    connect(&m_checkTimer, &QTimer::timeout, this, &CSwiftLauncher::checkRunningApplicationsAndCore);
    m_checkTimer.setInterval(2500);
    m_checkTimer.start();

    // auto launch wizard
    if (sGui->isInstallerOptionSet())
    {
        const QPointer<CSwiftLauncher> myself(this);
        QTimer::singleShot(2500, this, [ = ]
        {
            if (!sGui || sGui->isShuttingDown() || !myself) { return; }
            myself->startWizard();
        });
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

void CSwiftLauncher::displayLatestNews(QNetworkReply *reply)
{
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(reply);
    if (nwReply->error() == QNetworkReply::NoError)
    {
        const QString html = nwReply->readAll().trimmed();
        if (html.isEmpty()) { return; }
        CLogMessage(this).info(u"Received news from '%1'") << nwReply->url().toString();
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
        CLogMessage(this).warning(u"Error received news from '%1'") << nwReply->url().toString();
    }
}

void CSwiftLauncher::updateInfoAvailable()
{
    this->setHeaderInfo(ui->comp_UpdateInfo->getLatestAvailablePilotClientArtifactForSelection());
    this->loadLatestNews();
}

void CSwiftLauncher::mousePressEvent(QMouseEvent *event)
{
    if (!handleMousePressEvent(event)) { QDialog::mousePressEvent(event); }
}

void CSwiftLauncher::init()
{
    sGui->initMainApplicationWidget(this);

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
    if (!sGui || sGui->isShuttingDown()) { return; }
    const QString s = sGui->getStyleSheetUtility().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameStandardWidget(),
        CStyleSheetUtility::fileNameSwiftLauncher()
    }
    );
    this->setStyleSheet(""); // clear, otherwise launcher crashing
    this->setStyleSheet(s);
}

void CSwiftLauncher::loadLatestNews()
{
    if (!sGui || sGui->isShuttingDown()) { return; }

    CFailoverUrlList newsUrls(sGui->getGlobalSetup().getSwiftLatestNewsUrls());
    const CUrl newsUrl(newsUrls.obtainNextWorkingUrl(true, 10 * 1000));
    // const CUrl newsUrl("https://dev.swift-project.org/phame/blog/view/1/?__print__=1");

    if (newsUrl.isEmpty())
    {
        CLogMessage(this).warning(u"No working news URL in %1") << newsUrls.toQString();
        return;
    }
    sGui->getFromNetwork(newsUrl, { this, &CSwiftLauncher::displayLatestNews});
}

void CSwiftLauncher::initLogDisplay()
{
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    auto logHandler = CLogHandler::instance()->handlerForPattern(
                          CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
                      );
    logHandler->subscribe(this, &CSwiftLauncher::appendLogMessage);

    ui->comp_SwiftLauncherLog->showFilterBar();
    ui->comp_SwiftLauncherLog->filterUseRadioButtonDescriptiveIcons(false);
}

void CSwiftLauncher::setHeaderInfo(const CArtifact &latestArtifact)
{
    const bool isNewer = latestArtifact.isNewerThanCurrentBuild();
    ui->lbl_HeaderInfo->setVisible(isNewer);
    if (isNewer)
    {
        static const QString t("New version '%1' ['%2'/'%3']");
        ui->lbl_HeaderInfo->setText(
            t.arg(latestArtifact.getVersion(), latestArtifact.getPlatform().getPlatformName(),
                  latestArtifact.getMostStableDistribution().getChannel()));
        ui->lbl_HeaderInfo->setStyleSheet("background: red; color: yellow;");
    }
}

bool CSwiftLauncher::setSwiftCoreExecutable()
{
    if (!sGui || sGui->isShuttingDown()) { return false; }
    this->saveSetup();
    QStringList args = ui->comp_DBusSelector->getDBusCmdLineArgs();
    if (ui->rb_SwiftCoreAudioOnCore->isChecked())
    {
        args.append("--coreaudio");
    }

    m_executableArgs = sGui->argumentsJoined(args);
    m_executable = CDirectoryUtils::executableFilePath(CBuildConfig::swiftCoreExecutableName());
    return true;
}

bool CSwiftLauncher::setSwiftDataExecutable()
{
    m_executable = CDirectoryUtils::executableFilePath(CBuildConfig::swiftDataExecutableName());
    m_executableArgs = sGui->argumentsJoined({}, CNetworkVatlib::vatlibArguments());
    return true;
}

bool CSwiftLauncher::setSwiftGuiExecutable()
{
    if (!sGui || sGui->isShuttingDown()) { return false; }
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
                                   "DBus server for '" + dBus + "' can not be connected.\n" +
                                   "Likely the core is not running or is not reachable.\n" +
                                   "Details: " + msg, true);
            this->showStatusMessage(m);
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
        u"While using the wizard no other application should run.\nClose applications and try again.\nCurrently running: " %
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

void CSwiftLauncher::startButtonPressed()
{
    const QObject *sender = QObject::sender();
    const qreal scaleFactor = ui->comp_Scale->getScaleFactor();
    CGuiApplication::highDpiScreenSupport(scaleFactor);

    const Qt::KeyboardModifiers km = QGuiApplication::queryKeyboardModifiers();
    const bool shift = km.testFlag(Qt::ShiftModifier);

    if (sender == ui->tb_SwiftGui)
    {
        if (this->setSwiftGuiExecutable())
        {
            if (shift)
            {
                this->popupExecutableArgs();
            }
            else
            {
                this->accept();
            }
        }
    }
    else if (sender == ui->tb_SwiftMappingTool)
    {
        ui->tb_SwiftMappingTool->setEnabled(false);
        m_startMappingToolWaitCycles = 2;
        if (this->setSwiftDataExecutable())
        {
            if (shift)
            {
                this->popupExecutableArgs();
            }
            else
            {
                this->accept();
            }
        }
    }
    else if (sender == ui->tb_SwiftCore)
    {
        if (this->isStandaloneGuiSelected()) { ui->rb_SwiftCoreAudioOnGui->setChecked(true); }
        ui->tb_SwiftCore->setEnabled(false);
        m_startCoreWaitCycles = 2;
        if (this->setSwiftCoreExecutable())
        {
            if (shift)
            {
                this->popupExecutableArgs();
            }
            else
            {
                this->startDetached();
            }
        }
    }
    else if (sender == ui->tb_Database)
    {
        const CUrl homePage(sApp->getGlobalSetup().getDbHomePageUrl());
        QDesktopServices::openUrl(homePage);
    }
}

void CSwiftLauncher::dbusServerModeSelected(bool selected)
{
    if (!selected) { return; }
    if (!this->isStandaloneGuiSelected()) { return; }
    ui->rb_SwiftCoreAudioOnGui->setChecked(true);
}

void CSwiftLauncher::showStatusMessage(const CStatusMessage &msg)
{
    ui->fr_SwiftLauncherMain->showOverlayMessage(msg, 5000);
}

void CSwiftLauncher::showStatusMessage(const QString &htmlMsg)
{
    ui->fr_SwiftLauncherMain->showOverlayMessage(htmlMsg, 5000);
}

void CSwiftLauncher::appendLogMessage(const CStatusMessage &message)
{
    ui->comp_SwiftLauncherLog->appendStatusMessageToList(message);
    if (message.getSeverity() == CStatusMessage::SeverityError)
    {
        this->showStatusMessage(message);
    }
}

void CSwiftLauncher::appendLogMessages(const CStatusMessageList &messages)
{
    ui->comp_SwiftLauncherLog->appendStatusMessagesToList(messages);
    if (messages.hasErrorMessages())
    {
        this->showStatusMessage(messages.getErrorMessages().toSingleMessage());
    }
}

void CSwiftLauncher::showMainPage()
{
    ui->sw_SwiftLauncher->setCurrentWidget(ui->pg_SwiftLauncherMain);
}

void CSwiftLauncher::tabChanged(int current)
{
    if (current == static_cast<int>(PageUpdates))
    {
        ui->comp_DataUpdates->display();
    }
}

void CSwiftLauncher::showLogPage()
{
    ui->sw_SwiftLauncher->setCurrentWidget(ui->pg_SwiftLauncherLog);
}

void CSwiftLauncher::checkRunningApplicationsAndCore()
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

void CSwiftLauncher::startWizard()
{
    const bool show = this->warnAboutOtherSwiftApplications();
    if (!show) { return; }
    if (!m_wizard)
    {
        m_wizard.reset(new CConfigurationWizard(this));
    }
    m_wizard->show();
}

void CSwiftLauncher::onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void CSwiftLauncher::onDBusEditingFinished()
{
    ui->rb_SwiftCoreAudioOnGui->setChecked(true);
}

void CSwiftLauncher::onCoreModeReleased()
{
    ui->comp_DBusSelector->setEnabled(!ui->rb_SwiftStandalone->isChecked());
}

void CSwiftLauncher::popupExecutableArgs()
{
    QMessageBox::information(this, "Command line", this->getCmdLine());
}
