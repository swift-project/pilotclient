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
#include <functional>

using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
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
            this->ui->tvp_AircraftIcao->setResizeMode(CViewBaseNonTemplate::ResizingOff);
            connect(this->ui->tvp_AircraftIcao, &CAircraftIcaoCodeView::requestNewBackendData, this, &CDbAircraftIcaoComponent::ps_reload);

            this->ui->tvp_AircraftIcao->allowDragDropValueObjects(true, false);
            this->ui->tvp_AircraftIcao->setFilterWidget(this->ui->filter_AircraftIcao);
        }

        CDbAircraftIcaoComponent::~CDbAircraftIcaoComponent()
        { }

        void CDbAircraftIcaoComponent::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            this->ui->filter_AircraftIcao->setProvider(webDataReaderProvider);
            connectSwiftDatabaseSignals(
                this,
                std::bind(&CDbAircraftIcaoComponent::ps_icaoRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = getAircraftIcaoCodesCount();
            if (c > 0)
            {
                ps_icaoRead(CDbFlags::AircraftIcaoEntity, CDbFlags::ReadFinished, c);
            }
        }

        void CDbAircraftIcaoComponent::filter(const CAircraftIcaoCode &icao)
        {
            this->ui->filter_AircraftIcao->filter(icao);
        }

        void CDbAircraftIcaoComponent::ps_icaoRead(CDbFlags::Entity entity, CDbFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CDbFlags::AircraftIcaoEntity) && readState == CDbFlags::ReadFinished)
            {
                this->ui->tvp_AircraftIcao->updateContainerMaybeAsync(this->getAircraftIcaoCodes());
            }
        }

        void CDbAircraftIcaoComponent::ps_reload()
        {
            if (!hasProvider()) { return; }
            triggerRead(CDbFlags::AircraftIcaoEntity);
        }

    } // ns
} // ns
