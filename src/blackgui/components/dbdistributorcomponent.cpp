/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_dbdistributorcomponent.h"
#include "blackgui/guiapplication.h"
#include "dbdistributorcomponent.h"
#include <QFrame>
#include <QScopedPointer>

using namespace BlackCore;
using namespace BlackMisc::Network;
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
            this->setViewWithIndicator(this->ui->tvp_Distributors);
            this->ui->tvp_Distributors->setResizeMode(CViewBaseNonTemplate::ResizingAuto);
            this->ui->tvp_Distributors->allowDragDropValueObjects(true, false);
            connect(this->ui->tvp_Distributors, &CDistributorView::requestNewBackendData, this, &CDbDistributorComponent::ps_reload);
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbDistributorComponent::ps_distributorsRead);
            this->ps_distributorsRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getDistributorsCount());
        }

        CDbDistributorComponent::~CDbDistributorComponent()
        { }

        void CDbDistributorComponent::ps_distributorsRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::DistributorEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_Distributors->updateContainer(sGui->getWebDataServices()->getDistributors());
            }
        }

        void CDbDistributorComponent::ps_reload()
        {
            if (!sGui) { return; }
            sGui->getWebDataServices()->triggerRead(CEntityFlags::DistributorEntity);
        }
    } // ns
} // ns
