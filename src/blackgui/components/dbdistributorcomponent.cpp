/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dbdistributorcomponent.h"
#include "ui_dbdistributorcomponent.h"
#include "blackgui/components/dbdistributorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/views/viewbase.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QFrame>
#include <QPointer>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbDistributorComponent::CDbDistributorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbDistributorComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(ui->tvp_Distributors);
            ui->tvp_Distributors->setResizeMode(CViewBaseNonTemplate::ResizingAuto);
            ui->tvp_Distributors->allowDragDrop(true, false);
            ui->tvp_Distributors->setFilterWidget(ui->filter_Distributor);

            connect(ui->tvp_Distributors,     &CDistributorView::requestNewBackendData, this, &CDbDistributorComponent::reload);
            connect(ui->pb_SelectAllFsFamily, &QPushButton::released, this, &CDbDistributorComponent::selectStandardModels);
            connect(ui->pb_SelectXPlaneBB,    &QPushButton::released, this, &CDbDistributorComponent::selectStandardModels);
            connect(ui->pb_SelectXplaneXCSL,  &QPushButton::released, this, &CDbDistributorComponent::selectStandardModels);

            if (sGui && sGui->getWebDataServices())
            {
                connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbDistributorComponent::onDistributorsRead, Qt::QueuedConnection);
                this->onDistributorsRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getDistributorsCount());
            }
        }

        CDbDistributorComponent::~CDbDistributorComponent()
        { }

        CDistributorView *CDbDistributorComponent::view() const
        {
            return ui->tvp_Distributors;
        }

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

        bool CDbDistributorComponent::hasSelectedDistributors() const
        {
            return ui->tvp_Distributors->hasSelection();
        }

        void CDbDistributorComponent::onDistributorsRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
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
            QTimer::singleShot(2000, this, [ = ]
            {
                if (!myself || !sApp || sApp->isShuttingDown()) { return; }
                ui->tvp_Distributors->selectDbKeys(keys);
            });
        }

        void CDbDistributorComponent::reload()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
            sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::DistributorEntity);
        }
    } // ns
} // ns
