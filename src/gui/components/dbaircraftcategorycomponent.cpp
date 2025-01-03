// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbaircraftcategorycomponent.h"

#include <QStringBuilder>

#include "ui_dbaircraftcategorycomponent.h"

#include "core/webdataservices.h"
#include "gui/guiapplication.h"

using namespace swift::core;
using namespace swift::misc::network;
using namespace swift::misc::aviation;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CDbAircraftCategoryComponent::CDbAircraftCategoryComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CDbAircraftCategoryComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_AircraftCategoryView);

        ui->tvp_AircraftCategoryView->setResizeMode(CAircraftCategoryView::ResizingOff);
        ui->tvp_AircraftCategoryView->allowDragDrop(true, false);
        ui->tvp_AircraftCategoryView->menuAddItems(CViewBaseNonTemplate::MenuCopy);
        // ui->tvp_AircraftCategoryView->setFilterWidget(ui->filter_AircraftIcao);

        connect(ui->tvp_AircraftCategoryView, &CAircraftCategoryView::requestNewBackendData, this,
                &CDbAircraftCategoryComponent::onReload);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this,
                &CDbAircraftCategoryComponent::onCategoryRead, Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this,
                &CDbAircraftCategoryComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onCategoryRead(CEntityFlags::AircraftCategoryEntity, CEntityFlags::ReadFinished,
                             sGui->getWebDataServices()->getAircraftCategoriesCount());
    }

    CDbAircraftCategoryComponent::~CDbAircraftCategoryComponent() {}

    void CDbAircraftCategoryComponent::onCategoryRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState,
                                                      int count)
    {
        using namespace std::chrono_literals;
        Q_UNUSED(count);
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::AircraftCategoryEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2s);
            const CAircraftCategoryList categories = sGui->getWebDataServices()->getAircraftCategories();
            ui->tvp_AircraftCategoryView->updateContainerMaybeAsync(categories);
            ui->tvp_AircraftCategoryTree->updateContainer(categories);
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " %
                                             CEntityFlags::stateToString(readState),
                                         10s);
        }
    }

    void CDbAircraftCategoryComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress,
                                                                qint64 current, qint64 max, const QUrl &url)
    {
        using namespace std::chrono_literals;

        if (!entity.testFlag(CEntityFlags::AircraftCategoryEntity)) { return; }
        this->showDownloadProgress(progress, current, max, url, 5s);
        Q_UNUSED(logId);
    }

    void CDbAircraftCategoryComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::AircraftCategoryEntity, QDateTime());
    }

} // namespace swift::gui::components
