// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbairlineicaocomponent.h"
#include "blackgui/filters/airlineicaofilterbar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/airlineicaoview.h"
#include "blackgui/views/viewbase.h"
#include "ui_dbairlineicaocomponent.h"

#include <QDateTime>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackCore;
using namespace swift::misc::network;
using namespace BlackGui;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CDbAirlineIcaoComponent::CDbAirlineIcaoComponent(QWidget *parent) : COverlayMessagesFrame(parent),
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

        connect(ui->tvp_AirlineIcao, &CAirlineIcaoCodeView::requestNewBackendData, this, &CDbAirlineIcaoComponent::onReload);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAirlineIcaoComponent::onIcaoRead, Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this, &CDbAirlineIcaoComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onIcaoRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getAirlineIcaoCodesCount(), {});
    }

    CDbAirlineIcaoComponent::~CDbAirlineIcaoComponent()
    {}

    BlackGui::Views::CAirlineIcaoCodeView *CDbAirlineIcaoComponent::view() const
    {
        return ui->tvp_AirlineIcao;
    }

    void CDbAirlineIcaoComponent::onIcaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count, const QUrl &url)
    {
        Q_UNUSED(count)
        Q_UNUSED(url)

        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::AirlineIcaoEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2000);
            ui->tvp_AirlineIcao->updateContainerMaybeAsync(sGui->getWebDataServices()->getAirlineIcaoCodes());
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " % CEntityFlags::stateToString(readState), 10000);
        }
    }

    void CDbAirlineIcaoComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url)
    {
        if (!entity.testFlag(CEntityFlags::AirlineIcaoEntity)) { return; }
        this->showDownloadProgress(progress, current, max, url, 5000);
        Q_UNUSED(logId);
    }

    void CDbAirlineIcaoComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::AirlineIcaoEntity, QDateTime());
    }
} // ns
