/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_dbdistributorcomponent.h"
#include "dbdistributorcomponent.h"
#include <QFrame>
#include <QScopedPointer>

using namespace BlackMisc::Network;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbDistributorComponent::CDbDistributorComponent(QWidget *parent) :
            QFrame(parent),
            CWebDataServicesAware(nullptr),
            ui(new Ui::CDbDistributorComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_Distributors->setResizeMode(CViewBaseNonTemplate::ResizingAuto);
            this->ui->tvp_Distributors->allowDragDropValueObjects(true, false);
            connect(this->ui->tvp_Distributors, &CDistributorView::requestNewBackendData, this, &CDbDistributorComponent::ps_reload);
        }

        CDbDistributorComponent::~CDbDistributorComponent()
        { }

        void CDbDistributorComponent::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            connectDataReadSignal(
                this,
                std::bind(&CDbDistributorComponent::ps_distributorsRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = getDistributorsCount();
            if (c > 0)
            {
                ps_distributorsRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, c);
            }
        }

        void CDbDistributorComponent::ps_distributorsRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::DistributorEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_Distributors->updateContainer(this->getDistributors());
            }
        }

        void CDbDistributorComponent::ps_reload()
        {
            if (!hasProvider()) { return; }
            triggerRead(CEntityFlags::DistributorEntity);
        }
    } // ns
} // ns
