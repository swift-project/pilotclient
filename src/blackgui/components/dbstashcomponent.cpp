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
#include "blackgui/guiapplication.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackcore/databasewriter.h"
#include "blackmisc/icons.h"
#include "blackmisc/simulation/aircraftmodel.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackGui;
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
            this->ui->tvp_StashAircraftModels->allowDragDrop(false, true);
            this->ui->tvp_StashAircraftModels->setAcceptedMetaTypeIds();
            this->ui->tvp_StashAircraftModels->menuAddItems(CAircraftModelView::MenuLoadAndSave);

            connect(this->ui->pb_Unstash, &QPushButton::pressed, this, &CDbStashComponent::ps_onUnstashPressed);
            connect(this->ui->pb_Validate, &QPushButton::pressed, this, &CDbStashComponent::ps_onValidatePressed);
            connect(this->ui->pb_Publish, &QPushButton::pressed, this, &CDbStashComponent::ps_onPublishPressed);
            connect(this->ui->tvp_StashAircraftModels, &CAircraftModelView::modelChanged, this, &CDbStashComponent::stashedModelsChanged);
            connect(this->ui->tvp_StashAircraftModels, &CAircraftModelView::rowCountChanged, this, &CDbStashComponent::ps_onRowCountChanged);

            // copy over buttons
            connect(this->ui->pb_AircraftIcao, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverPartsToSelected);
            connect(this->ui->pb_AirlineIcao, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverPartsToSelected);
            connect(this->ui->pb_Livery, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverPartsToSelected);
            connect(this->ui->pb_Distributor, &QPushButton::pressed, this, &CDbStashComponent::ps_copyOverPartsToSelected);

            ui->tvp_StashAircraftModels->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows);
            ui->tvp_StashAircraftModels->setHighlightModelStrings(true);
            ui->tvp_StashAircraftModels->setHighlightModelStringsColor(Qt::red);
            this->enableButtonRow();

            connect(sApp->getWebDataServices()->getDatabaseWriter(), &CDatabaseWriter::published, this, &CDbStashComponent::ps_publishResponse);
        }

        CDbStashComponent::~CDbStashComponent()
        { }

        void CDbStashComponent::gracefulShutdown()
        {
            // shutdown
        }

        CStatusMessage CDbStashComponent::validateStashModel(const CAircraftModel &model, bool allowReplace) const
        {
            if (!allowReplace && this->ui->tvp_StashAircraftModels->container().containsModelStringOrDbKey(model))
            {
                const QString msg("Model \"%1\" already stashed");
                return CStatusMessage(validationCategories(), CStatusMessage::SeverityError, msg.arg(model.getModelString()));
            }
            return CStatusMessage();
        }

        CStatusMessage CDbStashComponent::stashModel(const CAircraftModel &model, bool replace)
        {
            const CAircraftModel stashModel(this->consolidateModel(model));
            const CStatusMessage m(validateStashModel(stashModel, replace));
            if (!m.isWarningOrAbove())
            {
                if (replace)
                {
                    this->ui->tvp_StashAircraftModels->replaceOrAdd(&CAircraftModel::getModelString, stashModel.getModelString(), stashModel);
                }
                else
                {
                    this->ui->tvp_StashAircraftModels->insert(stashModel);
                }
            }
            return m;
        }

        CStatusMessageList CDbStashComponent::stashModels(const CAircraftModelList &models)
        {
            if (models.isEmpty()) { return CStatusMessageList(); }
            CStatusMessageList msgs;
            for (const CAircraftModel &model : models)
            {
                const CStatusMessage m(stashModel(model));
                if (m.isWarningOrAbove()) { msgs.push_back(m); }
            }
            return msgs;
        }

        void CDbStashComponent::replaceModelsUnvalidated(const CAircraftModelList &models)
        {
            this->ui->tvp_StashAircraftModels->updateContainerMaybeAsync(models);
        }

        int CDbStashComponent::unstashModels(QList<int> keys)
        {
            if (keys.isEmpty()) { return 0; }
            return this->ui->tvp_StashAircraftModels->removeDbKeys(keys);
        }

        int CDbStashComponent::unstashModels(QStringList modelStrings)
        {
            if (modelStrings.isEmpty()) { return 0; }
            return this->ui->tvp_StashAircraftModels->removeModelsWithModelString(modelStrings);
        }

        CAircraftModelView *CDbStashComponent::view() const
        {
            return ui->tvp_StashAircraftModels;
        }

        bool CDbStashComponent::hasStashedModels() const
        {
            return !this->ui->tvp_StashAircraftModels->isEmpty();
        }

        int CDbStashComponent::getStashedModelsCount() const
        {
            return this->ui->tvp_StashAircraftModels->rowCount();
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
            const CLivery stdLivery(sApp->getWebDataServices()->getStdLiveryForAirlineCode(icao));
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

        void CDbStashComponent::applyToSelected(const CPropertyIndexVariantMap &vm)
        {
            if (vm.isEmpty()) { return; }
            if (!this->ui->tvp_StashAircraftModels->hasSelection()) { return; }
            this->ui->tvp_StashAircraftModels->applyToSelected(vm);
        }

        void CDbStashComponent::ps_onUnstashPressed()
        {
            this->ui->tvp_StashAircraftModels->removeSelectedRows();
        }

        void CDbStashComponent::ps_onValidatePressed()
        {
            if (this->ui->tvp_StashAircraftModels->isEmpty()) {return; }
            this->validateAndDisplay(true);
        }

        void CDbStashComponent::ps_onPublishPressed()
        {
            if (this->ui->tvp_StashAircraftModels->isEmpty()) {return; }

            // get models right here, because later steps might affect selection
            CAircraftModelList models(getSelectedOrAllModels());
            if (models.isEmpty()) { return; }

            // validate
            if (!this->validateAndDisplay()) { return; }
            CStatusMessageList msgs;
            if (models.size() > MaxModelPublished)
            {
                CAircraftModelList::iterator i = models.begin();
                std::advance(i, MaxModelPublished);
                models.erase(i, models.end());
                msgs.push_back(CStatusMessage(validationCategories(), CStatusMessage::SeverityWarning, QString("More than %1 values, values skipped").arg(MaxModelPublished)));
            }

            msgs.push_back(sApp->getWebDataServices()->asyncPublishModels(models));
            if (msgs.hasWarningOrErrorMessages())
            {
                this->showMessages(msgs);
            }
            else
            {
                this->ui->tvp_StashAircraftModels->showLoadIndicator();
            }
        }

        void CDbStashComponent::ps_publishResponse(const CAircraftModelList &publishedModels, const CAircraftModelList &skippedModels, const CStatusMessageList &msgs)
        {
            this->ui->tvp_StashAircraftModels->hideLoadIndicator();
            if (!publishedModels.isEmpty())
            {
                emit this->modelsSuccessfullyPublished(publishedModels);
            }

            if (!msgs.isEmpty())
            {
                if (publishedModels.isEmpty())
                {
                    this->showMessages(msgs);
                }
                else
                {
                    QString confirm("Remove %1 published models?");
                    auto lambda = [this, publishedModels]()
                    {
                        this->unstashModels(publishedModels.getModelStrings(false));
                    };
                    this->showMessagesWithConfirmation(msgs, confirm.arg(publishedModels.size()), lambda, QMessageBox::Ok);
                }
            }

            Q_UNUSED(publishedModels);
            Q_UNUSED(skippedModels);
        }

        CStatusMessageList CDbStashComponent::validate(CAircraftModelList &invalidModels) const
        {
            if (this->ui->tvp_StashAircraftModels->isEmpty()) {return CStatusMessageList(); }
            CAircraftModelList models(getSelectedOrAllModels());
            if (models.isEmpty()) { return CStatusMessageList(); }

            const CStatusMessageList msgs(models.validateForPublishing(invalidModels));

            // OK?
            if (msgs.isEmpty())
            {
                return CStatusMessageList(
                {
                    CStatusMessage(validationCategories(), CStatusMessage::SeverityInfo, QString("No errors in %1 model(s)").arg(models.size()))
                });
            }
            else
            {
                return msgs;
            }
        }

        bool CDbStashComponent::validateAndDisplay(bool displayInfo)
        {
            CAircraftModelList invalidModels;
            const CStatusMessageList msgs(this->validate(invalidModels));
            if (msgs.hasWarningOrErrorMessages())
            {
                this->showMessages(msgs);
                this->ui->tvp_StashAircraftModels->setHighlightModelStrings(invalidModels.getModelStrings(false));
                return false;
            }
            else
            {
                this->ui->tvp_StashAircraftModels->setHighlightModelStrings(QStringList());
                if (displayInfo)
                {
                    QString no = QString::number(this->getStashedModelsCount());
                    CStatusMessage msg(validationCategories(), CStatusMessage::SeverityInfo, "Validation passed for " + no + " models");
                    this->showMessage(msg);
                }
                return true; // no error
            }
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

        const CLogCategoryList &CDbStashComponent::validationCategories() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));
            return cats;
        }

        CAircraftModelList CDbStashComponent::getSelectedOrAllModels() const
        {
            bool selectedOnly = ui->cb_SelectedOnly->isChecked();
            const CAircraftModelList models(selectedOnly ? this->ui->tvp_StashAircraftModels->selectedObjects() : this->ui->tvp_StashAircraftModels->containerOrFilteredContainer());
            return models;
        }

        CAircraftModel CDbStashComponent::consolidateWithDbData(const CAircraftModel &model) const
        {
            if (!model.hasModelString()) { return model; }
            CAircraftModel dbModel(sApp->getWebDataServices()->getModelForModelString(model.getModelString()));

            // we try to best update by DB data here
            if (!dbModel.hasValidDbKey())
            {
                // we have no(!) DB model, so we update each of it subobjects
                CAircraftModel consolidatedModel(model); // copy over
                if (!consolidatedModel.getLivery().hasValidDbKey())
                {
                    const CLivery dbLivery(sApp->getWebDataServices()->smartLiverySelector(consolidatedModel.getLivery()));
                    if (dbLivery.hasValidDbKey())
                    {
                        consolidatedModel.setLivery(dbLivery);
                    }
                }
                if (!consolidatedModel.getAircraftIcaoCode().hasValidDbKey() && consolidatedModel.hasAircraftDesignator())
                {
                    // try to find DB aircraft ICAO here
                    const CAircraftIcaoCode dbIcao(sGui->getWebDataServices()->smartAircraftIcaoSelector(consolidatedModel.getAircraftIcaoCode()));
                    if (dbIcao.hasValidDbKey())
                    {
                        consolidatedModel.setAircraftIcaoCode(dbIcao);
                    }
                }

                // key alone here can be misleading, as the key can be valid but no DB key
                // mostly happens when key is an alias
                QString keyOrAlias(consolidatedModel.getDistributor().getDbKey());
                CDistributor dbDistributor(sGui->getWebDataServices()->getDistributors().findByKeyOrAlias(keyOrAlias));

                // if no distributor is found, it is now empty because it was invalid
                // otherwise replaced with the current DB data
                consolidatedModel.setDistributor(dbDistributor);

                // copy over
                dbModel = consolidatedModel;
            }

            bool someDbData = dbModel.hasValidDbKey() || dbModel.getLivery().hasValidDbKey() || dbModel.getAircraftIcaoCode().hasValidDbKey();
            if (!someDbData) { return model; }

            // use DB model as base, update everything else
            dbModel.updateMissingParts(model);
            return dbModel;
        }

        CAircraftModel CDbStashComponent::consolidateWithOwnModels(const CAircraftModel &model) const
        {
            if (!model.hasModelString()) { return model; }
            if (model.getModelType() == CAircraftModel::TypeOwnSimulatorModel) { return model; }
            CAircraftModel ownModel(this->getMappingComponent()->getOwnModelForModelString(model.getModelString()));
            if (!ownModel.hasModelString()) { return model; }
            ownModel.updateMissingParts(model);
            return ownModel;
        }

        CAircraftModel CDbStashComponent::consolidateModel(const CAircraftModel &model) const
        {
            CAircraftModel stashModel(model);
            bool ownModel = stashModel.getModelType() == CAircraftModel::TypeOwnSimulatorModel;

            // merge with DB data if any
            if (!stashModel.hasValidDbKey())
            {
                stashModel = this->consolidateWithDbData(stashModel);
            }

            // merge with own models if any
            if (!ownModel)
            {
                stashModel = this->consolidateWithOwnModels(stashModel);
            }

            return stashModel;
        }

        void CDbStashComponent::ps_copyOverPartsToSelected()
        {
            QObject *sender = QObject::sender();
            BLACK_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            if (!this->getMappingComponent()) { return; }
            if (!this->ui->tvp_StashAircraftModels->hasSelection()) { return; }

            CAircraftModel model(this->getMappingComponent()->getEditorAircraftModel());
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

        bool CDbStashComponent::showMessages(const CStatusMessageList &msgs, bool onlyErrors, int timeoutMs)
        {
            if (msgs.isEmpty()) { return false; }
            if (!msgs.hasErrorMessages() && onlyErrors) { return false; }
            BLACK_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            if (!this->getMappingComponent()) { return false; }
            this->getMappingComponent()->showOverlayMessages(msgs, timeoutMs);
            return true;
        }

        bool CDbStashComponent::showMessagesWithConfirmation(const CStatusMessageList &msgs, const QString &confirmation, std::function<void ()> okLambda, int defaultButton, bool onlyErrors, int timeoutMs)
        {
            if (msgs.isEmpty()) { return false; }
            if (!msgs.hasErrorMessages() && onlyErrors) { return false; }
            BLACK_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            if (!this->getMappingComponent()) { return false; }
            this->getMappingComponent()->showOverlayMessagesWithConfirmation(msgs, confirmation, okLambda, defaultButton, timeoutMs);
            return true;
        }

        bool CDbStashComponent::showMessage(const CStatusMessage &msg, int timeoutMs)
        {
            if (msg.isEmpty()) { return false; }
            BLACK_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
            if (!this->getMappingComponent()) { return false; }
            this->getMappingComponent()->showOverlayMessage(msg, timeoutMs);
            return true;
        }
    } // ns
} // ns
