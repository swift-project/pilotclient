/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftdata.h"
#include "blackcore/application.h"
#include "blackcore/data/globalsetup.h"
#include "blackgui/components/datamaininfoareacomponent.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/logpattern.h"
#include "blackmisc/network/url.h"
#include "ui_swiftdata.h"

#include <QAction>
#include <QString>
#include <QStyle>
#include <QtGlobal>

class QWidget;

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackGui;
using namespace BlackGui::Components;

CSwiftData::CSwiftData(QWidget *parent) :
    QMainWindow(parent),
    CIdentifiable(this),
    ui(new Ui::CSwiftData)
{
    ui->setupUi(this);
    this->init();
}

void CSwiftData::initStyleSheet()
{
    const QString s = sGui->getStyleSheetUtility().styles(
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
    if (sGui)
    {
        // save settings
        if (sGui->showCloseDialog(this, event) == QDialog::Rejected) { return; }
    }
    this->performGracefulShutdown();
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
    sGui->initMainApplicationWindow(this);
    this->initLogDisplay();

    this->m_mwaLogComponent = this->ui->comp_MainInfoArea->getLogComponent();
    this->m_mwaStatusBar = &this->m_statusBar;
    this->m_mwaOverlayFrame = this->ui->comp_MainInfoArea->getMappingComponent();

    this->initStyleSheet();
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CSwiftData::ps_onStyleSheetsChanged);
    this->initMenu();

    // update title
    const CGlobalSetup s(sApp->getGlobalSetup());
    if (!s.getDbHomePageUrl().isEmpty())
    {
        QString t(this->windowTitle());
        t += " ";
        t += s.getDbHomePageUrl().toQString(true);
        this->setWindowTitle(t);
    }
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

void CSwiftData::initMenu()
{
    // menu

    this->initDynamicMenus();
    this->ui->menu_WindowMinimize->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarMinButton));

    connect(this->ui->menu_WindowFont, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_MappingMaxData, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);
    connect(this->ui->menu_MappingMaxMapping, &QAction::triggered, this, &CSwiftData::ps_onMenuClicked);

    sGui->addMenuFile(*this->ui->menu_File);
    sGui->addMenuInternals(*this->ui->menu_Internals);
    sGui->addMenuWindow(*this->ui->menu_Window);
    sGui->addMenuHelp(*this->ui->menu_Help);
}

void CSwiftData::performGracefulShutdown()
{
    // void
}

void CSwiftData::displayConsole()
{
    this->ui->comp_MainInfoArea->displayConsole();
}

void CSwiftData::displayLog()
{
    this->ui->comp_MainInfoArea->displayLog();
}
