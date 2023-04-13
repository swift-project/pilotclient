/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "swiftdata.h"
#include "blackgui/components/datamaininfoareacomponent.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/datasettingscomponent.h"
#include "blackgui/components/autopublishdialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/simulation/autopublishdata.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/sharedstate/datalinkdbus.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logpattern.h"
#include "blackmisc/network/url.h"
#include "blackconfig/buildconfig.h"
#include "ui_swiftdata.h"

#include <QAction>
#include <QString>
#include <QStyle>
#include <QtGlobal>
#include <QVersionNumber>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackCore::Db;
using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackConfig;

CSwiftData::CSwiftData(QWidget *parent) : QMainWindow(parent),
                                          CIdentifiable(this),
                                          ui(new Ui::CSwiftData)
{

    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
    sGui->registerMainApplicationWidget(this);
    ui->setupUi(this);
    this->init();
}

void CSwiftData::initStyleSheet()
{
    const QString s = sGui->getStyleSheetUtility().styles(
        { CStyleSheetUtility::fileNameFonts(),
          CStyleSheetUtility::fileNameStandardWidget(),
          CStyleSheetUtility::fileNameSwiftData() });

    this->setStyleSheet("");
    this->setStyleSheet(s);
}

CSwiftData::~CSwiftData()
{}

void CSwiftData::closeEvent(QCloseEvent *event)
{
    if (sGui)
    {
        // save settings?
        if (sGui->showCloseDialog(this, event) == QDialog::Rejected) { return; }
    }
    this->performGracefulShutdown();
}

void CSwiftData::onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void CSwiftData::init()
{
    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
    sGui->initMainApplicationWidget(this);
    this->initLogDisplay();

    m_mwaLogComponent = ui->comp_MainInfoArea->getLogComponent();
    m_mwaStatusBar = &m_statusBar;
    m_mwaOverlayFrame = ui->comp_MainInfoArea->getMappingComponent();

    this->initStyleSheet();
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CSwiftData::onStyleSheetsChanged, Qt::QueuedConnection);
    this->initMenu();

    // update title
    const CGlobalSetup s(sApp->getGlobalSetup());
    if (!s.getDbHomePageUrl().isEmpty())
    {
        this->setWindowTitle(QStringLiteral("%1 %2").arg(this->windowTitle(), s.getDbHomePageUrl().toQString(true)));
    }

    sGui->triggerNewVersionCheck(20 * 1000);
    QPointer<CSwiftData> myself(this);
    QTimer::singleShot(15 * 1000, this, [=] {
        if (!myself || !sGui || sGui->isShuttingDown()) { return; }
        this->checkMinimumVersion();
        this->checkAutoPublishing();
    });

    emit sGui->startUpCompleted(true);
}

void CSwiftData::initLogDisplay()
{
    m_statusBar.initStatusBar(ui->sb_SwiftData);
    // m_statusBar.setSizeGripEnabled(false);

    connect(&m_logHistory, &CLogHistoryReplica::elementAdded, this, [this](const CStatusMessage &message) {
        m_statusBar.displayStatusMessage(message);
    });
    m_logHistory.setFilter(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo));
    m_logHistory.initialize(sApp->getDataLinkDBus());

    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
}

void CSwiftData::initMenu()
{
    // menu

    this->initDynamicMenus();
    ui->menu_WindowMinimize->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarMinButton));

    connect(ui->menu_WindowFont, &QAction::triggered, this, &CSwiftData::onMenuClicked);
    connect(ui->menu_MappingMaxData, &QAction::triggered, this, &CSwiftData::onMenuClicked);
    connect(ui->menu_MappingMaxMapping, &QAction::triggered, this, &CSwiftData::onMenuClicked);

    sGui->addMenuFile(*ui->menu_File);
    sGui->addMenuInternals(*ui->menu_Internals);
    sGui->addMenuWindow(*ui->menu_Window);
    sGui->addMenuHelp(*ui->menu_Help);
}

void CSwiftData::performGracefulShutdown()
{
    if (m_updater) { m_updater->abandonAndWait(); }
}

void CSwiftData::consolidationSettingChanged()
{
    const int consolidationSecs = m_consolidationSettings.get();
    if (consolidationSecs < 0)
    {
        if (m_updater)
        {
            ui->comp_MainInfoArea->getDataSettingsComponent()->setBackgroundUpdater(nullptr);
            disconnect(m_updater);
            m_updater->abandonAndWait();
            m_updater = nullptr;
        }
    }
    else
    {
        if (!m_updater)
        {
            m_updater = new CBackgroundDataUpdater(this);
            connect(m_updater, &CBackgroundDataUpdater::consolidating, ui->comp_InfoBar, &CInfoBarWebReadersStatusComponent::consolidationRunning, Qt::QueuedConnection);
            m_updater->start(QThread::LowestPriority);
            ui->comp_MainInfoArea->getDataSettingsComponent()->setBackgroundUpdater(m_updater);
        }
        m_updater->startUpdating(consolidationSecs);
    }
}

void CSwiftData::displayConsole()
{
    ui->comp_MainInfoArea->displayConsole();
}

void CSwiftData::displayLog()
{
    ui->comp_MainInfoArea->displayLog();
}

void CSwiftData::checkMinimumVersion()
{
    if (!sApp || sApp->isShuttingDown()) { return; }
    if (sApp->getGlobalSetup().isSwiftVersionMinimumMappingVersion())
    {
        CLogMessage(this).info(u"Checked mapping tool version, required '%1', this version '%2'") << sApp->getGlobalSetup().getMappingMinimumVersionString() << CBuildConfig::getVersionString();
    }
    else
    {
        const CStatusMessage sm = CStatusMessage(this, CStatusMessage::SeverityWarning, u"Your are using swift version: '%1'. Creating mappings requires at least '%2'.") << CBuildConfig::getVersionString() << sApp->getGlobalSetup().getMappingMinimumVersionString();
        CLogMessage::preformatted(sm);
        this->displayInOverlayWindow(sm);
    }
}

void CSwiftData::checkAutoPublishing()
{
    if (!CAutoPublishData::existAutoPublishFiles()) { return; }
    this->showAutoPublishing();
}

void CSwiftData::showAutoPublishing()
{
    if (!sApp || sApp->isShuttingDown()) { return; }
    if (!m_autoPublishDialog) { m_autoPublishDialog = new CAutoPublishDialog(this); }
    m_autoPublishDialog->readAndShow();
}
