/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbstashcomponent.h"
#include "dbmappingcomponent.h"
#include "ui_dbstashcomponent.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackmisc/icons.h"
#include "blackmisc/verify.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackGui::Models;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbStashComponent::CDbStashComponent(QWidget *parent) :
            QFrame(parent),
            CDbMappingComponentAware(parent),
            ui(new Ui::CDbStashComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_StashAircraftModels->setAircraftModelMode(CAircraftModelListModel::StashModel);
            this->ui->tvp_StashAircraftModels->allowDragDropValueObjects(false, true);
            this->ui->tvp_StashAircraftModels->setImplementedMetaTypeIds();
            this->ui->tvp_StashAircraftModels->menuAddItems(CAircraftModelView::MenuLoadAndSave);

            connect(this->ui->pb_Unstash, &QPushButton::pressed, this, &CDbStashComponent::ps_onUnstashPressed);
            connect(this->ui->pb_Validate, &QPushButton::pressed, this, &CDbStashComponent::ps_onValidatePressed);
            connect(this->ui->tvp_StashAircraftModels, &CAircraftModelView::modelChanged, this, &CDbStashComponent::stashedModelsChanged);
            connect(this->ui->tvp_StashAircraftModels, &CAircraftModelView::rowCountChanged, this, &CDbStashComponent::ps_onRowCountChanged);

            // copy over buttons
            connect(this->ui->pb_AircraftIcao, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverValues);
            connect(this->ui->pb_AirlineIcao, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverValues);
            connect(this->ui->pb_Livery, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverValues);
            connect(this->ui->pb_Distributor, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverValues);

            ui->tvp_StashAircraftModels->setCustomMenu(new CStashModelsMenu(this, true));
            this->enableButtonRow();
        }

        CDbStashComponent::~CDbStashComponent()
        { }

        void CDbStashComponent::setProvider(IWebDataServicesProvider *provider)
        {
            CWebDataServicesAware::setProvider(provider);
        }

        void CDbStashComponent::gracefulShutdown()
        {
            // shutdown
        }

        CStatusMessage CDbStashComponent::validateStashModel(const CAircraftModel &model) const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));
            if (this->ui->tvp_StashAircraftModels->container().containsModelStringOrId(model))
            {
                const QString msg("Model \"%1\" already stashed");
                return CStatusMessage(cats, CStatusMessage::SeverityError, msg.arg(model.getModelString()));
            }
            return CStatusMessage();
        }

        CStatusMessage CDbStashComponent::stashModel(const CAircraftModel &model)
        {
            CStatusMessage m(validateStashModel(model));
            if (!m.isWarningOrAbove())
            {
                this->ui->tvp_StashAircraftModels->insert(model);
            }
            return m;
        }

        CStatusMessageList CDbStashComponent::stashModels(const CAircraftModelList &models)
        {
            if (models.isEmpty()) { return CStatusMessageList(); }
            CStatusMessageList msgs;
            for (const CAircraftModel &model : models)
            {
                CStatusMessage m(stashModel(model));
                if (m.isWarningOrAbove()) { msgs.push_back(m); }
            }
            return msgs;
        }

        const Views::CAircraftModelView *CDbStashComponent::getView() const
        {
            return ui->tvp_StashAircraftModels;
        }

        bool CDbStashComponent::hasStashedModels() const
        {
            return !this->ui->tvp_StashAircraftModels->isEmpty();
        }

        QStringList CDbStashComponent::getStashedModelStrings() const
        {
            return this->ui->tvp_StashAircraftModels->derivedModel()->getModelStrings(false);
        }

        const CAircraftModelList &CDbStashComponent::getStashedModels() const
        {
            return this->ui->tvp_StashAircraftModels->derivedModel()->container();
        }

        void CDbStashComponent::applyToSelected(const CLivery &livery, bool acceptWarnings)
        {
            if (!this->ui->tvp_StashAircraftModels->hasSelection()) { return; }
            CStatusMessageList msgs(livery.validate());
            if (this->showMessages(msgs, acceptWarnings)) { return; }
            this->ui->tvp_StashAircraftModels->applyToSelected(livery);
        }

        void CDbStashComponent::applyToSelected(const CAircraftIcaoCode &icao, bool acceptWarnings)
        {
            if (!this->ui->tvp_StashAircraftModels->hasSelection()) { return; }
            CStatusMessageList msgs(icao.validate());
            if (this->showMessages(msgs, acceptWarnings)) { return; }
            this->ui->tvp_StashAircraftModels->applyToSelected(icao);
        }

        void CDbStashComponent::applyToSelected(const CAirlineIcaoCode &icao, bool acceptWarnings)
        {
            if (!icao.hasValidDesignator())
            {
                static const CStatusMessage msg(CStatusMessage::SeverityError, "No valid designator");
                this->showMessage(msg);
                return;
            }

            // retrieve the std livery
            const CLivery stdLivery(this->getStdLiveryForAirlineCode(icao));
            if (!stdLivery.hasValidDbKey())
            {
                static const CStatusMessage msg(CStatusMessage::SeverityError, "No valid standard livery for " + icao.getDesignator());
                this->showMessage(msg);
                return;
            }

            applyToSelected(stdLivery, acceptWarnings);
        }

        void CDbStashComponent::applyToSelected(const CDistributor &distributor, bool acceptWarnings)
        {
            if (!this->ui->tvp_StashAircraftModels->hasSelection()) { return; }
            CStatusMessageList msgs(distributor.validate());
            if (this->showMessages(msgs, acceptWarnings)) { return; }
            this->ui->tvp_StashAircraftModels->applyToSelected(distributor);
        }

        void CDbStashComponent::ps_onUnstashPressed()
        {
            this->ui->tvp_StashAircraftModels->removeSelectedRows();
        }

        void CDbStashComponent::ps_onValidatePressed()
        {
            if (this->ui->tvp_StashAircraftModels->isEmpty()) {return; }
            const CStatusMessageList msgs(this->validate());
            this->showMessages(msgs);
        }

        CStatusMessageList CDbStashComponent::validate() const
        {
            if (this->ui->tvp_StashAircraftModels->isEmpty()) {return CStatusMessageList(); }
            bool selectedOnly = ui->cb_SelectedOnly->isChecked();
            const CAircraftModelList models(selectedOnly ? this->ui->tvp_StashAircraftModels->selectedObjects() : this->ui->tvp_StashAircraftModels->container());
            if (models.isEmpty()) { return CStatusMessageList(); }
            const CStatusMessageList msgs(models.validateForPublishing());
            if (!msgs.isEmpty()) { return msgs; }

            return CStatusMessageList(
            {
                CStatusMessage(CStatusMessage::SeverityInfo, QString("No errors in %1 model(s)").arg(models.size()))
            });
        }

        void CDbStashComponent::enableButtonRow()
        {
            bool e = !this->ui->tvp_StashAircraftModels->isEmpty();
            this->ui->pb_AircraftIcao->setEnabled(e);
            this->ui->pb_AirlineIcao->setEnabled(e);
            this->ui->pb_Distributor->setEnabled(e);
            this->ui->pb_Livery->setEnabled(e);
            this->ui->pb_Publish->setEnabled(e);
            this->ui->pb_Unstash->setEnabled(e);
            this->ui->pb_Validate->setEnabled(e);
        }

        void CDbStashComponent::ps_copyOverValues()
        {
            QObject *sender = QObject::sender();
            BLACK_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            if (!this->getMappingComponent()) { return; }
            if (!this->ui->tvp_StashAircraftModels->hasSelection()) { return; }

            CAircraftModel model(this->getMappingComponent()->getAircraftModel());
            if (sender == this->ui->pb_AircraftIcao)
            {
                this->applyToSelected(model.getAircraftIcaoCode());
            }
            else if (sender == this->ui->pb_AirlineIcao)
            {
                this->applyToSelected(model.getAirlineIcaoCode());
            }
            else if (sender == this->ui->pb_Distributor)
            {
                this->applyToSelected(model.getDistributor());
            }
            else if (sender == this->ui->pb_Livery)
            {
                this->applyToSelected(model.getLivery());
            }
        }

        void CDbStashComponent::ps_onRowCountChanged(int number, bool filter)
        {
            Q_UNUSED(number);
            Q_UNUSED(filter);
            this->enableButtonRow();
        }

        bool CDbStashComponent::showMessages(const CStatusMessageList &msgs, bool onlyErrors)
        {
            if (msgs.isEmpty()) { return false; }
            if (!msgs.hasErrorMessages() && onlyErrors) { return false; }
            BLACK_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            if (!this->getMappingComponent()) { return false; }
            this->getMappingComponent()->showMessages(msgs);
            return true;
        }

        bool CDbStashComponent::showMessage(const CStatusMessage &msg)
        {
            if (msg.isEmpty()) { return false; }
            BLACK_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            if (!this->getMappingComponent()) { return false; }
            this->getMappingComponent()->showMessage(msg);
            return true;
        }

        void CDbStashComponent::CStashModelsMenu::customMenu(QMenu &menu) const
        {
            menu.addAction(CIcons::database16(), "Unstash", this->parent(), SLOT(ps_onUnstashPressed()));
            nestedCustomMenu(menu);
        }
    } // ns
} // ns
