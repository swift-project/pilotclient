// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbliverycomponent.h"

#include <QStringBuilder>
#include <QtGlobal>

#include "ui_dbliverycomponent.h"

#include "core/webdataservices.h"
#include "gui/filters/liveryfilterbar.h"
#include "gui/guiapplication.h"
#include "gui/views/liveryview.h"

using namespace swift::core;
using namespace swift::gui::views;
using namespace swift::misc::aviation;
using namespace swift::misc::network;

namespace swift::gui::components
{
    CDbLiveryComponent::CDbLiveryComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CDbLiveryComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_Liveries);
        connect(ui->tvp_Liveries, &CLiveryView::requestNewBackendData, this, &CDbLiveryComponent::onReload);

        // filter and drag and drop
        ui->tvp_Liveries->setFilterWidget(ui->filter_Livery);
        ui->tvp_Liveries->allowDragDrop(true, false);
        ui->tvp_Liveries->menuAddItems(CViewBaseNonTemplate::MenuCopy);

        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLiveryComponent::onLiveriesRead,
                Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this,
                &CDbLiveryComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onLiveriesRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFinished,
                             sGui->getWebDataServices()->getLiveriesCount());
    }

    CDbLiveryComponent::~CDbLiveryComponent() = default;

    CLiveryView *CDbLiveryComponent::view() { return ui->tvp_Liveries; }

    void CDbLiveryComponent::filter(const swift::misc::aviation::CLivery &livery) { ui->filter_Livery->filter(livery); }

    void CDbLiveryComponent::filterByAirline(const CAirlineIcaoCode &icao) { ui->filter_Livery->filter(icao); }

    void CDbLiveryComponent::onLiveriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
    {
        using namespace std::chrono_literals;

        Q_UNUSED(count);
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::LiveryEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2s);
            ui->tvp_Liveries->updateContainerMaybeAsync(sGui->getWebDataServices()->getLiveries());
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " %
                                             CEntityFlags::stateToString(readState),
                                         10s);
        }
    }

    void CDbLiveryComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress,
                                                      qint64 current, qint64 max, const QUrl &url)
    {
        using namespace std::chrono_literals;

        if (!entity.testFlag(CEntityFlags::LiveryEntity)) { return; }
        this->showDownloadProgress(progress, current, max, url, 5s);
        Q_UNUSED(logId);
    }

    void CDbLiveryComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::LiveryEntity);
    }
} // namespace swift::gui::components
