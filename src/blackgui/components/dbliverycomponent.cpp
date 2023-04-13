/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbliverycomponent.h"
#include "blackgui/filters/liveryfilterbar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/liveryview.h"
#include "ui_dbliverycomponent.h"

#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackCore;
using namespace BlackGui::Views;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CDbLiveryComponent::CDbLiveryComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                              ui(new Ui::CDbLiveryComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_Liveries);
        connect(ui->tvp_Liveries, &CLiveryView::requestNewBackendData, this, &CDbLiveryComponent::onReload);

        // filter and drag and drop
        ui->tvp_Liveries->setFilterWidget(ui->filter_Livery);
        ui->tvp_Liveries->allowDragDrop(true, false);
        ui->tvp_Liveries->menuAddItems(CViewBaseNonTemplate::MenuCopy);

        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLiveryComponent::onLiveriesRead, Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this, &CDbLiveryComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onLiveriesRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getLiveriesCount());
    }

    CDbLiveryComponent::~CDbLiveryComponent()
    {}

    CLiveryView *CDbLiveryComponent::view()
    {
        return ui->tvp_Liveries;
    }

    void CDbLiveryComponent::filter(const BlackMisc::Aviation::CLivery &livery)
    {
        ui->filter_Livery->filter(livery);
    }

    void CDbLiveryComponent::filterByAirline(const CAirlineIcaoCode &icao)
    {
        ui->filter_Livery->filter(icao);
    }

    void CDbLiveryComponent::onLiveriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
    {
        Q_UNUSED(count);
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::LiveryEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2000);
            ui->tvp_Liveries->updateContainerMaybeAsync(sGui->getWebDataServices()->getLiveries());
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " % CEntityFlags::stateToString(readState), 10000);
        }
    }

    void CDbLiveryComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url)
    {
        if (!entity.testFlag(CEntityFlags::LiveryEntity)) { return; }
        this->showDownloadProgress(progress, current, max, url, 5000);
        Q_UNUSED(logId);
    }

    void CDbLiveryComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::LiveryEntity);
    }
} // ns
