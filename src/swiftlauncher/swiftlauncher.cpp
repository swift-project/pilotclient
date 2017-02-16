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
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/setupreader.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include <QPixmap>
#include <QBitmap>
#include <QTimer>
#include <QProcess>
#include <QDesktopServices>
#include <QShortcut>
#include <qcompilerdetection.h>

using namespace BlackConfig;
using namespace BlackGui;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackMisc;
using namespace BlackMisc::Network;

CSwiftLauncher::CSwiftLauncher(QWidget *parent) :
    QDialog(parent, CEnableForFramelessWindow::modeToWindowFlags(CEnableForFramelessWindow::WindowNormal)),
    CEnableForFramelessWindow(CEnableForFramelessWindow::WindowFrameless, true, "framelessMainWindow", this),
    ui(new Ui::CSwiftLauncher)
{
    ui->setupUi(this);
    this->init();
    connect(ui->pb_CheckForUpdates, &QPushButton::pressed, this, &CSwiftLauncher::ps_loadSetup);
    connect(ui->tb_SwiftCore, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_SwiftMappingTool, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_SwiftGui, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_Database, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_BackToMain, &QToolButton::pressed, this, &CSwiftLauncher::ps_showMainPage);
    connect(ui->tb_Launcher, &QToolBox::currentChanged, this, &CSwiftLauncher::ps_tabChanged);

    // use version signal as trigger for completion
    connect(sGui, &CApplication::updateInfoAvailable, this, &CSwiftLauncher::ps_loadedUpdateInfo);
    QTimer::singleShot(10 * 1000, this, [ = ]
    {
        if (m_updateInfoLoaded) { return; }
        this->ps_loadedUpdateInfo(true); // failover
    });

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this, SLOT(ps_showLogPage()));
    ui->le_DBusServerPort->setValidator(new QIntValidator(0, 65535, this));

    // default from settings
    const QString dbus(m_dbusServerAddress.getThreadLocal());
    this->setDefault(dbus);

    // periodically check
    connect(&m_checkTimer, &QTimer::timeout, this, &CSwiftLauncher::ps_checkRunningApplications);
    m_checkTimer.setInterval(5000);
    m_checkTimer.start();
}


CSwiftLauncher::~CSwiftLauncher()
{ }

QString CSwiftLauncher::getCmdLine() const
{
    return toCmdLine(m_executable, m_executableArgs);
}

CEnableForFramelessWindow::WindowMode CSwiftLauncher::getWindowMode() const
{
    if (ui->rb_WindowFrameless->isChecked()) { return CEnableForFramelessWindow::WindowFrameless; }
    return CEnableForFramelessWindow::WindowNormal;
}

CoreModes::CoreMode CSwiftLauncher::getCoreMode() const
{
    if (ui->rb_SwiftStandalone->isChecked()) { return CoreModes::CoreInGuiProcess; }
    if (ui->rb_SwiftCoreAudio->isChecked()) { return CoreModes::CoreExternalCoreAudio; }
    if (ui->rb_SwiftCoreGuiAudio->isChecked()) { return CoreModes::CoreExternalAudioGui; }

    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
    return CoreModes::CoreInGuiProcess;
}

QString CSwiftLauncher::getDBusAddress() const
{
    if (ui->rb_DBusSession->isChecked()) { return CDBusServer::sessionBusAddress(); }
    if (ui->rb_DBusSystem->isChecked()) { return CDBusServer::systemBusAddress(); }
    return CDBusServer::p2pAddress(
               ui->cb_DBusServerAddress->currentText() + ":" +
               ui->le_DBusServerPort->text()
           );
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
        ui->tbr_LatestNews->setHtml(html);
        constexpr qint64 newNews = 72 * 3600 * 1000;
        const qint64 deltaT = CNetworkUtils::lastModifiedSinceNow(nwReply.data());
        if (deltaT > 0 && deltaT < newNews)
        {
            ui->tb_Launcher->setCurrentWidget(ui->pg_LatestNews);
        }
    }
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
    m_mwaLogComponent = ui->fr_SwiftLauncherLog;

    ui->lbl_NewVersionUrl->setTextFormat(Qt::RichText);
    ui->lbl_NewVersionUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->lbl_NewVersionUrl->setOpenExternalLinks(true);

    ui->wi_NewVersionAvailable->setVisible(false);
    ui->wi_NoNewVersion->setVisible(true);

    this->initStyleSheet();
    this->initLogDisplay();
    this->initDBusGui();
    this->initVersion();

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
    const CUrl newsUrl(newsUrls.obtainNextWorkingUrl());
    if (newsUrl.isEmpty()) { return; }
    sGui->getFromNetwork(newsUrl, { this, &CSwiftLauncher::ps_displayLatestNews});
}

