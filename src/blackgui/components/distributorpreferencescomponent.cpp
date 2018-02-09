/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/distributorpreferencescomponent.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/models/distributorlistmodel.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/orderable.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlistpreferences.h"
#include "blackmisc/statusmessage.h"
#include "ui_distributorpreferencescomponent.h"

#include <QFlags>
#include <QPushButton>
#include <QTimer>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {
        CDistributorPreferencesComponent::CDistributorPreferencesComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDistributorPreferencesComponent)
        {
            const CSimulatorInfo simulator = this->m_modelSetLoader.getSimulator();

            ui->setupUi(this);
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
            ui->comp_SimulatorSelector->setRememberSelection(true);
            ui->comp_SimulatorSelector->setValue(simulator);

            connect(ui->pb_All, &QPushButton::pressed, this, &CDistributorPreferencesComponent::ps_loadAll);
            connect(ui->pb_AllForSimulator, &QPushButton::pressed, this, &CDistributorPreferencesComponent::ps_loadAllForSimulator);
            connect(ui->pb_AllInSet, &QPushButton::pressed, this, &CDistributorPreferencesComponent::ps_loadDistributorsFromSet);
            connect(ui->pb_Save, &QPushButton::pressed, this, &CDistributorPreferencesComponent::ps_save);
            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CDistributorPreferencesComponent::ps_simulatorChanged);

            this->m_overlayMessageFrame = CGuiUtility::nextOverlayMessageFrame(this);
            Q_ASSERT_X(this->m_overlayMessageFrame, Q_FUNC_INFO, "Missing message frame");

            ui->tvp_Distributors->setDistributorMode(CDistributorListModel::NormalWithOrder);
            ui->tvp_Distributors->menuRemoveItems(CDistributorView::MenuBackend | CDistributorView::MenuDisplayAutomaticallyAndRefresh | CDistributorView::MenuLoadAndSave);
            ui->tvp_Distributors->menuAddItems(CDistributorView::MenuClear | CDistributorView::MenuOrderable | CDistributorView::MenuRemoveSelectedRows);
            ui->tvp_Distributors->initAsOrderable();

            QTimer::singleShot(1000, this, &CDistributorPreferencesComponent::ps_deferredInit);
        }

        CDistributorPreferencesComponent::~CDistributorPreferencesComponent()
        { }

        void CDistributorPreferencesComponent::ps_preferencesChanged()
        {
            // changed somewhere else
            const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
            const CDistributorList distributors = this->m_distributorPreferences.getThreadLocal().getDistributors(sim);
            this->updateContainerMaybeAsync(distributors);
        }

        void CDistributorPreferencesComponent::ps_loadAll()
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing application");
            const CDistributorList distributors(sGui->getWebDataServices()->getDistributors());
            if (distributors.isEmpty())
            {
                const CStatusMessage m = CStatusMessage(this).error("No distributors");
                this->m_overlayMessageFrame->showOverlayMessage(m);
                return;
            }
            this->updateContainerMaybeAsync(distributors);
        }

        void CDistributorPreferencesComponent::ps_loadAllForSimulator()
        {
            const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
            const CDistributorList distributors(sGui->getWebDataServices()->getDistributors().matchesSimulator(sim));
            if (distributors.isEmpty())
            {
                const CStatusMessage m = CStatusMessage(this).error("No distributors, or no distributors matching %1") << sim.toQString();
                this->m_overlayMessageFrame->showOverlayMessage(m);
                return;
            }
            this->updateContainerMaybeAsync(distributors);
        }

        void CDistributorPreferencesComponent::ps_loadDistributorsFromSet()
        {
            const CSimulatorInfo sim(ui->comp_SimulatorSelector->getValue());
            const CAircraftModelList models = this->m_modelSetLoader.getAircraftModels(sim);
            if (models.isEmpty())
            {
                const CStatusMessage m = CStatusMessage(this).error("No data in model set %1") << sim.toQString();
                this->m_overlayMessageFrame->showOverlayMessage(m);
                return;
            }
            const CDistributorList distributors = models.getDistributors();
            if (distributors.isEmpty())
            {
                const CStatusMessage m = CStatusMessage(this).error("No distributors for model set %1") << sim.toQString();
                this->m_overlayMessageFrame->showOverlayMessage(m);
                return;
            }
            this->updateContainerMaybeAsync(distributors);
        }

        void CDistributorPreferencesComponent::ps_save()
        {
            const CDistributorList distributors(ui->tvp_Distributors->container());
            const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
            CDistributorListPreferences preferences = this->m_distributorPreferences.getThreadLocal();
            preferences.setDistributors(distributors, simulator);
            const CStatusMessage m = this->m_distributorPreferences.setAndSave(preferences);
            CLogMessage::preformatted(m);
        }

        void CDistributorPreferencesComponent::ps_simulatorChanged(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Expect single simulator");
            const CDistributorList distributors(this->m_distributorPreferences.getThreadLocal().getDistributors(simulator));
            ui->tvp_Distributors->updateContainerMaybeAsync(distributors);
        }

        void CDistributorPreferencesComponent::ps_deferredInit()
        {
            this->ps_simulatorChanged(ui->comp_SimulatorSelector->getValue());
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
} // ns
