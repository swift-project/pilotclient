/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_dbliverycomponent.h"
#include "dbliverycomponent.h"
#include "blackcore/webdataservices.h"
#include <functional>

using namespace BlackCore;
using namespace BlackGui::Views;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbLiveryComponent::CDbLiveryComponent(QWidget *parent) :
            QFrame(parent),
            CWebDataServicesAware(nullptr),
            ui(new Ui::CDbLiveryComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(this->ui->tvp_Liveries);
            connect(this->ui->tvp_Liveries, &CLiveryView::requestNewBackendData, this, &CDbLiveryComponent::ps_reload);

            // filter and drag and drop
            this->ui->tvp_Liveries->setFilterWidget(this->ui->filter_Livery);
            this->ui->tvp_Liveries->allowDragDropValueObjects(true, false);
        }

        CDbLiveryComponent::~CDbLiveryComponent()
        { }

        void CDbLiveryComponent::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            connectDataReadSignal(
                this,
                std::bind(&CDbLiveryComponent::ps_liveriesRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = getLiveriesCount();
            if (c > 0)
            {
                ps_liveriesRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFinished, c);
            }
        }

        void CDbLiveryComponent::filter(const BlackMisc::Aviation::CLivery &livery)
        {
            this->ui->filter_Livery->filter(livery);
        }

        void CDbLiveryComponent::ps_liveriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::LiveryEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_Liveries->updateContainerMaybeAsync(this->getLiveries());
            }
        }

        void CDbLiveryComponent::ps_reload()
        {
            if (!hasProvider()) { return; }
            triggerRead(CEntityFlags::LiveryEntity);
        }

    } // ns
} // ns