void CSwiftLauncher::loadAbout()
{
    // workaround:
    // 1) Only reading as HTML gives proper formatting
    // 2) Reading the file resource fails (likely because of the style sheet)
    static const QString html = CFileUtils::readFileToString(CBuildConfig::getAboutFileLocation());
    static const QString legalDir = sGui->getGlobalSetup().getLegalDirectoryUrl().getFullUrl();
    // make links absolute
    static const QString htmlFixed = QString(html).
                                     replace(QLatin1Literal("href=\"./"), "href=\"" + legalDir);
    ui->tbr_About->setHtml(htmlFixed);
}

void CSwiftLauncher::initDBusGui()
{
    ui->cb_DBusServerAddress->addItem("127.0.0.1");
    ui->cb_DBusServerAddress->addItems(CNetworkUtils::getKnownLocalIpV4Addresses());
    ui->cb_DBusServerAddress->setCurrentIndex(0);

    connect(ui->cb_DBusServerAddress, &QComboBox::currentTextChanged, this, &CSwiftLauncher::ps_dbusServerAddressSelectionChanged);
    connect(ui->rb_DBusP2P, &QRadioButton::clicked, this, &CSwiftLauncher::ps_dbusServerModeSelected);
    connect(ui->rb_DBusSession, &QRadioButton::clicked, this, &CSwiftLauncher::ps_dbusServerModeSelected);
    connect(ui->rb_DBusSystem, &QRadioButton::clicked, this, &CSwiftLauncher::ps_dbusServerModeSelected);
}

void CSwiftLauncher::initVersion()
{
    ui->le_CurrentVersion->setText(sGui->versionStringDevBetaInfo());
}

void CSwiftLauncher::initLogDisplay()
{
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    auto logHandler = CLogHandler::instance()->handlerForPattern(
                          CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
                      );
    logHandler->subscribe(this, &CSwiftLauncher::ps_appendLogMessage);
}

void CSwiftLauncher::startSwiftCore()
{
    const QString dBus(this->getDBusAddress());
    m_dbusServerAddress.setAndSave(dBus);
    QStringList args(
    {
        "--start",
        "--dbus", dBus
    });

    if (ui->rb_SwiftCoreAudio->isChecked())
    {
        args.append("--coreaudio");
    }

    // I set this for debug purpose only
    m_executableArgs = args;
    m_executable.clear();
    if (CBuildConfig::isRunningOnUnixPlatform()) { m_executable += "./"; }
    m_executable += CBuildConfig::swiftCoreExecutableName();
    CLogMessage(this).info(this->getCmdLine());

    // start
    QProcess::startDetached(m_executable, m_executableArgs);
}

void CSwiftLauncher::setSwiftDataExecutable()
{
    m_executable.clear();
    if (CBuildConfig::isRunningOnUnixPlatform()) { m_executable += "./"; }
    m_executable += CBuildConfig::swiftDataExecutableName();
    m_executableArgs.clear();
}

bool CSwiftLauncher::setSwiftGuiExecutable()
{
    QString msg;
    if (this->isStandaloneGuiSelected() || this->canConnectDBusServer(msg))
    {
        m_executable.clear();
        if (CBuildConfig::isRunningOnUnixPlatform()) { m_executable += "./"; }
        m_executable += CBuildConfig::swiftGuiExecutableName();
        QStringList args
        {
            "--core", CoreModes::coreModeToString(getCoreMode()),
            "--window", CEnableForFramelessWindow::windowModeToString(getWindowMode())
        };
        if (!this->isStandaloneGuiSelected())
        {
            const QString dBus(this->getDBusAddress());
            m_dbusServerAddress.setAndSave(dBus);

            args.append("--dbus");
            args.append(dBus); // already converted
        }
        m_executableArgs = args;
        return true;
    }
    else
    {
        m_executable = CBuildConfig::swiftGuiExecutableName();
        m_executableArgs.clear();
        static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation() }));
        CStatusMessage m(cats, CStatusMessage::SeverityError,
                         "DBus server for " + getDBusAddress() + " can not be connected: " + msg);
        this->ps_showStatusMessage(m);
        return false;
    }
}

bool CSwiftLauncher::canConnectDBusServer(QString &msg) const
{
    if (this->isStandaloneGuiSelected()) { return true; } // do not mind here
    return CDBusServer::isDBusAvailable(getDBusAddress(), msg);
}

bool CSwiftLauncher::isStandaloneGuiSelected() const
{
    return ui->rb_SwiftStandalone->isChecked();
}

void CSwiftLauncher::setDefault(const QString &value)
{
    QString v(value.toLower().trimmed());
    if (v.isEmpty() || v.startsWith("session"))
    {
        ui->rb_DBusSession->setChecked(true);
    }
    else if (v.startsWith("sys"))
    {
        ui->rb_DBusSystem->setChecked(true);
    }
    else
    {
        ui->rb_DBusP2P->setChecked(true);
    }
}

