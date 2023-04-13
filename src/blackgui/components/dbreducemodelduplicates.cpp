/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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

namespace BlackGui::Components
{
    CDbReduceModelDuplicates::CDbReduceModelDuplicates(QWidget *parent) : QDialog(parent),
                                                                          ui(new Ui::CDbReduceModelDuplicates)
    {
        ui->setupUi(this);
        this->clearProgressBar();

        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        ui->tvp_RemoveModels->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows);
        ui->tvp_RemoveModels->setAircraftModelMode(CAircraftModelListModel::OwnModelSet);
        ui->le_Distributor->setValidator(new CUpperCaseValidator(ui->le_Distributor));
        ui->bb_ReduceModelDuplicates->button(QDialogButtonBox::Ok)->setEnabled(false);

        connect(ui->pb_Run, &QPushButton::clicked, this, &CDbReduceModelDuplicates::process);
        connect(ui->pb_Stop, &QPushButton::clicked, this, &CDbReduceModelDuplicates::stop);
        connect(this, &QDialog::finished, this, &CDbReduceModelDuplicates::stop);
    }

    CDbReduceModelDuplicates::~CDbReduceModelDuplicates()
    {}

    void CDbReduceModelDuplicates::setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
    {
        this->clearProgressBar();

        const bool changedSim = (m_simulator != simulator);
        if (changedSim)
        {
            ui->le_Distributor->clear();
            ui->tvp_RemoveModels->clear();
        }

        m_models = models;
        m_simulator = simulator;

        if (ui->le_Distributor->text().isEmpty())
        {
            const CDistributor distributor = m_distributorPreferences.get().getFirstOrDefaultDistributor(simulator);
            ui->le_Distributor->setText(distributor.getDbKey());
        }

        const QStringList distributors = models.getDistributors().getDbKeysAndAliases(true);
        const int distributorCount = m_models.getDistributors().sizeInt();
        QCompleter *c = new QCompleter(distributors, this);
        c->setCaseSensitivity(Qt::CaseInsensitive);
        ui->le_Distributor->setCompleter(c);
        ui->le_Models->setText(QStringLiteral("%1 models for simulator '%2', distributors: %3").arg(models.size()).arg(simulator.toQString(true)).arg(distributorCount));
    }

    void CDbReduceModelDuplicates::process()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        this->clearProgressBar();
        m_stop = false;

        if (m_models.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this).validationError(u"No models");
            ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
            return;
        }

        if (ui->le_Distributor->text().isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this).validationError(u"No distributor");
            ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
            return;
        }

        const CAircraftModelList keyDuplicates = m_models.findDuplicateModelStrings();
        if (!keyDuplicates.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this).validationError(u"Found %1 key duplicates") << keyDuplicates.size();
            ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
            return;
        }

        const CDistributorList distributors = m_models.getDistributors();
        const CDistributor distributor = distributors.findByKeyOrAlias(ui->le_Distributor->text().trimmed().toUpper());
        if (!distributor.hasValidDbKey())
        {
            const CStatusMessage m = CStatusMessage(this).validationError(u"Invalid distributor");
            ui->fr_Overlay->showOverlayHTMLMessage(m, 5000);
            return;
        }

        const CAircraftModelList distributorModels = m_models.findByDistributor(distributor);
        CAircraftModelList otherDistributorsModels = m_models;
        otherDistributorsModels.removeByDistributor(distributor); // all models of other distrbutors
        CAircraftModelList removeModels;

        ui->pb_Progress->setMaximum(distributorModels.sizeInt());
        ui->pb_Progress->setValue(0);

        int mc = 0;
        for (const CAircraftModel &distributorModel : distributorModels)
        {
            if (m_stop) { break; }
            if (!distributorModel.getLivery().isAirlineLivery()) { continue; }
            if (!distributorModel.getAircraftIcaoCode().hasValidDbKey()) { continue; }
            if (ui->rb_SameLiveryAndAircraft->isChecked())
            {
                removeModels.replaceOrAddModelsWithString(otherDistributorsModels.findByAircraftAndLivery(distributorModel.getAircraftIcaoCode(), distributorModel.getLivery()), Qt::CaseInsensitive);
            }
            else if (ui->rb_SameAirlineAndAircraft->isChecked())
            {
                removeModels.replaceOrAddModelsWithString(otherDistributorsModels.findByAircraftAndAirline(distributorModel.getAircraftIcaoCode(), distributorModel.getAirlineIcaoCode()), Qt::CaseInsensitive);
            }

            if (mc % 50 == 0) { ui->pb_Progress->setValue(mc); }
            if (mc % 100 == 0) { CGuiApplication::processEventsFor(10); }
            mc++;
        }

        if (!m_stop) { ui->pb_Progress->setValue(distributorModels.sizeInt()); }
        ui->tvp_RemoveModels->updateContainerMaybeAsync(removeModels);
        m_removeCandidates = removeModels;
        m_stop = false;

        const QString distKeys = removeModels.getDistributors().dbKeysAsString(", ");
        const CStatusMessage msg = removeModels.isEmpty() ?
                                       CStatusMessage(this).info(u"No duplicates to be removed!") :
                                       CStatusMessage(this).info(u"You can remove %1 models of the following distributors: '%2'.") << removeModels.size() << distKeys;
        ui->fr_Overlay->showOverlayHTMLMessage(msg, 5000);
        ui->bb_ReduceModelDuplicates->button(QDialogButtonBox::Ok)->setEnabled(true);
    }

    void CDbReduceModelDuplicates::updateProgressIndicator(int percentage)
    {
        const int delta = ui->pb_Progress->maximum() - ui->pb_Progress->minimum();
        const double p = percentage;
        const int value = qRound(delta * 100.0 / p);
        ui->pb_Progress->setValue(value);
    }

    void CDbReduceModelDuplicates::clearProgressBar()
    {
        ui->pb_Progress->setMaximum(100); // otherwise undetermined
        ui->pb_Progress->setMinimum(0);
        ui->pb_Progress->setValue(0);
        ui->pb_Progress->setFormat("%v/%m");
    }

} // ns
