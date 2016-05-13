/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_dbaircrafticaocomponent.h"
#include "dbaircrafticaocomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include <functional>

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
            this->setViewWithIndicator(this->ui->tvp_AircraftIcao);
            this->ui->tvp_AircraftIcao->setResizeMode(CAircraftIcaoCodeView::ResizingOff);
            connect(this->ui->tvp_AircraftIcao, &CAircraftIcaoCodeView::requestNewBackendData, this, &CDbAircraftIcaoComponent::ps_reload);

            this->ui->tvp_AircraftIcao->allowDragDrop(true, false);
            this->ui->tvp_AircraftIcao->setFilterWidget(this->ui->filter_AircraftIcao);

            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAircraftIcaoComponent::ps_icaoRead);
            this->ps_icaoRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getAircraftIcaoCodesCount());
        }

        CDbAircraftIcaoComponent::~CDbAircraftIcaoComponent()
        { }

        void CDbAircraftIcaoComponent::filter(const CAircraftIcaoCode &icao)
        {
            this->ui->filter_AircraftIcao->filter(icao);
        }

        void CDbAircraftIcaoComponent::ps_icaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::AircraftIcaoEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_AircraftIcao->updateContainerMaybeAsync(sGui->getWebDataServices()->getAircraftIcaoCodes());
            }
        }

        void CDbAircraftIcaoComponent::ps_reload()
        {
            if (!sGui) { return; }
            sGui->getWebDataServices()->triggerRead(CEntityFlags::AircraftIcaoEntity, QDateTime());
        }

    } // ns
} // ns
