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
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/datamaininfoareacomponent.h"
#include "blackgui/components/datamappingcomponent.h"
#include "blackgui/components/enableforruntime.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/web_datareader.h"
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
    QMainWindow(parent), CIdentifiable(this), ui(new Ui::CSwiftData),
    m_webDataReader(new CWebDataReader(CWebDataReader::AllSwiftDbReaders))
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

void CSwiftData::ps_appendLogMessage(const CStatusMessage &message)
{
    ui->comp_MainInfoArea->getLogComponent()->appendStatusMessageToList(message);
}

void CSwiftData::ps_onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void CSwiftData::init()
{
    this->setWindowIcon(CIcons::swiftDatabase24());
    this->setWindowTitle(QString("swiftdata %1").arg(CProject::version()));
    this->setObjectName("CSwiftData");
    this->initStyleSheet();
    connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CSwiftData::ps_onStyleSheetsChanged);
    this->initReaders();
}

void CSwiftData::initLogDisplay()
{
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    auto logHandler = CLogHandler::instance()->handlerForPattern(
                          CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
                      );
    logHandler->subscribe(this, &CSwiftData::ps_appendLogMessage);
}


void CSwiftData::initReaders()
{
    CDataMappingComponent *mc = this->ui->comp_MainInfoArea->getMappingComponent();
    Q_ASSERT_X(mc, Q_FUNC_INFO, "Missing mapping component");
    Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing reader");
    mc->readersInitialized(this->m_webDataReader);
    this->m_webDataReader->readAllInBackground(1000); // kick of readers
}
