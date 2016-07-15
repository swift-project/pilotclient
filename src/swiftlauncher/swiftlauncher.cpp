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
    connect(ui->tb_SwiftData, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_SwiftGui, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_Database, &QPushButton::pressed, this, &CSwiftLauncher::ps_startButtonPressed);
    connect(ui->tb_BackToMain, &QToolButton::pressed, this, &CSwiftLauncher::ps_showMainPage);

    // use version signal as trigger for completion
    this->ps_loadedUpdateInfo(true); // defaults from settings, overridden by signal/slot when changed
    connect(sGui, &CApplication::updateInfoAvailable, this, &CSwiftLauncher::ps_loadedUpdateInfo);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this, SLOT(ps_showLogPage()));
    this->ui->le_DBusServerPort->setValidator(new QIntValidator(0, 65535, this));

    // default from settings
    const QString dbus(this->m_dbusServerAddress.getThreadLocal());
    this->setDefault(dbus);
}

CSwiftLauncher::~CSwiftLauncher()
{ }

QString CSwiftLauncher::getCmdLine() const
{
    return toCmdLine(this->m_executable, this->m_executableArgs);
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
    if (this->ui->rb_DBusSession->isChecked()) { return CDBusServer::sessionBusAddress(); }
    if (this->ui->rb_DBusSystem->isChecked()) { return CDBusServer::systemBusAddress(); }
    return CDBusServer::p2pAddress(
               this->ui->cb_DBusServerAddress->currentText() + ":" +
               this->ui->le_DBusServerPort->text()
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
        this->ui->te_LatestNews->setHtml(html);
    }
}

void CSwiftLauncher::mousePressEvent(QMouseEvent *event)
{
    if (!handleMousePressEvent(event)) { QDialog::mousePressEvent(event); }
}

void CSwiftLauncher::init()
{
    sGui->initMainApplicationWindow(this);

    this->m_mwaOverlayFrame = this->ui->fr_SwiftLauncherMain;
    this->m_mwaStatusBar = nullptr;
    this->m_mwaLogComponent = this->ui->fr_SwiftLauncherLog;

    this->ui->lbl_NewVersionUrl->setTextFormat(Qt::RichText);
    this->ui->lbl_NewVersionUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    this->ui->lbl_NewVersionUrl->setOpenExternalLinks(true);

    this->ui->wi_NewVersionAvailable->setVisible(false);
    this->ui->wi_NoNewVersion->setVisible(true);

    this->initStyleSheet();
    this->initLogDisplay();
    this->initDBusGui();
    this->initVersion();
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

void CSwiftLauncher::initDBusGui()
{
    this->ui->cb_DBusServerAddress->addItem("127.0.0.1");
    this->ui->cb_DBusServerAddress->addItems(CNetworkUtils::getKnownIpAddresses());
    this->ui->cb_DBusServerAddress->setCurrentIndex(0);

    connect(ui->cb_DBusServerAddress, &QComboBox::currentTextChanged, this, &CSwiftLauncher::ps_dbusServerAddressSelectionChanged);
    connect(ui->rb_DBusP2P, &QRadioButton::clicked, this, &CSwiftLauncher::ps_dbusServerModeSelected);
    connect(ui->rb_DBusSession, &QRadioButton::clicked, this, &CSwiftLauncher::ps_dbusServerModeSelected);
    connect(ui->rb_DBusSystem, &QRadioButton::clicked, this, &CSwiftLauncher::ps_dbusServerModeSelected);
}

void CSwiftLauncher::initVersion()
{
    this->ui->le_CurrentVersion->setText(sGui->versionStringDevBetaInfo());
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
    this->m_dbusServerAddress.setAndSave(dBus);
    QStringList args(
    {
        "--start",
        "--dbus", dBus
    });

    if (this->ui->rb_SwiftCoreAudio->isChecked())
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
    QProcess::startDetached(this->m_executable, this->m_executableArgs);
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
            this->m_dbusServerAddress.setAndSave(dBus);

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
    return this->ui->rb_SwiftStandalone->isChecked();
}

void CSwiftLauncher::setDefault(const QString &value)
{
    QString v(value.toLower().trimmed());
    if (v.isEmpty() || v.startsWith("session"))
    {
        this->ui->rb_DBusSession->setChecked(true);
    }
    else if (v.startsWith("sys"))
    {
        this->ui->rb_DBusSystem->setChecked(true);
    }
    else
    {
        this->ui->rb_DBusP2P->setChecked(true);
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
    if (!this->ui->le_LatestVersion->text().isEmpty())
    {
        this->ui->le_LatestVersion->setText("");
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

    const CUpdateInfo updateInfo(this->m_updateInfo.get());
    const QString latestVersion(updateInfo.getLatestVersion()) ; // need to get this from somewhere
    CFailoverUrlList downloadUrls(updateInfo.getDownloadUrls());
    bool newVersionAvailable = CVersion::isNewerVersion(latestVersion) && !downloadUrls.isEmpty();
    this->ui->wi_NewVersionAvailable->setVisible(newVersionAvailable);
    this->ui->wi_NoNewVersion->setVisible(!newVersionAvailable);
    this->ui->le_LatestVersion->setText(latestVersion);

    if (!downloadUrls.isEmpty())
    {
        const CUrl downloadUrl(downloadUrls.obtainNextUrl());
        const QString urlStr(downloadUrl.toQString());
        QString hl("<a href=\"%1\">%2 %3</a>");
        this->ui->lbl_NewVersionUrl->setText(hl.arg(urlStr).arg(urlStr).arg(latestVersion));
    }

    this->loadLatestNews();
}

void CSwiftLauncher::ps_changedUpdateInfoCache()
{
    this->ps_loadedUpdateInfo(true);
}

void CSwiftLauncher::ps_startButtonPressed()
{
    QObject *sender = QObject::sender();
    if (sender == this->ui->tb_SwiftGui)
    {
        if (this->setSwiftGuiExecutable())
        {
            this->accept();
        }
    }
    else if (sender == this->ui->tb_SwiftData)
    {
        this->setSwiftDataExecutable();
        this->accept();
    }
    else if (sender == this->ui->tb_SwiftCore)
    {
        if (this->isStandaloneGuiSelected()) { this->ui->rb_SwiftCoreGuiAudio->setChecked(true); }
        this->startSwiftCore();
    }
    else if (sender == this->ui->tb_Database)
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
        this->ui->rb_SwiftCoreGuiAudio->setChecked(true);
    }
    this->ui->rb_DBusP2P->setChecked(true);
}

void CSwiftLauncher::ps_dbusServerModeSelected(bool selected)
{
    if (!selected) { return; }
    if (!this->isStandaloneGuiSelected()) { return; }
    this->ui->rb_SwiftCoreGuiAudio->setChecked(true);
}

void CSwiftLauncher::ps_showStatusMessage(const CStatusMessage &msg)
{
    this->ui->fr_SwiftLauncherMain->showOverlayMessage(msg, 5000);
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
    this->ui->sw_SwiftLauncher->setCurrentWidget(this->ui->pg_SwiftLauncherMain);
}

void CSwiftLauncher::ps_showLogPage()
{
    this->ui->sw_SwiftLauncher->setCurrentWidget(this->ui->pg_SwiftLauncherLog);
}
