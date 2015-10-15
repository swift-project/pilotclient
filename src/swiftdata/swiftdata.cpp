/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftdata.h"
#include "ui_swiftdata.h"
#include "blackgui/components/datamaininfoareacomponent.h"
#include "blackgui/components/datainfoareacomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/enableforruntime.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/icon.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/project.h"
#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Components;

CSwiftData::CSwiftData(QWidget *parent) :
    QMainWindow(parent),
    CIdentifiable(this),
    ui(new Ui::CSwiftData),
    m_webDataReader(new CWebDataServices(CWebReaderFlags::AllSwiftDbReaders))
{
    ui->setupUi(this);
    this->init();
}

void CSwiftData::initStyleSheet()
{
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameStandardWidget(),
        CStyleSheetUtility::fileNameSwiftData()
    }
    );
    this->setStyleSheet(s);
}

CSwiftData::~CSwiftData()
{ }

void CSwiftData::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    this->performGracefulShutdown();
    QApplication::exit();
}

void CSwiftData::ps_appendLogMessage(const CStatusMessage &message)
{
    CLogComponent *logComponent = ui->comp_MainInfoArea->getLogComponent();
    Q_ASSERT_X(logComponent, Q_FUNC_INFO, "missing log component");
    logComponent->appendStatusMessageToList(message);

    // status bar
    m_statusBar.displayStatusMessage(message);
}

void CSwiftData::ps_onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void CSwiftData::init()
{
    this->setWindowIcon(CIcons::swiftDatabase24());
    this->setWindowTitle(QString("Mapping tool %1").arg(CProject::swiftVersionStringDevInfo()));
    this->setObjectName("CSwiftData");
    this->initStyleSheet();
    this->initLogDisplay();
    connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CSwiftData::ps_onStyleSheetsChanged);
    this->initReaders();
    this->initMenu();
}

void CSwiftData::initLogDisplay()
{
    this->m_statusBar.initStatusBar(this->ui->sb_SwiftData);

    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    auto logHandler = CLogHandler::instance()->handlerForPattern(
                          CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
                      );
    logHandler->subscribe(this, &CSwiftData::ps_appendLogMessage);
}

void CSwiftData::initReaders()
{
    Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing reader");
    this->ui->comp_MainInfoArea->setProvider(this->m_webDataReader);
    this->ui->comp_InfoBar->setProvider(this->m_webDataReader);
    this->m_webDataReader->readAllInBackground(1000); // kick of readers a little delayed
}

void CSwiftData::initMenu()
{
    // menu
    this->initDynamicMenus();
    this->ui->menu_WindowMinimize->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarMinButton));
    connect(this->ui->menu_FileExit, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_FileSettingsDirectory, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_FileCacheDirectory, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_FileResetSettings, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_FileReloadStyleSheets, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);

    connect(this->ui->menu_WindowFont, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_WindowMinimize, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_WindowToggleOnTop, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);

    connect(this->ui->menu_MappingMaxData, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_MappingMaxMapping, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);

    connect(this->ui->menu_InternalsCompileInfo, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_InternalsEnvVars, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_InternalsMetatypes, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_InternalsSetup, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_InternalsDeleteCachedFiles, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_InternalsDisplayCachedFiles, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_InternalsJsonBootstrapTemplate, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
}

void CSwiftData::performGracefulShutdown()
{
    if (this->m_webDataReader)
    {
        m_webDataReader->gracefulShutdown();
        m_webDataReader = nullptr;
    }
}

void CSwiftData::displayConsole()
{
    this->ui->comp_MainInfoArea->displayConsole();
}

void CSwiftData::displayLog()
{
    this->ui->comp_MainInfoArea->displayLog();
}
