/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/modelsetbuilder.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbownmodelsetdialog.h"
#include "blackgui/editors/ownmodelsetform.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "ui_dbownmodelsetdialog.h"

#include <QPushButton>
#include <QString>
#include <QWidget>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        const CLogCategoryList &CDbOwnModelSetDialog::getLogCategories()
        {
            static const CLogCategoryList cats({ CLogCategory("swift.ownmodelset"), CLogCategory::guiComponent()});
            return cats;
        }

        CDbOwnModelSetDialog::CDbOwnModelSetDialog(QWidget *parent) :
            QDialog(parent),
            CDbMappingComponentAware(parent),
            ui(new Ui::CDbOwnModelSetDialog)
        {
            ui->setupUi(this);
            connect(ui->pb_Cancel, &QPushButton::clicked, this, &CDbOwnModelSetDialog::ps_buttonClicked);
            connect(ui->pb_Ok, &QPushButton::clicked, this, &CDbOwnModelSetDialog::ps_buttonClicked);
        }

        CDbOwnModelSetDialog::~CDbOwnModelSetDialog()
        {
            // void
        }

        void CDbOwnModelSetDialog::reloadData()
        {
            this->ui->form_OwnModelSet->reloadData();
        }

        int CDbOwnModelSetDialog::exec()
        {
            Q_ASSERT_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            this->m_simulatorInfo = this->getMappingComponent()->getOwnModelsSimulator();
            this->setWindowTitle("Create model set for " + this->m_simulatorInfo.toQString(true));
            return QDialog::exec();
        }

        void CDbOwnModelSetDialog::ps_buttonClicked()
        {
            const QObject *sender = QObject::sender();
            if (sender == ui->pb_Cancel)
            {
                this->reject();
            }
            else if (sender == ui->pb_Ok)
            {
                this->m_modelSet = this->buildSet(this->m_simulatorInfo, this->m_modelSet);
                this->accept();
            }
        }

        CAircraftModelList CDbOwnModelSetDialog::buildSet(const CSimulatorInfo &simulator, const CAircraftModelList &currentSet)
        {
            Q_ASSERT_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            const bool selectedProviders  = this->ui->form_OwnModelSet->selectedDistributors();
            const bool dbDataOnly = this->ui->form_OwnModelSet->dbDataOnly();
            const bool dbIcaoOnly = this->ui->form_OwnModelSet->dbIcaoCodesOnly();
            const bool incremnental = this->ui->form_OwnModelSet->incrementalBuild();

            const CAircraftModelList models = this->getMappingComponent()->getOwnModels();
            this->m_simulatorInfo = this->getMappingComponent()->getOwnModelsSimulator();
            const CDistributorList distributors = selectedProviders ?
                                                  this->ui->form_OwnModelSet->getSelectedDistributors() :
                                                  CDistributorList();
            const CModelSetBuilder builder(this);
            CModelSetBuilder::Builder options = selectedProviders ? CModelSetBuilder::FilterDistributos : CModelSetBuilder::NoOptions;
            if (dbDataOnly) { options |= CModelSetBuilder::OnlyDbData; }
            if (dbIcaoOnly) { options |= CModelSetBuilder::OnlyDbIcaoCodes; }
            if (incremnental) { options |= CModelSetBuilder::Incremental; }
            return builder.buildModelSet(simulator, models, currentSet, options, distributors);
        }
    } // ns
} // ns
