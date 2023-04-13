/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/dbownmodelscomponent.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackcore/db/databaseutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "dbmodelworkbenchcomponent.h"
#include "ui_dbmodelworkbenchcomponent.h"

#include <QAction>
#include <QIcon>
#include <QDir>
#include <QtGlobal>
#include <QPointer>
#include <QFileDialog>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Db;
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
