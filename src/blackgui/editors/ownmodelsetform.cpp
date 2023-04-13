/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;
using namespace BlackGui::Components;

namespace BlackGui::Editors
{
    COwnModelSetForm::COwnModelSetForm(QWidget *parent) : CForm(parent),
                                                          ui(new Ui::COwnModelSetForm)
    {
        ui->setupUi(this);
        ui->tvp_Distributors->setDistributorMode(CDistributorListModel::Minimal);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setLeftMargin(0);

        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &COwnModelSetForm::onSimulatorChanged, Qt::QueuedConnection);
        connect(ui->rb_DisplayAllDistributors, &QRadioButton::clicked, this, &COwnModelSetForm::changeDistributorDisplay);
        connect(ui->rb_DisplayPreferencesDistributors, &QRadioButton::clicked, this, &COwnModelSetForm::changeDistributorDisplay);

        QPointer<COwnModelSetForm> myself(this);
        QTimer::singleShot(1250, [=] {
            if (!myself) { return; }
            this->onSimulatorChanged(ui->comp_SimulatorSelector->getValue());
        });
    }

    COwnModelSetForm::~COwnModelSetForm()
    {
        // void
    }

    void COwnModelSetForm::reloadData()
    {
        const bool hasPreferences = this->hasDistributorPreferences();
        ui->cb_SortByPreferences->setChecked(hasPreferences);
        CGuiUtility::checkBoxReadOnly(ui->cb_SortByPreferences, !hasPreferences);
        ui->comp_SimulatorSelector->setValue(m_simulator);
        this->setDistributorView(hasPreferences);
        this->initDistributorDisplay();
    }

    bool COwnModelSetForm::optionUseSelectedDistributors() const
    {
        return ui->rb_DistributorsSelected->isChecked();
    }

    bool COwnModelSetForm::optionUseAllDistributors() const
    {
        return ui->rb_DistributorsAll->isChecked();
    }

    bool COwnModelSetForm::optionDbDataOnly() const
    {
        return ui->rb_DbDataOnly->isChecked();
    }

    bool COwnModelSetForm::optionIncrementalBuild() const
    {
        return ui->rb_Incremental->isChecked();
    }

    bool COwnModelSetForm::optionSortByDistributorPreferences() const
    {
        return ui->cb_SortByPreferences->isChecked() && this->hasDistributorPreferences();
    }

    bool COwnModelSetForm::optionConsolidateModelSetWithDbData() const
    {
        return ui->cb_ConsolidateModelSet->isChecked();
    }

    void COwnModelSetForm::onPreferencesChanged()
    {
        // void
    }

    void COwnModelSetForm::onSimulatorChanged(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Expect single simulator");
        this->setSimulator(simulator);
        this->reloadData();
        emit this->simulatorChanged(simulator);
    }

    void COwnModelSetForm::changeDistributorDisplay()
    {
        if (ui->rb_DisplayAllDistributors->isChecked())
        {
            ui->tvp_Distributors->updateContainerMaybeAsync(this->getAllDistributors());
            ui->cb_SortByPreferences->setChecked(false);
            CGuiUtility::checkBoxReadOnly(ui->cb_SortByPreferences, true);
            this->setDistributorView(false);
        }
        else
        {
            ui->tvp_Distributors->updateContainerMaybeAsync(this->getDistributorsFromPreferences());
            ui->cb_SortByPreferences->setChecked(true);
            CGuiUtility::checkBoxReadOnly(ui->cb_SortByPreferences, false);
            this->setDistributorView(true);
        }
    }

    void COwnModelSetForm::initDistributorDisplay()
    {
        if (this->hasDistributorPreferences())
        {
            ui->rb_DisplayPreferencesDistributors->setChecked(true);
        }
        else
        {
            ui->rb_DisplayAllDistributors->setChecked(true);
        }
        this->changeDistributorDisplay();
    }

    void COwnModelSetForm::setDistributorView(bool hasPreferences)
    {
        ui->tvp_Distributors->setDistributorMode(hasPreferences ? CDistributorListModel::MinimalWithOrder : CDistributorListModel::Minimal);
        ui->tvp_Distributors->fullResizeToContents();
        const CPropertyIndex i = hasPreferences ? CPropertyIndex(CDistributor::IndexOrder) : CPropertyIndex(CDistributor::IndexDbStringKey);
        ui->tvp_Distributors->sortByPropertyIndex(i);
    }

    CDistributorList COwnModelSetForm::getDistributorsFromPreferences() const
    {
        Q_ASSERT_X(m_simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        const CDistributorListPreferences prefs(m_distributorPreferences.get());
        const CDistributorList distributors(prefs.getDistributors(m_simulator));
        return distributors;
    }

    CDistributorList COwnModelSetForm::getAllDistributors() const
    {
        Q_ASSERT_X(m_simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        Q_ASSERT_X(sGui && sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web data services");
        return sGui->getWebDataServices()->getDistributors().matchesSimulator(m_simulator);
    }

    CDistributorList COwnModelSetForm::getDistributorsBasedOnOptions() const
    {
        if (ui->rb_DistributorsAll->isChecked()) { return this->getAllDistributors(); }
        if (ui->rb_DistributorsSelected->isChecked()) { return this->getSelectedDistributors(); }
        if (ui->rb_DistributorsFromBelow->isChecked()) { return this->getShownDistributors(); }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong option");
        return CDistributorList();
    }

    bool COwnModelSetForm::optionDbIcaoCodesOnly() const
    {
        return ui->rb_DbIcaoCodesOnly->isChecked();
    }

    CDistributorList COwnModelSetForm::getSelectedDistributors() const
    {
        return ui->tvp_Distributors->selectedObjects();
    }

    CDistributorList COwnModelSetForm::getShownDistributors() const
    {
        return ui->tvp_Distributors->containerOrFilteredContainer();
    }

    void COwnModelSetForm::setSimulator(const CSimulatorInfo &simulator)
    {
        // Ref T663, avoid ASSERT in some weird cases
        if (simulator.isSingleSimulator())
        {
            m_simulator = simulator;
        }
        else
        {
            const CSimulatorInfo resetSim = m_simulator.isSingleSimulator() ? m_simulator : CSimulatorInfo::guessDefaultSimulator();
            const QString msg = QStringLiteral("Set invalid simulator, continue to use '%1'").arg(resetSim.toQString(true));
            this->showOverlayHTMLMessage(msg);
            m_simulator = resetSim;
        }
    }

    bool COwnModelSetForm::hasDistributorPreferences() const
    {
        const CDistributorListPreferences prefs(m_distributorPreferences.get());
        return !prefs.getDistributors(m_simulator).isEmpty();
    }
} // ns
