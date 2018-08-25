/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbaircrafticaocomponent.h"
#include "blackgui/filters/aircrafticaofilterbar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/aircrafticaoview.h"
#include "blackgui/views/viewbase.h"
#include "dbaircrafticaocomponent.h"
#include "ui_dbaircrafticaocomponent.h"

#include <QDateTime>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackGui;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbAircraftIcaoComponent::CDbAircraftIcaoComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbAircraftIcaoComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(ui->tvp_AircraftIcao);
            ui->tvp_AircraftIcao->setResizeMode(CAircraftIcaoCodeView::ResizingOff);
            connect(ui->tvp_AircraftIcao, &CAircraftIcaoCodeView::requestNewBackendData, this, &CDbAircraftIcaoComponent::onReload);

            ui->tvp_AircraftIcao->allowDragDrop(true, false);
            ui->tvp_AircraftIcao->setFilterWidget(ui->filter_AircraftIcao);
            ui->tvp_AircraftIcao->menuAddItems(CViewBaseNonTemplate::MenuCopy);

            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAircraftIcaoComponent::onIcaoRead, Qt::QueuedConnection);
            this->onIcaoRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getAircraftIcaoCodesCount());
        }

        CDbAircraftIcaoComponent::~CDbAircraftIcaoComponent()
        { }

        BlackGui::Views::CAircraftIcaoCodeView *CDbAircraftIcaoComponent::view() const
        {
            return ui->tvp_AircraftIcao;
        }

        void CDbAircraftIcaoComponent::filter(const CAircraftIcaoCode &icao)
        {
            ui->filter_AircraftIcao->filter(icao);
        }

        void CDbAircraftIcaoComponent::onIcaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
            if (entity.testFlag(CEntityFlags::AircraftIcaoEntity) && CEntityFlags::isFinishedReadState(readState))
            {
                ui->tvp_AircraftIcao->updateContainerMaybeAsync(sGui->getWebDataServices()->getAircraftIcaoCodes());
            }
        }

        void CDbAircraftIcaoComponent::onReload()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::AircraftIcaoEntity, QDateTime());
        }
    } // ns
} // ns
