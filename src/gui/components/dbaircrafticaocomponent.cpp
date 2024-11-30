// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbaircrafticaocomponent.h"

#include <QDateTime>
#include <QStringBuilder>
#include <QtGlobal>

#include "dbaircrafticaocomponent.h"
#include "ui_dbaircrafticaocomponent.h"

#include "core/webdataservices.h"
#include "gui/filters/aircrafticaofilterbar.h"
#include "gui/guiapplication.h"
#include "gui/views/aircrafticaoview.h"
#include "gui/views/viewbase.h"

using namespace swift::core;
using namespace swift::misc::network;
using namespace swift::misc::aviation;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CDbAircraftIcaoComponent::CDbAircraftIcaoComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CDbAircraftIcaoComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_AircraftIcao);
        ui->tvp_AircraftIcao->setResizeMode(CAircraftIcaoCodeView::ResizingOff);

        ui->tvp_AircraftIcao->allowDragDrop(true, false);
        ui->tvp_AircraftIcao->setFilterWidget(ui->filter_AircraftIcao);
        ui->tvp_AircraftIcao->menuAddItems(CViewBaseNonTemplate::MenuCopy);

        connect(ui->tvp_AircraftIcao, &CAircraftIcaoCodeView::requestNewBackendData, this,
                &CDbAircraftIcaoComponent::onReload);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAircraftIcaoComponent::onIcaoRead,
                Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this,
                &CDbAircraftIcaoComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onIcaoRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished,
                         sGui->getWebDataServices()->getAircraftIcaoCodesCount());
    }

    CDbAircraftIcaoComponent::~CDbAircraftIcaoComponent() {}

    swift::gui::views::CAircraftIcaoCodeView *CDbAircraftIcaoComponent::view() const { return ui->tvp_AircraftIcao; }

    void CDbAircraftIcaoComponent::filter(const CAircraftIcaoCode &icao) { ui->filter_AircraftIcao->filter(icao); }

    bool CDbAircraftIcaoComponent::selectAircraftIcao(const CAircraftIcaoCode &icao)
    {
        if (!icao.isLoadedFromDb()) { return false; }
        return ui->tvp_AircraftIcao->selectDbKey(icao.getDbKey());
    }

    void CDbAircraftIcaoComponent::onIcaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
    {
        using namespace std::chrono_literals;

        Q_UNUSED(count);
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::AircraftIcaoEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2s);
            ui->tvp_AircraftIcao->updateContainerMaybeAsync(sGui->getWebDataServices()->getAircraftIcaoCodes());
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " %
                                             CEntityFlags::stateToString(readState),
                                         10s);
        }
    }

    void CDbAircraftIcaoComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress,
                                                            qint64 current, qint64 max, const QUrl &url)
    {
        using namespace std::chrono_literals;

        if (CEntityFlags::AircraftIcaoEntity != entity) { return; }
        this->showDownloadProgress(progress, current, max, url, 5s);
        Q_UNUSED(logId);
    }

    void CDbAircraftIcaoComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::AircraftIcaoEntity, QDateTime());
    }
} // namespace swift::gui::components
