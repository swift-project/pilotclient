// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbownmodelsetformdialog.h"

#include <QPushButton>
#include <QString>
#include <QWidget>
#include <QtGlobal>

#include "ui_dbownmodelsetformdialog.h"

#include "core/modelsetbuilder.h"
#include "gui/components/dbmappingcomponent.h"
#include "gui/editors/ownmodelsetform.h"
#include "gui/guiapplication.h"
#include "misc/logcategories.h"
#include "misc/simulation/distributorlist.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::core;
using namespace swift::gui::editors;

namespace swift::gui::components
{
    const QStringList &CDbOwnModelSetFormDialog::getLogCategories()
    {
        static const QStringList cats({ "swift.ownmodelset", CLogCategories::guiComponent() });
        return cats;
    }

    CDbOwnModelSetFormDialog::CDbOwnModelSetFormDialog(QWidget *parent)
        : QDialog(parent), CDbMappingComponentAware(parent), ui(new Ui::CDbOwnModelSetFormDialog)
    {
        ui->setupUi(this);
        connect(ui->pb_Cancel, &QPushButton::clicked, this, &CDbOwnModelSetFormDialog::buttonClicked);
        connect(ui->pb_Ok, &QPushButton::clicked, this, &CDbOwnModelSetFormDialog::buttonClicked);
        connect(ui->form_OwnModelSet, &COwnModelSetForm::simulatorChanged, this,
                &CDbOwnModelSetFormDialog::simulatorChanged);
    }

    CDbOwnModelSetFormDialog::~CDbOwnModelSetFormDialog()
    {
        // void
    }

    void CDbOwnModelSetFormDialog::reloadData()
    {
        m_simulatorInfo = this->getMappingComponent()->getOwnModelsSimulator();
        Q_ASSERT_X(m_simulatorInfo.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        ui->form_OwnModelSet->setSimulator(m_simulatorInfo);
        ui->form_OwnModelSet->reloadData();
        m_modelSet = this->getMappingComponent()->getOwnModelSet();
    }

    int CDbOwnModelSetFormDialog::exec()
    {
        Q_ASSERT_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
        const CSimulatorInfo sim(this->getMappingComponent()->getOwnModelsSimulator());
        Q_ASSERT_X(sim.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
        this->setSimulator(sim);
        this->checkData();
        return QDialog::exec();
    }

    void CDbOwnModelSetFormDialog::buttonClicked()
    {
        const QObject *sender = QObject::sender();
        if (sender == ui->pb_Cancel) { this->reject(); }
        else if (sender == ui->pb_Ok)
        {
            m_modelSet = this->buildSet(m_simulatorInfo, m_modelSet);
            this->accept();
        }
    }

    void CDbOwnModelSetFormDialog::simulatorChanged(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        this->setSimulator(simulator);

        // if possible set in mappings component
        if (this->getMappingComponent())
        {
            this->getMappingComponent()->setOwnModelsSimulator(simulator);
            this->getMappingComponent()->setOwnModelSetSimulator(simulator);
            this->checkData();
        }
    }

    bool CDbOwnModelSetFormDialog::checkData()
    {
        // models
        Q_ASSERT_X(m_simulatorInfo.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        const int c = this->getMappingComponent()->getOwnModelsCount();
        if (c < 1)
        {
            const CStatusMessage m = CStatusMessage(this).error(u"No models for '%1'")
                                     << m_simulatorInfo.toQString(true);
            ui->form_OwnModelSet->showOverlayMessage(m);
            return false;
        }
        return true;
    }

    void CDbOwnModelSetFormDialog::setSimulator(const CSimulatorInfo &simulator)
    {
        // Ref T663, avoid ASSERT in some weird cases
        if (simulator.isSingleSimulator()) { m_simulatorInfo = simulator; }
        else
        {
            const CSimulatorInfo resetSim =
                m_simulatorInfo.isSingleSimulator() ? m_simulatorInfo : CSimulatorInfo::guessDefaultSimulator();
            const QString msg =
                QStringLiteral("Set invalid simulator, continue to use '%1'").arg(resetSim.toQString(true));
            this->showMappingComponentOverlayHtmlMessage(msg);
            m_simulatorInfo = resetSim;
        }
        ui->form_OwnModelSet->setSimulator(m_simulatorInfo);
        this->setWindowTitle("Create model set for " + m_simulatorInfo.toQString(true));
    }

    CAircraftModelList CDbOwnModelSetFormDialog::buildSet(const CSimulatorInfo &simulator,
                                                          const CAircraftModelList &currentSet)
    {
        Q_ASSERT_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
        const bool givenDistributorsOnly = !ui->form_OwnModelSet->optionUseAllDistributors();
        const bool dbDataOnly = ui->form_OwnModelSet->optionDbDataOnly();
        const bool dbIcaoOnly = ui->form_OwnModelSet->optionDbIcaoCodesOnly();
        const bool incremnental = ui->form_OwnModelSet->optionIncrementalBuild();
        const bool sortByDistributor = ui->form_OwnModelSet->optionSortByDistributorPreferences();
        const bool consolidateWithDb = ui->form_OwnModelSet->optionConsolidateModelSetWithDbData();

        m_simulatorInfo = this->getMappingComponent()->getOwnModelsSimulator();
        const CAircraftModelList models = this->getMappingComponent()->getOwnModels();
        const CDistributorList distributors = ui->form_OwnModelSet->getDistributorsBasedOnOptions();

        if (givenDistributorsOnly && distributors.isEmpty())
        {
            // nothing to do, keep current set
            return currentSet;
        }

        const CModelSetBuilder builder(this);
        CModelSetBuilder::Builder options =
            givenDistributorsOnly ? CModelSetBuilder::GivenDistributorsOnly : CModelSetBuilder::NoOptions;
        if (dbDataOnly) { options |= CModelSetBuilder::OnlyDbData; }
        if (dbIcaoOnly) { options |= CModelSetBuilder::OnlyDbIcaoCodes; }
        if (incremnental) { options |= CModelSetBuilder::Incremental; }
        if (sortByDistributor) { options |= CModelSetBuilder::SortByDistributors; }
        if (consolidateWithDb) { options |= CModelSetBuilder::ConsolidateWithDb; }
        return builder.buildModelSet(simulator, models, currentSet, options, distributors);
    }
} // namespace swift::gui::components
