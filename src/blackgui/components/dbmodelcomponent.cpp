/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbmodelcomponent.h"
#include "dbmappingcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/shortcut.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "ui_dbmodelcomponent.h"
#include <functional>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {
        CDbModelComponent::CDbModelComponent(QWidget *parent) :
            QFrame(parent),
            CDbMappingComponentAware(parent),
            ui(new Ui::CDbModelComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(this->ui->tvp_AircraftModel);
            this->ui->tvp_AircraftModel->setAircraftModelMode(CAircraftModelListModel::Database);
            this->ui->tvp_AircraftModel->menuAddItems(CAircraftModelView::MenuStashing);
            this->ui->tvp_AircraftModel->menuRemoveItems(CAircraftModelView::MenuHighlightStashed); // not supported here
            connect(this->ui->tvp_AircraftModel, &CAircraftModelView::requestNewBackendData, this, &CDbModelComponent::ps_reload);
            connect(this->ui->tvp_AircraftModel, &CAircraftModelView::requestStash, this, &CDbModelComponent::requestStash);
            connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CDbModelComponent::ps_onStyleSheetChanged);

            // configure view
            this->ui->tvp_AircraftModel->setFilterWidget(this->ui->filter_AircraftModelFilter);
            this->ui->tvp_AircraftModel->allowDragDropValueObjects(true, false);

            connect(sApp->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbModelComponent::ps_modelsRead);
            this->ps_modelsRead(CEntityFlags::ModelEntity, CEntityFlags::ReadFinished, sApp->getWebDataServices()->getModelsCount());
        }

        CDbModelComponent::~CDbModelComponent()
        {
            // void
        }

        bool CDbModelComponent::hasModels() const
        {
            return !this->ui->tvp_AircraftModel->isEmpty();
        }

        void CDbModelComponent::requestUpdatedData()
        {
            QDateTime ts;
            if (!this->ui->tvp_AircraftModel->isEmpty())
            {
                CAircraftModel model(this->ui->tvp_AircraftModel->container().latestObject());
                ts = model.getUtcTimestamp();
            }
            sGui->getWebDataServices()->triggerRead(CEntityFlags::ModelEntity, ts);
        }

        void CDbModelComponent::ps_modelsRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::ModelEntity))
            {
                if (readState == CEntityFlags::ReadFinished || readState == CEntityFlags::ReadFinishedRestricted)
                {
                    this->ui->tvp_AircraftModel->updateContainerMaybeAsync(sGui->getWebDataServices()->getModels());
                }
            }
        }

        void CDbModelComponent::ps_reload()
        {
            if (!sGui) { return; }
            sGui->getWebDataServices()->triggerRead(CEntityFlags::ModelEntity);
        }

        void CDbModelComponent::ps_onStyleSheetChanged()
        {
            // code goes here
        }

        void CDbModelComponent::ps_stashSelectedModels()
        {
            if (!this->ui->tvp_AircraftModel->hasSelection()) { return; }
            const CAircraftModelList models(this->ui->tvp_AircraftModel->selectedObjects());
            if (!models.isEmpty())
            {
                emit requestStash(models);
            }
        }
    } // ns
} // ns
