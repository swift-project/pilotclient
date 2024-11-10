// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbownmodelscomponent.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/guiapplication.h"
#include "core/webdataservices.h"
#include "core/db/databaseutils.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"
#include "dbmodelworkbenchcomponent.h"
#include "ui_dbmodelworkbenchcomponent.h"

#include <QAction>
#include <QIcon>
#include <QDir>
#include <QtGlobal>
#include <QPointer>
#include <QFileDialog>

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::core::db;
using namespace BlackGui::Menus;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui::Components
{
    CDbModelWorkbenchComponent::CDbModelWorkbenchComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                              ui(new Ui::CDbModelWorkbenchComponent)
    {
        ui->setupUi(this);

        ui->tvp_Models->menuAddItems(CAircraftModelView::MenuStashing);
        ui->tvp_Models->setAircraftModelMode(CAircraftModelListModel::OwnAircraftModelMappingTool);
        ui->tvp_Models->addFilterDialog();
        ui->tvp_Models->setDisplayAutomatically(true);
        ui->tvp_Models->allowDragDrop(false, true, true);
        ui->tvp_Models->setAcceptedMetaTypeIds();

        // menu
        ui->tvp_Models->setCustomMenu(new CConsolidateWithDbDataMenu(ui->tvp_Models, this));
    }

    CDbModelWorkbenchComponent::~CDbModelWorkbenchComponent()
    {
        // void
    }

    const QStringList &CDbModelWorkbenchComponent::getLogCategories()
    {
        static const QStringList l({ CLogCategories::modelGui(), CLogCategories::guiComponent() });
        return l;
    }

    CAircraftModelView *CDbModelWorkbenchComponent::view() const
    {
        return ui->tvp_Models;
    }

    CAircraftModelListModel *CDbModelWorkbenchComponent::model() const
    {
        return ui->tvp_Models->derivedModel();
    }

    CAircraftModel CDbModelWorkbenchComponent::getOwnModelForModelString(const QString &modelString) const
    {
        return this->getModels().findFirstByModelStringOrDefault(modelString);
    }

    CAircraftModelList CDbModelWorkbenchComponent::getSelectedModels() const
    {
        return ui->tvp_Models->selectedObjects();
    }

    CAircraftModelList CDbModelWorkbenchComponent::getModels() const
    {
        return ui->tvp_Models->container();
    }

    int CDbModelWorkbenchComponent::getModelsCount() const
    {
        return ui->tvp_Models->rowCount();
    }

    void CDbModelWorkbenchComponent::setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator)
    {
        Q_UNUSED(simulator);
        ui->tvp_Models->replaceOrAddModelsWithString(models);
    }

    int CDbModelWorkbenchComponent::updateModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator)
    {
        Q_UNUSED(simulator);
        return ui->tvp_Models->replaceOrAddModelsWithString(models);
    }
} // ns
