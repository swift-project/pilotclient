// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/distributorpreferencescomponent.h"

#include <QFlags>
#include <QPointer>
#include <QPushButton>
#include <QTimer>
#include <Qt>
#include <QtGlobal>

#include "ui_distributorpreferencescomponent.h"

#include "core/webdataservices.h"
#include "gui/components/simulatorselector.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "gui/models/distributorlistmodel.h"
#include "gui/overlaymessagesframe.h"
#include "gui/views/distributorview.h"
#include "gui/views/viewbase.h"
#include "misc/logmessage.h"
#include "misc/orderable.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/distributorlistpreferences.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::data;
using namespace swift::misc::network;
using namespace swift::gui::views;
using namespace swift::gui::models;

namespace swift::gui::components
{
    CDistributorPreferencesComponent::CDistributorPreferencesComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CDistributorPreferencesComponent)
    {
        ui->setupUi(this);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();

        connect(ui->pb_All, &QPushButton::pressed, this, &CDistributorPreferencesComponent::loadAllDistributors);
        connect(ui->pb_AllForSimulator, &QPushButton::pressed, this,
                &CDistributorPreferencesComponent::loadDistributorsForSimulator);
        connect(ui->pb_AllInSet, &QPushButton::pressed, this,
                &CDistributorPreferencesComponent::loadDistributorsFromSet);
        connect(ui->pb_Save, &QPushButton::pressed, this, &CDistributorPreferencesComponent::save);
        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this,
                &CDistributorPreferencesComponent::onSimulatorChanged);

        ui->tvp_Distributors->setDistributorMode(CDistributorListModel::NormalWithOrder);
        ui->tvp_Distributors->menuRemoveItems(CDistributorView::MenuBackend |
                                              CDistributorView::MenuDisplayAutomaticallyAndRefresh |
                                              CDistributorView::MenuLoadAndSave);
        ui->tvp_Distributors->menuAddItems(CDistributorView::MenuClear | CDistributorView::MenuOrderable |
                                           CDistributorView::MenuRemoveSelectedRows);
        ui->tvp_Distributors->initAsOrderable();

        this->triggerDeferredSimulatorChange();
    }

    CDistributorPreferencesComponent::~CDistributorPreferencesComponent() = default;

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
            const CStatusMessage m = CStatusMessage(this).error(u"No distributors, or no distributors matching %1")
                                     << sim.toQString();
            this->showOverlayMessage(m);
            return;
        }
        this->updateContainerMaybeAsync(distributors);
    }

    void CDistributorPreferencesComponent::loadDistributorsFromSet()
    {
        const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
        const CAircraftModelList models =
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(sim);
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
        using namespace std::chrono_literals;
        const CDistributorList distributors(ui->tvp_Distributors->container());
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        CDistributorListPreferences preferences = m_distributorPreferences.getThreadLocal();
        preferences.setDistributors(distributors, simulator);
        const CStatusMessage m = m_distributorPreferences.setAndSave(preferences);
        CLogMessage::preformatted(m);
        if (m.isSuccess()) { this->showOverlayHTMLMessage("Saved settings", 5s); }
        else { this->showOverlayMessage(m); }
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
        if (sortByOrder) { ui->tvp_Distributors->setSorting(CDistributor::IndexOrder, Qt::AscendingOrder); }
        ui->tvp_Distributors->updateContainerMaybeAsync(models);
    }
} // namespace swift::gui::components
