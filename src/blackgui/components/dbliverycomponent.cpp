/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbliverycomponent.h"
#include "blackgui/filters/liveryfilterbar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/liveryview.h"
#include "dbliverycomponent.h"
#include "ui_dbliverycomponent.h"

#include <QtGlobal>

using namespace BlackCore;
using namespace BlackGui::Views;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbLiveryComponent::CDbLiveryComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbLiveryComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(ui->tvp_Liveries);
            connect(ui->tvp_Liveries, &CLiveryView::requestNewBackendData, this, &CDbLiveryComponent::onReload);

            // filter and drag and drop
            ui->tvp_Liveries->setFilterWidget(ui->filter_Livery);
            ui->tvp_Liveries->allowDragDrop(true, false);
            ui->tvp_Liveries->menuAddItems(CViewBaseNonTemplate::MenuCopy);

            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLiveryComponent::onLiveriesRead);
            this->onLiveriesRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getLiveriesCount());
        }

        CDbLiveryComponent::~CDbLiveryComponent()
        { }

        CLiveryView *CDbLiveryComponent::view()
        {
            return ui->tvp_Liveries;
        }

        void CDbLiveryComponent::filter(const BlackMisc::Aviation::CLivery &livery)
        {
            ui->filter_Livery->filter(livery);
        }

        void CDbLiveryComponent::filterByAirline(const BlackMisc::Aviation::CAirlineIcaoCode &icao)
        {
            ui->filter_Livery->filter(icao);
        }

        void CDbLiveryComponent::onLiveriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
            if (entity.testFlag(CEntityFlags::LiveryEntity) && CEntityFlags::isFinishedReadState(readState))
            {
                ui->tvp_Liveries->updateContainerMaybeAsync(sGui->getWebDataServices()->getLiveries());
            }
        }

        void CDbLiveryComponent::onReload()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
            sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::LiveryEntity);
        }
    } // ns
} // ns
