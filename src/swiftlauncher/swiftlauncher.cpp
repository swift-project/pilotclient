/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "swiftlauncher.h"
#include "ui_swiftlauncher.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/components/configurationwizard.h"
#include "blackgui/components/texteditdialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/context/contextapplicationproxy.h"
#include "blackcore/setupreader.h"
#include "blacksound/audioutilities.h"
#include "blackmisc/simulation/fscommon/fsdirectories.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/sharedstate/datalinkdbus.h"
#include "blackconfig/buildconfig.h"

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
using namespace BlackSound;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;
using namespace BlackMisc::Simulation::FsCommon;

CSwiftLauncher::CSwiftLauncher(QWidget *parent) :
    QDialog(parent, CEnableForFramelessWindow::modeToWindowFlags(CEnableForFramelessWindow::WindowNormal)),
    CEnableForFramelessWindow(CEnableForFramelessWindow::WindowFrameless, true, "framelessMainWindow", this),
    CCentralMultiSimulatorModelSetCachesAware(),
    CIdentifiable(this),
    ui(new Ui::CSwiftLauncher)
{
    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
    sGui->registerMainApplicationWidget(this);
    ui->setupUi(this);
    this->init(); // reads also defaults from settings

    connect(ui->tb_SwiftCore,           &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_SwiftMappingTool,    &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_SwiftGui,            &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_Database,            &QPushButton::pressed, this, &CSwiftLauncher::startButtonPressed);
    connect(ui->tb_BackToMain,          &QToolButton::pressed, this, &CSwiftLauncher::showMainPage);
    connect(ui->tb_ConfigurationWizard, &QToolButton::pressed, this, &CSwiftLauncher::startWizard);
    connect(ui->tb_Launcher,            &QToolBox::currentChanged, this, &CSwiftLauncher::tabChanged);

    connect(ui->rb_SwiftDistributed, &QRadioButton::released, this, &CSwiftLauncher::onCoreModeReleased, Qt::QueuedConnection);
    connect(ui->rb_SwiftStandalone,  &QRadioButton::released, this, &CSwiftLauncher::onCoreModeReleased, Qt::QueuedConnection);

    connect(ui->comp_UpdateInfo,   &CUpdateInfoComponent::updateInfoAvailable,       this, &CSwiftLauncher::updateInfoAvailable,   Qt::QueuedConnection);
    connect(ui->comp_UpdateInfo,   &CUpdateInfoComponent::newerPilotClientAvailable, this, &CSwiftLauncher::setHeaderInfo,         Qt::QueuedConnection);
    connect(ui->comp_DBusSelector, &CDBusServerAddressSelector::editingFinished,     this, &CSwiftLauncher::onDBusEditingFinished, Qt::QueuedConnection);
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CSwiftLauncher::onStyleSheetsChanged, Qt::QueuedConnection);

    connect(ui->pb_LogDir,        &QPushButton::released, sGui, &CGuiApplication::openStandardLogDirectory, Qt::QueuedConnection);
    connect(ui->pb_DumpDir,       &QPushButton::released, sGui, &CGuiApplication::openStandardCrashDumpDirectory, Qt::QueuedConnection);
    connect(ui->pb_Log,           &QPushButton::released, this, &CSwiftLauncher::showLogPage, Qt::QueuedConnection);
    connect(ui->pb_Log,           &QPushButton::released, this, &CSwiftLauncher::showLogPage, Qt::QueuedConnection);
    connect(ui->pb_FSXConfigDirs, &QPushButton::released, this, &CSwiftLauncher::showSimulatorConfigDirs, Qt::QueuedConnection);
    connect(ui->pb_P3DConfigDirs, &QPushButton::released, this, &CSwiftLauncher::showSimulatorConfigDirs, Qt::QueuedConnection);

    const QShortcut *logPageShortCut = new QShortcut(QKeySequence(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_L), this, SLOT(showLogPage()));
    Q_UNUSED(logPageShortCut)

    // periodically check
    connect(&m_checkTimer, &QTimer::timeout, this, &CSwiftLauncher::checkRunningApplicationsAndCore);
    m_checkTimer.setInterval(2500);
    m_checkTimer.start();

    // platform specific tool
    ui->gb_ToolsWindows->setEnabled(CBuildConfig::isRunningOnWindowsNtPlatform());
    if (CBuildConfig::isRunningOnWindowsNtPlatform())
    {
        connect(ui->pb_ClearRegistry, &QPushButton::released, this, &CSwiftLauncher::clearWindowsRegistry);
    }

    const QPointer<CSwiftLauncher> myself(this);
    if (sGui->isInstallerOptionSet())
    {
        QTimer::singleShot(1000, this, [ = ]
        {
            if (!sGui || sGui->isShuttingDown() || !myself) { return; }
            ui->fr_SwiftLauncherMain->showOverlayHTMLMessage("Checking installation!<br>One moment please ....");
            this->raise();
        });
    }

    // auto launch wizard and other init parts
    QTimer::singleShot(2500, this, [ = ]
    {
        if (!sGui || sGui->isShuttingDown() || !myself) { return; }
        this->onCoreModeReleased();
        this->requestMacMicrophoneAccess();
        this->installerMode();
    });
}

