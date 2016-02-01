/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbairlineicaocomponent.h"
#include "ui_dbairlineicaocomponent.h"
#include "blackcore/webdataservices.h"
#include <functional>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbAirlineIcaoComponent::CDbAirlineIcaoComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbAirlineIcaoComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(this->ui->tvp_AirlineIcao);
            this->ui->tvp_AirlineIcao->setResizeMode(CAirlineIcaoCodeView::ResizingOff);
            this->ui->tvp_AirlineIcao->setResizeMode(CAirlineIcaoCodeView::ResizingOff);
            this->ui->tvp_AirlineIcao->allowDragDropValueObjects(true, false);
            this->ui->tvp_AirlineIcao->setFilterWidget(this->ui->filter_AirlineIcao);
            connect(this->ui->tvp_AirlineIcao, &CAirlineIcaoCodeView::requestNewBackendData, this, &CDbAirlineIcaoComponent::ps_reload);
        }

        CDbAirlineIcaoComponent::~CDbAirlineIcaoComponent()
        { }

        void CDbAirlineIcaoComponent::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            this->ui->filter_AirlineIcao->setProvider(webDataReaderProvider);
            webDataReaderProvider->connectDataReadSignal(
                this,
                std::bind(&CDbAirlineIcaoComponent::ps_icaoRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = this->getAirlineIcaoCodesCount();
            if (c > 0)
            {
                this->ps_icaoRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, c);
            }
        }

        void CDbAirlineIcaoComponent::ps_icaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::AirlineIcaoEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_AirlineIcao->updateContainerMaybeAsync(this->getAirlineIcaoCodes());
            }
        }

        void CDbAirlineIcaoComponent::ps_reload()
        {
            if (!hasProvider()) { return; }
            triggerRead(CEntityFlags::AirlineIcaoEntity, QDateTime());
        }

    } // ns
} // ns
