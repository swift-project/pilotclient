/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dbaircraftcategorycomponent.h"
#include "ui_dbaircraftcategorycomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"

#include <QStringBuilder>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CDbAircraftCategoryComponent::CDbAircraftCategoryComponent(QWidget *parent) :
        COverlayMessagesFrame(parent),
        ui(new Ui::CDbAircraftCategoryComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_AircraftCategoryView);

        ui->tvp_AircraftCategoryView->setResizeMode(CAircraftCategoryView::ResizingOff);
        ui->tvp_AircraftCategoryView->allowDragDrop(true, false);
        ui->tvp_AircraftCategoryView->menuAddItems(CViewBaseNonTemplate::MenuCopy);
        // ui->tvp_AircraftCategoryView->setFilterWidget(ui->filter_AircraftIcao);

        connect(ui->tvp_AircraftCategoryView, &CAircraftCategoryView::requestNewBackendData, this, &CDbAircraftCategoryComponent::onReload);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAircraftCategoryComponent::onCategoryRead, Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this, &CDbAircraftCategoryComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onCategoryRead(CEntityFlags::AircraftCategoryEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getAircraftCategoriesCount());
    }

    CDbAircraftCategoryComponent::~CDbAircraftCategoryComponent()
    { }

    void CDbAircraftCategoryComponent::onCategoryRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
    {
        Q_UNUSED(count);
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::AircraftCategoryEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2000);
            const CAircraftCategoryList categories = sGui->getWebDataServices()->getAircraftCategories();
            ui->tvp_AircraftCategoryView->updateContainerMaybeAsync(categories);
            ui->tvp_AircraftCategoryTree->updateContainer(categories);
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " % CEntityFlags::stateToString(readState), 10000);
        }
    }

    void CDbAircraftCategoryComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url)
    {
        if (!entity.testFlag(CEntityFlags::AircraftCategoryEntity)) { return; }
        this->showDownloadProgress(progress, current, max, url, 5000);
        Q_UNUSED(logId);
    }

    void CDbAircraftCategoryComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::AircraftCategoryEntity, QDateTime());
    }

} // ns