void CSwiftLauncher::installerMode()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (!sGui->isInstallerOptionSet())   { return; }

    bool runDialog = false;
    do
    {
        const QDir dir = CSwiftDirectories::logDirectory();
        if (!dir.exists()) { break; }

        if (sGui && sGui->getSetupReader())
        {
            sGui->getSetupReader()->prefillCacheWithLocalResourceBootstrapFile();
        }

        for (const CSimulatorInfo &sim : CSimulatorInfo::allSimulatorsSet())
        {
            this->synchronizeCache(sim);
            const int c = this->getCachedModelsCount(sim);
            if (c > 0)
            {
                // we already have data
                runDialog = true;
                break;
            }
        }
    }
    while (false);

    bool startWizard = true;
    ui->fr_SwiftLauncherMain->closeOverlay();

    if (runDialog)
    {
        const QMessageBox::StandardButton ret =
            QMessageBox::question(this,
                                  tr("swift configuration"),
                                  tr("This installation directory already contains a swift configuration.\n"
                                     "Do you want to use that one?"));
        if (ret != QMessageBox::No) { startWizard = false; }
    }

    if (startWizard) { this->startWizard(); }
}

void CSwiftLauncher::clearWindowsRegistry()
{
    if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return; }
    const QMessageBox::StandardButton ret = QMessageBox::warning(this,
                                            tr("Registry swift applications"),
                                            tr("Do you really want to delete all entries?\nThis cannot be undone!"),
                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                            QMessageBox::No);
    if (ret != QMessageBox::Yes) { return; }
    CGuiApplication::removeAllWindowsSwiftRegistryEntries();
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
    if (ui->rb_SwiftStandalone->isChecked())  { return CoreModes::Standalone; }
    if (ui->rb_SwiftDistributed->isChecked()) { return CoreModes::Distributed; }

    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
    return CoreModes::Standalone;
}

void CSwiftLauncher::mousePressEvent(QMouseEvent *event)
{
    if (!handleMousePressEvent(event)) { QDialog::mousePressEvent(event); }
}

void CSwiftLauncher::mouseMoveEvent(QMouseEvent *event)
{
    if (!handleMouseMoveEvent(event)) { QDialog::mouseMoveEvent(event); }
}

void CSwiftLauncher::mouseReleaseEvent(QMouseEvent *event)
{
    m_framelessDragPosition = QPoint();
    QDialog::mouseReleaseEvent(event);
}

void CSwiftLauncher::updateInfoAvailable()
{
    this->setHeaderInfo(ui->comp_UpdateInfo->getLatestAvailablePilotClientArtifactForSelection());
}

