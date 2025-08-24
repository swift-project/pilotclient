// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbdistributorcomponent.h"

#include <QFrame>
#include <QPointer>
#include <QtGlobal>

#include "ui_dbdistributorcomponent.h"

#include "core/webdataservices.h"
#include "gui/guiapplication.h"
#include "gui/views/distributorview.h"
#include "gui/views/viewbase.h"
#include "misc/simulation/simulatorinfo.h"

using namespace swift::core;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CDbDistributorComponent::CDbDistributorComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CDbDistributorComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_Distributors);
        ui->tvp_Distributors->setResizeMode(CViewBaseNonTemplate::ResizingAuto);
        ui->tvp_Distributors->allowDragDrop(true, false);
        ui->tvp_Distributors->setFilterWidget(ui->filter_Distributor);

        connect(ui->tvp_Distributors, &CDistributorView::requestNewBackendData, this, &CDbDistributorComponent::reload);
        connect(ui->pb_SelectAllFsFamily, &QPushButton::released, this, &CDbDistributorComponent::selectStandardModels);
        connect(ui->pb_SelectXPlaneBB, &QPushButton::released, this, &CDbDistributorComponent::selectStandardModels);
        connect(ui->pb_SelectXplaneXCSL, &QPushButton::released, this, &CDbDistributorComponent::selectStandardModels);

        if (sGui && sGui->getWebDataServices())
        {
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this,
                    &CDbDistributorComponent::onDistributorsRead, Qt::QueuedConnection);
            this->onDistributorsRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished,
                                     sGui->getWebDataServices()->getDistributorsCount());
        }
    }

    CDbDistributorComponent::~CDbDistributorComponent() {}

    CDistributorView *CDbDistributorComponent::view() const { return ui->tvp_Distributors; }

    void CDbDistributorComponent::filterBySimulator(const CSimulatorInfo &simulator)
    {
        ui->filter_Distributor->setSimulator(simulator);
    }

    CDistributorList CDbDistributorComponent::getSelectedDistributors() const
    {
        return ui->tvp_Distributors->selectedObjects();
    }

    bool CDbDistributorComponent::selectDistributor(const CDistributor &distributor)
    {
        if (ui->tvp_Distributors->isEmpty() || !distributor.isLoadedFromDb()) { return false; }
        return ui->tvp_Distributors->selectDbKey(distributor.getDbKey());
    }

    bool CDbDistributorComponent::hasSelectedDistributors() const { return ui->tvp_Distributors->hasSelection(); }

    void CDbDistributorComponent::onDistributorsRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState,
                                                     int count)
    {
        Q_UNUSED(count)
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        if (entity.testFlag(CEntityFlags::DistributorEntity) && CEntityFlags::isFinishedReadState(readState))
        {
            const CDistributorList distributors = sGui->getWebDataServices()->getDistributors();
            ui->tvp_Distributors->updateContainer(distributors);
        }
    }

    void CDbDistributorComponent::selectStandardModels()
    {
        const QObject *s = QObject::sender();
        QSet<QString> keys;
        if (s == ui->pb_SelectAllFsFamily)
        {
            this->filterBySimulator(CSimulatorInfo::AllFsFamily);
            keys = CDistributor::standardAllFsFamily();
        }
        else if (s == ui->pb_SelectXPlaneBB)
        {
            this->filterBySimulator(CSimulatorInfo::XPLANE);
            keys = QSet<QString>({ CDistributor::xplaneBlueBell() });
        }
        else if (s == ui->pb_SelectXplaneXCSL)
        {
            this->filterBySimulator(CSimulatorInfo::XPLANE);
            keys = QSet<QString>({ CDistributor::xplaneXcsl() });
        }

        // deferred because filter must first work and update
        const QPointer<CDbDistributorComponent> myself(this);
        QTimer::singleShot(2000, this, [=, this] {
            if (!myself || !sApp || sApp->isShuttingDown()) { return; }
            ui->tvp_Distributors->selectDbKeys(keys);
        });
    }

    void CDbDistributorComponent::reload()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::DistributorEntity);
    }
} // namespace swift::gui::components
