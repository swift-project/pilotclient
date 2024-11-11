// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/datainfoareacomponent.h"
#include "blackgui/components/datamaininfoareacomponent.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/guiapplication.h"
#include "misc/icons.h"
#include "misc/swiftdirectories.h"
#include "misc/directoryutils.h"
#include "swiftdata.h"
#include "ui_swiftdata.h"

#include <QAction>
#include <QDir>
#include <QMenu>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QtGlobal>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackGui::Components;
using namespace swift::misc;

void CSwiftData::onMenuClicked()
{
    QObject *sender = QObject::sender();
    if (sender == ui->menu_WindowFont)
    {
        // this->ps_setMainPageToInfoArea();
        // ui->comp_MainInfoArea->selectSettingsTab(BlackGui::Components::CSettingsComponent::SettingTabGui);
    }
    else if (sender == ui->menu_MappingMaxData)
    {
        CDbMappingComponent *mappingComponent = ui->comp_MainInfoArea->getMappingComponent();
        mappingComponent->resizeForSelect();
    }
    else if (sender == ui->menu_MappingMaxMapping)
    {
        CDbMappingComponent *mappingComponent = ui->comp_MainInfoArea->getMappingComponent();
        mappingComponent->resizeForMapping();
    }
}

void CSwiftData::initDynamicMenus()
{
    Q_ASSERT_X(ui->menu_InfoAreas, Q_FUNC_INFO, "missing info areas");
    Q_ASSERT_X(ui->comp_MainInfoArea, Q_FUNC_INFO, "missing main area");
    ui->menu_InfoAreas->addActions(ui->comp_MainInfoArea->getInfoAreaSelectActions(true, ui->menu_InfoAreas));

    //  menu entry for auto publish data
    ui->menu_Mapping->addAction(CIcons::database16(), "Auto publish data", this, &CSwiftData::showAutoPublishing);

    QString resourceDir(CSwiftDirectories::shareDirectory());
    if (!resourceDir.isEmpty() && QDir(resourceDir).exists())
    {
        Q_ASSERT_X(ui->comp_MainInfoArea, Q_FUNC_INFO, "Missing main info area");
        Q_ASSERT_X(ui->comp_MainInfoArea->getDataInfoAreaComponent(), Q_FUNC_INFO, "Missing DB info area");
        ui->menu_Mapping->addAction(CIcons::database16(), "Load all DB data", ui->comp_MainInfoArea->getDataInfoAreaComponent(), &CDataInfoAreaComponent::requestUpdateOfAllDbData);
        ui->menu_Mapping->addAction(CIcons::load16(), "Load DB test data from disk", ui->comp_MainInfoArea->getDataInfoAreaComponent(), &CDataInfoAreaComponent::readDbDataFromResourceDir);
        if (sGui->isDeveloperFlagSet())
        {
            ui->menu_Mapping->addAction(CIcons::save16(), "Save DB test data to disk", ui->comp_MainInfoArea->getDataInfoAreaComponent(), &CDataInfoAreaComponent::writeDbDataToResourceDir);
        }
    }
}
