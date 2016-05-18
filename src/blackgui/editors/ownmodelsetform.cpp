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
#include "blackgui/guiapplication.h"
#include "blackgui/models/distributorlistmodel.h"
#include "blackgui/views/distributorview.h"
#include "blackmisc/simulation/distributorlist.h"
#include "ui_ownmodelsetform.h"

#include <QRadioButton>
#include <QtGlobal>

using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Editors
    {
        COwnModelSetForm::COwnModelSetForm(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::COwnModelSetForm)
        {
            ui->setupUi(this);
            this->ui->tvp_Distributors->setDistributorMode(CDistributorListModel::Minimal);
        }

        COwnModelSetForm::~COwnModelSetForm()
        {
            // void
        }

        void COwnModelSetForm::reloadData()
        {
            Q_ASSERT_X(sGui && sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web data services");
            const CDistributorList distributors(sGui->getWebDataServices()->getDistributors());
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

        bool COwnModelSetForm::dbIcaoCodesOnly() const
        {
            return this->ui->rb_DbIcaoCodesOnly->isChecked();
        }

        CDistributorList COwnModelSetForm::getSelectedDistributors() const
        {
            return ui->tvp_Distributors->selectedObjects();
        }
    } // ns
} // ns