QString CSwiftLauncher::toCmdLine(const QString &exe, const QStringList &exeArgs)
{
    if (exeArgs.isEmpty()) { return exe; }
    QString cmd(exe);
    for (const QString &a : exeArgs)
    {
        cmd = cmd.append(" ").append(a);
    }
    return cmd;
}

void CSwiftLauncher::ps_loadSetup()
{
    if (!ui->le_LatestVersion->text().isEmpty())
    {
        ui->le_LatestVersion->setText("");
        const CStatusMessageList msgs(sApp->requestReloadOfSetupAndVersion());
        this->ps_appendLogMessages(msgs);
    }
}

void CSwiftLauncher::ps_loadedUpdateInfo(bool success)
{
    if (!success)
    {
        CLogMessage(this).warning("Loading setup or version information failed");
        return;
    }

    m_updateInfoLoaded = true;
    const CUpdateInfo updateInfo(m_updateInfo.get());
    const QString latestVersion(updateInfo.getLatestVersion()) ; // need to get this from somewhere
    CFailoverUrlList downloadUrls(updateInfo.getDownloadUrls());
    const bool newVersionAvailable = CVersion::isNewerVersion(latestVersion) && !downloadUrls.isEmpty();
    ui->wi_NewVersionAvailable->setVisible(newVersionAvailable);
    ui->wi_NoNewVersion->setVisible(!newVersionAvailable);
    ui->le_LatestVersion->setText(latestVersion);
    ui->le_Channel->setText(updateInfo.getChannel());
    ui->lbl_HeaderInfo->setVisible(newVersionAvailable);
    ui->lbl_HeaderInfo->setText("New version " + latestVersion + " available");
    ui->lbl_HeaderInfo->setStyleSheet("background: red; color: yellow;");

    if (!downloadUrls.isEmpty())
    {
        const CUrl downloadUrl(downloadUrls.obtainNextUrl());
        const QString urlStr(downloadUrl.toQString());
        const QString hl("<a href=\"%1\"><img src=\":/own/icons/own/drophere16.png\"></a>");
        ui->lbl_NewVersionUrl->setText(hl.arg(urlStr));
        ui->lbl_NewVersionUrl->setToolTip("Download " + latestVersion);
    }

    this->loadLatestNews();
    this->loadAbout();
}

void CSwiftLauncher::ps_changedUpdateInfoCache()
{
    this->ps_loadedUpdateInfo(true);
}

void CSwiftLauncher::ps_startButtonPressed()
{
    QObject *sender = QObject::sender();
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
        if (this->isStandaloneGuiSelected()) { ui->rb_SwiftCoreGuiAudio->setChecked(true); }
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

void CSwiftLauncher::ps_dbusServerAddressSelectionChanged(const QString &currentText)
{
    Q_UNUSED(currentText);
    if (this->isStandaloneGuiSelected())
    {
        ui->rb_SwiftCoreGuiAudio->setChecked(true);
    }
    ui->rb_DBusP2P->setChecked(true);
}

void CSwiftLauncher::ps_dbusServerModeSelected(bool selected)
{
    if (!selected) { return; }
    if (!this->isStandaloneGuiSelected()) { return; }
    ui->rb_SwiftCoreGuiAudio->setChecked(true);
}

void CSwiftLauncher::ps_showStatusMessage(const CStatusMessage &msg)
{
    ui->fr_SwiftLauncherMain->showOverlayMessage(msg, 5000);
}

void CSwiftLauncher::ps_appendLogMessage(const CStatusMessage &message)
{
    ui->fr_SwiftLauncherLog->appendStatusMessageToList(message);
    if (message.getSeverity() == CStatusMessage::SeverityError)
    {
        this->ps_showStatusMessage(message);
    }
}

void CSwiftLauncher::ps_appendLogMessages(const CStatusMessageList &messages)
{
    ui->fr_SwiftLauncherLog->appendStatusMessagesToList(messages);
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

void CSwiftLauncher::ps_checkRunningApplications()
{
    const CApplicationInfoList runningApps = sGui->getRunningApplications();
    if (m_startCoreWaitCycles > 0) { m_startCoreWaitCycles--; }
    else { ui->tb_SwiftCore->setEnabled(true); }
    if (m_startMappingToolWaitCycles > 0) { m_startMappingToolWaitCycles--; }
    else { ui->tb_SwiftMappingTool->setEnabled(true); }

    for (const CApplicationInfo &info : runningApps)
    {
        switch (info.application())
        {
        case CApplicationInfo::PilotClientCore :
            ui->tb_SwiftCore->setEnabled(false);
            break;
        case CApplicationInfo::MappingTool :
            ui->tb_SwiftMappingTool->setEnabled(false);
            break;
        default:
            break;
        }
    }
}
