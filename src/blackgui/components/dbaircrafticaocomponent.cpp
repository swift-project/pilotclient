/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/dbaircrafticaocomponent.h"
#include "blackgui/filters/aircrafticaofilterbar.h"
#include "blackgui/views/aircrafticaoview.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "dbaircrafticaocomponent.h"
#include "ui_dbaircrafticaocomponent.h"

#include <QDateTime>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CDbAircraftIcaoComponent::CDbAircraftIcaoComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                          ui(new Ui::CDbAircraftIcaoComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_AircraftIcao);
        ui->tvp_AircraftIcao->setResizeMode(CAircraftIcaoCodeView::ResizingOff);

        ui->tvp_AircraftIcao->allowDragDrop(true, false);
        ui->tvp_AircraftIcao->setFilterWidget(ui->filter_AircraftIcao);
        ui->tvp_AircraftIcao->menuAddItems(CViewBaseNonTemplate::MenuCopy);

        connect(ui->tvp_AircraftIcao, &CAircraftIcaoCodeView::requestNewBackendData, this, &CDbAircraftIcaoComponent::onReload);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAircraftIcaoComponent::onIcaoRead, Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this, &CDbAircraftIcaoComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onIcaoRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getAircraftIcaoCodesCount());
    }

    CDbAircraftIcaoComponent::~CDbAircraftIcaoComponent()
    {}

    BlackGui::Views::CAircraftIcaoCodeView *CDbAircraftIcaoComponent::view() const
    {
        return ui->tvp_AircraftIcao;
    }

    void CDbAircraftIcaoComponent::filter(const CAircraftIcaoCode &icao)
    {
        ui->filter_AircraftIcao->filter(icao);
    }

    bool CDbAircraftIcaoComponent::selectAircraftIcao(const CAircraftIcaoCode &icao)
    {
        if (!icao.isLoadedFromDb()) { return false; }
        return ui->tvp_AircraftIcao->selectDbKey(icao.getDbKey());
    }

    void CDbAircraftIcaoComponent::onIcaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
    {
        Q_UNUSED(count);
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::AircraftIcaoEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2000);
            ui->tvp_AircraftIcao->updateContainerMaybeAsync(sGui->getWebDataServices()->getAircraftIcaoCodes());
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " % CEntityFlags::stateToString(readState), 10000);
        }
    }

    void CDbAircraftIcaoComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url)
    {
        if (CEntityFlags::AircraftIcaoEntity != entity) { return; }
        this->showDownloadProgress(progress, current, max, url, 5000);
        Q_UNUSED(logId);
    }

    void CDbAircraftIcaoComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::AircraftIcaoEntity, QDateTime());
    }
} // ns