void CSwiftLauncher::init()
{
    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
    sGui->initMainApplicationWidget(this);

    m_mwaOverlayFrame = ui->fr_SwiftLauncherMain;
    m_mwaStatusBar = nullptr;
    m_mwaLogComponent = ui->comp_SwiftLauncherLog;

    this->initStyleSheet();
    this->initLogDisplay();
    this->setDefaults();

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

void CSwiftLauncher::initLogDisplay()
{
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable

    ui->comp_SwiftLauncherLog->showFilterBar();
    ui->comp_SwiftLauncherLog->filterUseRadioButtonDescriptiveIcons(false);

    m_logHistory.setFilter(CLogPattern().withSeverity(CStatusMessage::SeverityError));
    connect(&m_logHistory, &CLogHistoryReplica::elementAdded, this, qOverload<const CStatusMessage &>(&CSwiftLauncher::showStatusMessage));
    m_logHistory.initialize(sApp->getDataLinkDBus());
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
    if (ui->cb_ResetWindow->isChecked())      { args.append("--resetsize"); }
    if (ui->cb_DisableCoreAudio->isChecked()) { args.append("--noaudio"); }

    m_executableArgs = sGui->argumentsJoined(args);
    m_executable = CSwiftDirectories::executableFilePath(CBuildConfig::swiftCoreExecutableName());
    return true;
}

bool CSwiftLauncher::setSwiftDataExecutable()
{
    m_executable = CSwiftDirectories::executableFilePath(CBuildConfig::swiftDataExecutableName());

    QStringList fsdArgs;
    int id = 0;
    QString key;
    if (IContextNetwork::getCmdLineClientIdAndKey(id, key))
    {
        // from cmd. line
        fsdArgs << "--idAndKey";
        fsdArgs << sApp->getParserValue("clientIdAndKey"); // as typed in
    }

    m_executableArgs = sGui->argumentsJoined({}, fsdArgs);
    return true;
}

bool CSwiftLauncher::setSwiftGuiExecutable()
{
    if (!sGui || sGui->isShuttingDown()) { return false; }
    m_executable = CSwiftDirectories::executableFilePath(CBuildConfig::swiftGuiExecutableName());
    QStringList args
    {
        "--core", CoreModes::coreModeToString(getCoreMode()),
        "--window", CEnableForFramelessWindow::windowModeToString(getWindowMode())
    };

    if (ui->cb_ResetWindow->isChecked()) { args << "--resetsize"; }
    if (this->isStandaloneGuiSelected())
    {
        if (ui->cb_DisableSaAfv->isChecked()) { args.append("--noaudio"); }
    }
    else
    {
        if (ui->cb_DisableGUIAfv->isChecked()) { args.append("--noaudio"); }

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
    this->saveSetup();
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

    ui->rb_WindowFrameless->setChecked(setup.useFramelessWindow());
    ui->rb_WindowNormal->setChecked(!setup.useFramelessWindow());

    const CLauncherSetup::CoreMode mode = setup.getCoreMode();
    ui->rb_SwiftStandalone->setChecked(mode  == CLauncherSetup::Standalone  ? true : false);
    ui->rb_SwiftDistributed->setChecked(mode == CLauncherSetup::Distributed ? true : false);

    const CLauncherSetup::AudioMode audio = setup.getAudioMode();
    ui->cb_DisableCoreAudio->setChecked(audio.testFlag(CLauncherSetup::AudioDisableDistributedCoreAudio));
    ui->cb_DisableGUIAfv->setChecked(audio.testFlag(CLauncherSetup::AudioDisableDistributedGuiAudio));
    ui->cb_DisableSaAfv->setChecked(audio.testFlag(CLauncherSetup::AudioDisableStandaloneAudio));
}

void CSwiftLauncher::saveSetup()
{
    CLauncherSetup setup = m_setup.get();
    const QString dBus(ui->comp_DBusSelector->getDBusAddress());
    if (!dBus.isEmpty()) { setup.setDBusAddress(dBus); }
    setup.setFramelessWindow(ui->rb_WindowFrameless->isChecked());
    setup.setCoreMode(CLauncherSetup::Standalone);

    CLauncherSetup::AudioMode audio = CLauncherSetup::AudioNothingDisabled;
    audio.setFlag(CLauncherSetup::AudioDisableDistributedCoreAudio, ui->cb_DisableCoreAudio->isChecked());
    audio.setFlag(CLauncherSetup::AudioDisableDistributedGuiAudio,  ui->cb_DisableGUIAfv->isChecked());
    audio.setFlag(CLauncherSetup::AudioDisableStandaloneAudio,      ui->cb_DisableSaAfv->isChecked());
    setup.setAudioMode(audio);

    if (ui->rb_SwiftDistributed->isChecked())
    {
        setup.setCoreMode(CLauncherSetup::Distributed);
    }

    const CStatusMessage msg = m_setup.set(setup);
    Q_UNUSED(msg)
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
    CGuiApplication::highDpiScreenSupport(QString::number(scaleFactor, 'f', 4));

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
        if (this->isStandaloneGuiSelected()) { ui->rb_SwiftDistributed->setChecked(true); }
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
    ui->rb_SwiftDistributed->setChecked(true);
}

void CSwiftLauncher::showStatusMessage(const CStatusMessage &msg)
{
    ui->fr_SwiftLauncherMain->showOverlayMessage(msg, 5000);
}

void CSwiftLauncher::showStatusMessage(const QString &htmlMsg)
{
    ui->fr_SwiftLauncherMain->showOverlayMessage(htmlMsg, 5000);
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
    if (m_startCoreWaitCycles > 0)        { m_startCoreWaitCycles--; }
    if (m_startMappingToolWaitCycles > 0) { m_startMappingToolWaitCycles--; }
    if (m_startGuiWaitCycles > 0)         { m_startGuiWaitCycles--; }

    const CApplicationInfoList runningApps = sGui->getRunningApplications();
    const bool foundLocalCore           = runningApps.containsApplication(CApplicationInfo::PilotClientCore);
    const bool foundLocalMappingTool    = runningApps.containsApplication(CApplicationInfo::MappingTool);
    const bool foundLocalPilotClientGui = runningApps.containsApplication(CApplicationInfo::PilotClientGui);
    const bool standalone               = ui->rb_SwiftStandalone->isChecked();

    ui->tb_SwiftCore->setEnabled(!standalone && !foundLocalCore && m_startCoreWaitCycles < 1);
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
    CGuiUtility::centerWidget(m_wizard.data(), this);
}

void CSwiftLauncher::onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void CSwiftLauncher::onDBusEditingFinished()
{
    ui->rb_SwiftDistributed->setChecked(true);
}

void CSwiftLauncher::onCoreModeReleased()
{
    const bool sa = ui->rb_SwiftStandalone->isChecked();
    ui->comp_DBusSelector->setEnabled(!sa);
    ui->tb_SwiftCore->setEnabled(!sa);
    ui->gb_AudioSa->setEnabled(sa);
    ui->gb_AudioDistributed->setEnabled(!sa);
    this->saveSetup();
}

void CSwiftLauncher::popupExecutableArgs()
{
    QMessageBox::information(this, "Command line", this->getCmdLine());
}

void CSwiftLauncher::showSimulatorConfigDirs()
{
    if (!m_textEditDialog)
    {
        m_textEditDialog.reset(new CTextEditDialog(this));
    }

    const QObject *s = QObject::sender();
    QStringList dirs;
    QString simDir;
    QString simObjDir;

    if (s == ui->pb_P3DConfigDirs)
    {
        simDir    = CFsDirectories::p3dDir();
        simObjDir = CFsDirectories::p3dSimObjectsDir();
        const QString versionHint = CFsDirectories::guessP3DVersion(simDir);
        dirs      = CFsDirectories::p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(simObjDir, versionHint);
    }
    else if (s == ui->pb_FSXConfigDirs)
    {
        dirs      = CFsDirectories::fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths();
        simDir    = CFsDirectories::fsxDir();
        simObjDir = CFsDirectories::fsxSimObjectsDir();
    }

    const QString info = u"Sim.dir: " % simDir % "\n" %
                         u"Sim.objects: " % simObjDir % "\n" %
                         (dirs.isEmpty() ? "No dirs" : dirs.join("\n"));

    m_textEditDialog->setReadOnly();
    m_textEditDialog->textEdit()->setText(info);
    m_textEditDialog->show();
}

void CSwiftLauncher::requestMacMicrophoneAccess()
{
    // needed to be able to start core/GUI which need MIC access
    // https://discordapp.com/channels/539048679160676382/567983892791951374/634806582013591603
#ifdef Q_OS_MAC
    const CMacOSMicrophoneAccess::AuthorizationStatus status = m_micAccess.getAuthorizationStatus();
    if (status == CMacOSMicrophoneAccess::Authorized) { return; }
    m_micAccess.requestAccess();
    CLogMessage(this).info(u"MacOS requested input device");
#endif
}
