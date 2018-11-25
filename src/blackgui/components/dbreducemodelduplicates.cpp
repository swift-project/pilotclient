/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbreducemodelduplicates.h"
#include "ui_dbreducemodelduplicates.h"

#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/statusmessage.h"

#include <QCompleter>
#include <QPushButton>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbReduceModelDuplicates::CDbReduceModelDuplicates(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CDbReduceModelDuplicates)
        {
            ui->setupUi(this);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
            ui->tvp_RemoveModels->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows);
            ui->tvp_RemoveModels->setAircraftModelMode(CAircraftModelListModel::OwnModelSet);
            ui->le_Distributor->setValidator(new CUpperCaseValidator(ui->le_Distributor));

            connect(ui->pb_Run, &QPushButton::clicked, this, &CDbReduceModelDuplicates::process);
        }

        CDbReduceModelDuplicates::~CDbReduceModelDuplicates()
        { }

        void CDbReduceModelDuplicates::setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            m_models = models;
            m_simulator = simulator;

            if (ui->le_Distributor->text().isEmpty())
            {
                const CDistributor distributor = m_distributorPreferences.get().getFirstOrDefaultDistributor(simulator);
                ui->le_Distributor->setText(distributor.getDbKey());
            }

            const QStringList distributors = models.getDistributors().getDbKeysAndAliases(true);
            QCompleter *c = new QCompleter(distributors, this);
            c->setCaseSensitivity(Qt::CaseInsensitive);
            ui->le_Distributor->setCompleter(c);
            ui->le_Models->setText(QStringLiteral("%1 models for simulator '%2', distributors: %3").arg(models.size()).arg(simulator.toQString(true)).arg(distributors.size()));
        }

        void CDbReduceModelDuplicates::process()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (m_models.isEmpty())
            {
                const CStatusMessage m = CStatusMessage(this).validationError("No models");
                ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
                return;
            }

            const CAircraftModelList keyDuplicates = m_models.findDuplicateModelStrings();
            if (!keyDuplicates.isEmpty())
            {
                const CStatusMessage m = CStatusMessage(this).validationError("Found %1 key duplicates") << keyDuplicates.size();
                ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
            }

            if (ui->le_Distributor->text().isEmpty())
            {
                const CStatusMessage m = CStatusMessage(this).validationError("No distributor");
                ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
                return;
            }

            const CDistributorList distributors = m_models.getDistributors();
            const CDistributor distributor = distributors.findByKeyOrAlias(ui->le_Distributor->text().trimmed().toUpper());
            if (!distributor.hasValidDbKey())
            {
                const CStatusMessage m = CStatusMessage(this).validationError("Invalid distributor");
                ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
                return;
            }

            const CAircraftModelList distributorModels = m_models.findByDistributor(distributor);
            CAircraftModelList nonDistributorModels = m_models;
            nonDistributorModels.removeByDistributor(distributor); // all models of other distrbutors
            CAircraftModelList removeModels;

            for (const CAircraftModel &distributorModel : distributorModels)
            {
                if (!distributorModel.getLivery().isAirlineLivery()) { continue; }
                if (!distributorModel.getAircraftIcaoCode().hasValidDbKey()) { continue; }
                if (ui->rb_SameLiveryAndAircraft->isChecked())
                {
                    removeModels.replaceOrAddModelsWithString(nonDistributorModels.findByAircraftAndLivery(distributorModel.getAircraftIcaoCode(), distributorModel.getLivery()), Qt::CaseInsensitive);
                }
                else if (ui->rb_SameAirlineAndAircraft->isChecked())
                {
                    removeModels.replaceOrAddModelsWithString(nonDistributorModels.findByAircraftAndAirline(distributorModel.getAircraftIcaoCode(), distributorModel.getAirlineIcaoCode()), Qt::CaseInsensitive);
                }
            }

            ui->tvp_RemoveModels->updateContainerMaybeAsync(removeModels);
            m_removeCandidates = removeModels;

            const QString distKeys = removeModels.getDistributors().dbKeysAsString(", ");
            const CStatusMessage m = removeModels.isEmpty() ?
                                     CStatusMessage(this).info("No duplicates to be removed!")  :
                                     CStatusMessage(this).info("You can remove %1 models of the following distributors: '%2'.") << removeModels.size() << distKeys;
            ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
        }
    } // ns
} // ns
