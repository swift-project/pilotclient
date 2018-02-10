/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbairlineicaocomponent.h"
#include "blackgui/filters/airlineicaofilterbar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/airlineicaoview.h"
#include "blackgui/views/viewbase.h"
#include "ui_dbairlineicaocomponent.h"

#include <QDateTime>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackGui;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbAirlineIcaoComponent::CDbAirlineIcaoComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbAirlineIcaoComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");

            ui->setupUi(this);
            this->setViewWithIndicator(ui->tvp_AirlineIcao);
            ui->tvp_AirlineIcao->setResizeMode(CAirlineIcaoCodeView::ResizingOff);
            ui->tvp_AirlineIcao->setResizeMode(CAirlineIcaoCodeView::ResizingOff);
            ui->tvp_AirlineIcao->allowDragDrop(true, false);
            ui->tvp_AirlineIcao->setFilterWidget(ui->filter_AirlineIcao);
            ui->tvp_AirlineIcao->menuAddItems(CViewBaseNonTemplate::MenuCopy);

            connect(ui->tvp_AirlineIcao, &CAirlineIcaoCodeView::requestNewBackendData, this, &CDbAirlineIcaoComponent::ps_reload);
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAirlineIcaoComponent::ps_icaoRead);
            this->ps_icaoRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getAirlineIcaoCodesCount());
        }

        CDbAirlineIcaoComponent::~CDbAirlineIcaoComponent()
        { }

        BlackGui::Views::CAirlineIcaoCodeView *CDbAirlineIcaoComponent::view() const
        {
            return ui->tvp_AirlineIcao;
        }

        void CDbAirlineIcaoComponent::ps_icaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::AirlineIcaoEntity) && readState == CEntityFlags::ReadFinished)
            {
                ui->tvp_AirlineIcao->updateContainerMaybeAsync(sGui->getWebDataServices()->getAirlineIcaoCodes());
            }
        }

        void CDbAirlineIcaoComponent::ps_reload()
        {
            if (!sGui || !sGui->hasWebDataServices()) { return; }
            sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::AirlineIcaoEntity, QDateTime());
        }
    } // ns
} // ns
