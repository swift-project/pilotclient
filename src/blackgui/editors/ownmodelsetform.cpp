/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/editors/ownmodelsetform.h"
#include "blackgui/models/distributorlistmodel.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/views/distributorview.h"
#include "blackmisc/simulation/distributorlist.h"
#include "ui_ownmodelsetform.h"

#include <QRadioButton>
#include <QtGlobal>

using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        COwnModelSetForm::COwnModelSetForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::COwnModelSetForm)
        {
            ui->setupUi(this);
            ui->tvp_Distributors->setDistributorMode(CDistributorListModel::Minimal);
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
            ui->comp_SimulatorSelector->setLeftMargin(0);
            CGuiUtility::checkBoxReadOnly(ui->cb_Preferences, true);

            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &COwnModelSetForm::ps_simulatorChanged);
        }

        COwnModelSetForm::~COwnModelSetForm()
        {
            // void
        }

        void COwnModelSetForm::reloadData()
        {
            const CDistributorList distributors(this->getDistributors());
            const bool hasPreferences = this->hasDIstributorPreferences();
            ui->cb_Preferences->setChecked(hasPreferences);
            ui->comp_SimulatorSelector->setValue(this->m_simulator);
            ui->tvp_Distributors->setDistributorMode(hasPreferences ? CDistributorListModel::MinimalWithOrder : CDistributorListModel::Minimal);
            if (!distributors.isEmpty())
            {
                this->ui->tvp_Distributors->updateContainerMaybeAsync(distributors);
            }
        }

        bool COwnModelSetForm::selectedDistributors() const
        {
            return this->ui->rb_SelectedDistributors->isChecked();
        }

        bool COwnModelSetForm::dbDataOnly() const
        {
            return this->ui->rb_DbDataOnly->isChecked();
        }

        bool COwnModelSetForm::incrementalBuild() const
        {
            return ui->rb_Incremental->isChecked();
        }

        void COwnModelSetForm::ps_preferencesChanged()
        {
            // void
        }

        void COwnModelSetForm::ps_simulatorChanged(const CSimulatorInfo &simulator)
        {
            this->setSimulator(simulator);
            this->reloadData();
            emit simulatorChanged(simulator);
        }

        CDistributorList COwnModelSetForm::getDistributors() const
        {
            Q_ASSERT_X(sGui && sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web data services");
            Q_ASSERT_X(this->m_simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const CDistributorListPreferences prefs(this->m_distributorPreferences.get());
            const CDistributorList distributors(prefs.getDistributors(this->m_simulator));
            if (!distributors.isEmpty()) { return distributors; }

            // no preferences
            return sGui->getWebDataServices()->getDistributors().matchesSimulator(this->m_simulator);
        }

        bool COwnModelSetForm::dbIcaoCodesOnly() const
        {
            return this->ui->rb_DbIcaoCodesOnly->isChecked();
        }

        CDistributorList COwnModelSetForm::getSelectedDistributors() const
        {
            return ui->tvp_Distributors->selectedObjects();
        }

        void COwnModelSetForm::setSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            m_simulator = simulator;
        }

        bool COwnModelSetForm::hasDIstributorPreferences() const
        {
            const CDistributorListPreferences prefs(this->m_distributorPreferences.get());
            return !prefs.getDistributors(this->m_simulator).isEmpty();
        }
    } // ns
} // ns

