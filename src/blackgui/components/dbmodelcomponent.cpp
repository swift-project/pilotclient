// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbmodelcomponent.h"
#include "blackgui/filters/aircraftmodelfilterbar.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/guiapplication.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "ui_dbmodelcomponent.h"

#include <QDateTime>
#include <QWidget>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui::Components
{
    CDbModelComponent::CDbModelComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                            CDbMappingComponentAware(parent),
                                                            ui(new Ui::CDbModelComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");

        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_AircraftModel);
        ui->tvp_AircraftModel->setAircraftModelMode(CAircraftModelListModel::Database);
        ui->tvp_AircraftModel->menuAddItems(CAircraftModelView::MenuStashing);
        ui->tvp_AircraftModel->menuAddItems(CViewBaseNonTemplate::MenuCopy);
        ui->tvp_AircraftModel->menuRemoveItems(CAircraftModelView::MenuHighlightStashed); // not supported here

        // configure view
        ui->tvp_AircraftModel->setFilterWidget(ui->filter_AircraftModelFilter);
        ui->tvp_AircraftModel->allowDragDrop(true, false);

        connect(ui->tvp_AircraftModel, &CAircraftModelView::requestNewBackendData, this, &CDbModelComponent::onReload);
        connect(ui->tvp_AircraftModel, &CAircraftModelView::requestStash, this, &CDbModelComponent::requestStash);
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CDbModelComponent::onStyleSheetChanged, Qt::QueuedConnection);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbModelComponent::onModelsRead);
        connect(sGui->getWebDataServices(), &CWebDataServices::entityDownloadProgress, this, &CDbModelComponent::onEntityDownloadProgress, Qt::QueuedConnection);
        this->onModelsRead(CEntityFlags::ModelEntity, CEntityFlags::ReadFinished, sApp->getWebDataServices()->getModelsCount(), {});
    }

    CDbModelComponent::~CDbModelComponent()
    {
        // void
    }

    bool CDbModelComponent::hasModels() const
    {
        return !ui->tvp_AircraftModel->isEmpty();
    }

    void CDbModelComponent::requestUpdatedData()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }

        QDateTime ts;
        if (!ui->tvp_AircraftModel->isEmpty())
        {
            CAircraftModel model(ui->tvp_AircraftModel->container().latestObject());
            ts = model.getUtcTimestamp();
        }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::ModelEntity, ts);
    }

    void CDbModelComponent::onModelsRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count, const QUrl &url)
    {
        Q_UNUSED(count)
        Q_UNUSED(url)

        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (!entity.testFlag(CEntityFlags::ModelEntity)) { return; }

        if (CEntityFlags::isFinishedReadState(readState))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Updating %1").arg(CEntityFlags::entitiesToString(entity)), 2000);
            ui->tvp_AircraftModel->updateContainerMaybeAsync(sGui->getWebDataServices()->getModels());
        }
        else
        {
            this->showOverlayHTMLMessage(u"Current state: " % CEntityFlags::entitiesToString(entity) % u" " % CEntityFlags::stateToString(readState), 10000);
        }
    }

    void CDbModelComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::ModelEntity);
    }

    void CDbModelComponent::onStyleSheetChanged()
    {
        // code goes here
    }

    void CDbModelComponent::onEntityDownloadProgress(CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url)
    {
        if (!entity.testFlag(CEntityFlags::ModelEntity)) { return; }
        this->showDownloadProgress(progress, current, max, url, 5000);
        Q_UNUSED(logId)
    }
} // ns
