/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/distributorpreferencescomponent.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/models/distributorlistmodel.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlistpreferences.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/orderable.h"
#include "blackmisc/statusmessage.h"
#include "ui_distributorpreferencescomponent.h"

#include <QFlags>
#include <QPushButton>
#include <QTimer>
#include <Qt>
#include <QtGlobal>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;
using namespace BlackMisc::Network;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui::Components
{
    CDistributorPreferencesComponent::CDistributorPreferencesComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                                          ui(new Ui::CDistributorPreferencesComponent)
    {
        ui->setupUi(this);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();

        connect(ui->pb_All, &QPushButton::pressed, this, &CDistributorPreferencesComponent::loadAllDistributors);
        connect(ui->pb_AllForSimulator, &QPushButton::pressed, this, &CDistributorPreferencesComponent::loadDistributorsForSimulator);
        connect(ui->pb_AllInSet, &QPushButton::pressed, this, &CDistributorPreferencesComponent::loadDistributorsFromSet);
        connect(ui->pb_Save, &QPushButton::pressed, this, &CDistributorPreferencesComponent::save);
        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CDistributorPreferencesComponent::onSimulatorChanged);

        ui->tvp_Distributors->setDistributorMode(CDistributorListModel::NormalWithOrder);
        ui->tvp_Distributors->menuRemoveItems(CDistributorView::MenuBackend | CDistributorView::MenuDisplayAutomaticallyAndRefresh | CDistributorView::MenuLoadAndSave);
        ui->tvp_Distributors->menuAddItems(CDistributorView::MenuClear | CDistributorView::MenuOrderable | CDistributorView::MenuRemoveSelectedRows);
        ui->tvp_Distributors->initAsOrderable();

        this->triggerDeferredSimulatorChange();
    }

    CDistributorPreferencesComponent::~CDistributorPreferencesComponent()
    {}

    void CDistributorPreferencesComponent::onPreferencesChanged()
    {
        // changed somewhere else
        const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
        const CDistributorList distributors = m_distributorPreferences.getThreadLocal().getDistributors(sim);
        this->updateContainerMaybeAsync(distributors);
    }

    void CDistributorPreferencesComponent::loadAllDistributors()
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing application");
        const CDistributorList distributors(sGui->getWebDataServices()->getDistributors());
        if (distributors.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this).error(u"No distributors");
            this->showOverlayMessage(m);
            return;
        }
        this->updateContainerMaybeAsync(distributors);
    }

    void CDistributorPreferencesComponent::loadDistributorsForSimulator()
    {
        const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
        const CDistributorList distributors(sGui->getWebDataServices()->getDistributors().matchesSimulator(sim));
        if (distributors.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this).error(u"No distributors, or no distributors matching %1") << sim.toQString();
            this->showOverlayMessage(m);
            return;
        }
        this->updateContainerMaybeAsync(distributors);
    }

    void CDistributorPreferencesComponent::loadDistributorsFromSet()
    {
        const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
        const CAircraftModelList models = CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(sim);
        if (models.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this).error(u"No data in model set %1") << sim.toQString();
            this->showOverlayMessage(m);
            return;
        }
        const CDistributorList distributors = models.getDistributors();
        if (distributors.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this).error(u"No distributors for model set %1") << sim.toQString();
            this->showOverlayMessage(m);
            return;
        }
        this->updateContainerMaybeAsync(distributors);
    }

    void CDistributorPreferencesComponent::save()
    {
        const CDistributorList distributors(ui->tvp_Distributors->container());
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        CDistributorListPreferences preferences = m_distributorPreferences.getThreadLocal();
        preferences.setDistributors(distributors, simulator);
        const CStatusMessage m = m_distributorPreferences.setAndSave(preferences);
        CLogMessage::preformatted(m);
    }

    void CDistributorPreferencesComponent::onSimulatorChanged(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Expect single simulator");
        const CDistributorList distributors(m_distributorPreferences.getThreadLocal().getDistributors(simulator));
        ui->tvp_Distributors->updateContainerMaybeAsync(distributors);
    }

    void CDistributorPreferencesComponent::triggerDeferredSimulatorChange()
    {
        QPointer<CDistributorPreferencesComponent> myself(this);
        QTimer::singleShot(1000, this, [=] {
            if (!myself) { return; }
            if (!sApp || sApp->isShuttingDown()) { return; }
            const CSimulatorInfo sim = ui->comp_SimulatorSelector->getValue();
            this->onSimulatorChanged(sim);
        });
    }

    void CDistributorPreferencesComponent::updateContainerMaybeAsync(const CDistributorList &models, bool sortByOrder)
    {
        if (sortByOrder)
        {
            ui->tvp_Distributors->setSorting(CDistributor::IndexOrder, Qt::AscendingOrder);
        }
        ui->tvp_Distributors->updateContainerMaybeAsync(models);
    }
} // ns
