/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbdistributorcomponent.h"
#include "ui_dbdistributorcomponent.h"
#include "blackgui/components/dbdistributorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/views/viewbase.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QFrame>
#include <QScopedPointer>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbDistributorComponent::CDbDistributorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbDistributorComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(ui->tvp_Distributors);
            ui->tvp_Distributors->setResizeMode(CViewBaseNonTemplate::ResizingAuto);
            ui->tvp_Distributors->allowDragDrop(true, false);
            ui->tvp_Distributors->setFilterWidget(ui->filter_Distributor);

            connect(ui->tvp_Distributors, &CDistributorView::requestNewBackendData, this, &CDbDistributorComponent::reload);
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbDistributorComponent::distributorsRead);
            this->distributorsRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getDistributorsCount());
        }

        CDbDistributorComponent::~CDbDistributorComponent()
        { }

        CDistributorView *CDbDistributorComponent::view() const
        {
            return ui->tvp_Distributors;
        }

        void CDbDistributorComponent::filterBySimulator(const CSimulatorInfo &simulator)
        {
            ui->filter_Distributor->setSimulator(simulator);
        }

        void CDbDistributorComponent::distributorsRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::DistributorEntity) && readState == CEntityFlags::ReadFinished)
            {
                ui->tvp_Distributors->updateContainer(sGui->getWebDataServices()->getDistributors());
            }
        }

        void CDbDistributorComponent::reload()
        {
            if (!sGui) { return; }
            sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::DistributorEntity);
        }
    } // ns
} // ns
