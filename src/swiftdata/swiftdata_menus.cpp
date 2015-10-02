/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftdata.h"
#include "ui_swiftdata.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/datainfoareacomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/roles.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include <QPoint>
#include <QMenu>
#include <QDesktopServices>
#include <QProcess>
#include <QFontDialog>

using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackMisc;

/*
 * Menu clicked
 */
void CSwiftData::ps_onMenuClicked()
{
    QObject *sender = QObject::sender();
    if (sender == this->ui->menu_FileReloadStyleSheets)
    {
        CStyleSheetUtility::instance().read();
    }
    else if (sender == this->ui->menu_WindowFont)
    {
        // this->ps_setMainPageToInfoArea();
        // this->ui->comp_MainInfoArea->selectSettingsTab(BlackGui::Components::CSettingsComponent::SettingTabGui);
    }
    else if (sender == this->ui->menu_WindowMinimize)
    {
        this->showMinimized();
    }
    else if (sender == this->ui->menu_FileExit)
    {
        CLogMessage(this).info("Closing");
        this->close(); // graceful shutdown in close event
    }
    else if (sender == this->ui->menu_FileSettingsDirectory)
    {
        QString path;
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
    else if (sender == this->ui->menu_FileResetSettings)
    {
        // !\todo reset settings
    }
    else if (sender == this->ui->menu_DebugMetaTypes)
    {
        QString metadata;
        QTextStream stream(&metadata);
        BlackMisc::displayAllUserMetatypesTypes(stream);
        this->ui->comp_MainInfoArea->getLogComponent()->appendPlainTextToConsole(metadata);
    }
    else if (sender == this->ui->menu_MappingMaxData)
    {
        CDbMappingComponent *mappingComponent = this->ui->comp_MainInfoArea->getMappingComponent();
        mappingComponent->resizeForSelect();
    }
    else if (sender == this->ui->menu_MappingMaxMapping)
    {
        CDbMappingComponent *mappingComponent = this->ui->comp_MainInfoArea->getMappingComponent();
        mappingComponent->resizeForMapping();
    }
    else
    {
        QAction *a = qobject_cast<QAction *>(sender);
        if (a)
        {
            if (a->data() == "admin")
            {
                CRoles::roles().setAdmin(true);
            }
        }
    }
}

void CSwiftData::initDynamicMenus()
{
    Q_ASSERT(this->ui->menu_InfoAreas);
    Q_ASSERT(this->ui->comp_MainInfoArea);
    this->ui->menu_InfoAreas->addActions(this->ui->comp_MainInfoArea->getInfoAreaSelectActions(this->ui->menu_InfoAreas));

    QString resourceDir(CProject::getSwiftResourceDir());
    if (!resourceDir.isEmpty())
    {
        Q_ASSERT_X(this->ui->comp_MainInfoArea, Q_FUNC_INFO, "Missing main info area");
        Q_ASSERT_X(this->ui->comp_MainInfoArea->getDataInfoAreaComponent(), Q_FUNC_INFO, "Missing DB info area");
        this->ui->menu_Mapping->addAction(CIcons::load16(), "Load DB data", this->ui->comp_MainInfoArea->getDataInfoAreaComponent(), SLOT(readDbDataFromResourceDir()));
        if (CProject::isRunningInDeveloperEnvironment() && this->ui->comp_MainInfoArea->getDataInfoAreaComponent()->canConnectSwiftDb())
        {
            this->ui->menu_Mapping->addAction(CIcons::save16(), "Save DB data", this->ui->comp_MainInfoArea->getDataInfoAreaComponent(), SLOT(writeDbDataToResourceDir()));
        }
    }

    if (CProject::isRunningInDeveloperEnvironment() && !CRoles::roles().isAdmin())
    {
        QAction *a = this->ui->menu_Internals->addAction(CIcons::user16(), "Set administrator");
        a->setData("admin");
    }
}
