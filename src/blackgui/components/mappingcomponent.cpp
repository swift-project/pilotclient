/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context_simulator.h"
#include "../views/aircraftmodelview.h"
#include "../models/aircraftmodellistmodel.h"
#include "../guiutility.h"
#include "mappingcomponent.h"
#include "ui_mappingcomponent.h"

using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {

        CMappingComponent::CMappingComponent(QWidget *parent) :
            QTabWidget(parent), ui(new Ui::CMappingComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_CurrentMappings->setAircraftModelMode(CAircraftModelListModel::MappedModel);
            this->ui->tvp_AircraftModels->setAircraftModelMode(CAircraftModelListModel::ModelOnly);
            this->ui->tvp_CurrentMappings->setResizeMode(CAircraftModelView::ResizingOnce);
            this->ui->tvp_AircraftModels->setResizeMode(CAircraftModelView::ResizingOff);

            connect(this->ui->tvp_AircraftModels, &CAircraftModelView::requestUpdate, this, &CMappingComponent::ps_aircraftModelsLoaded);
            connect(this->ui->tvp_CurrentMappings, &CAircraftModelView::requestUpdate, this, &CMappingComponent::ps_onMappingsChanged);
            connect(this->ui->tvp_AircraftModels, &CAircraftModelView::countChanged, this, &CMappingComponent::ps_countChanged);
            connect(this->ui->tvp_CurrentMappings, &CAircraftModelView::countChanged, this, &CMappingComponent::ps_countChanged);
        }

        CMappingComponent::~CMappingComponent()
        { }

        int CMappingComponent::countCurrentMappings() const
        {
            Q_ASSERT(this->ui->tvp_CurrentMappings);
            return this->ui->tvp_CurrentMappings->rowCount();
        }

        int CMappingComponent::countAircraftModels() const
        {
            Q_ASSERT(this->ui->tvp_AircraftModels);
            return this->ui->tvp_AircraftModels->rowCount();
        }

        void CMappingComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(getIContextSimulator());
            connect(getIContextSimulator(), &IContextSimulator::installedAircraftModelsChanged, this, &CMappingComponent::ps_aircraftModelsLoaded);
            connect(getIContextSimulator(), &IContextSimulator::modelMatchingCompleted, this, &CMappingComponent::ps_modelMatched);
        }

        void CMappingComponent::ps_aircraftModelsLoaded()
        {
            Q_ASSERT(getIContextSimulator());
            this->ui->tvp_AircraftModels->updateContainer(getIContextSimulator()->getInstalledModels());
        }

        void CMappingComponent::ps_modelMatched(const BlackMisc::Network::CAircraftModel &model)
        {
            Q_UNUSED(model);
            ps_onMappingsChanged();
        }

        void CMappingComponent::ps_onMappingsChanged()
        {
            Q_ASSERT(getIContextSimulator());
            this->ui->tvp_CurrentMappings->updateContainer(getIContextSimulator()->getCurrentlyMatchedModels());
        }

        void CMappingComponent::ps_countChanged(int count)
        {
            Q_UNUSED(count);
            int am = this->indexOf(this->ui->tb_AircraftModels);
            int cm = this->indexOf(this->ui->tb_CurrentMappings);
            QString a = this->tabBar()->tabText(am);
            QString c = this->tabBar()->tabText(cm);
            a = CGuiUtility::replaceTabCountValue(a, this->countAircraftModels());
            c = CGuiUtility::replaceTabCountValue(c, this->countCurrentMappings());
            this->tabBar()->setTabText(am, a);
            this->tabBar()->setTabText(cm, c);
        }

    } // namespace
} // namespace

