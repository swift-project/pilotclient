// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbstashcomponent.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/guiapplication.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "core/db/databasewriter.h"
#include "core/db/databaseutils.h"
#include "core/application.h"
#include "core/webdataservices.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/distributorlist.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/livery.h"
#include "misc/logcategories.h"
#include "misc/sequence.h"
#include "misc/verify.h"
#include "ui_dbstashcomponent.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>
#include <QStringBuilder>
#include <Qt>
#include <QtGlobal>
#include <iterator>

using namespace swift::core;
using namespace swift::core::db;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CDbStashComponent::CDbStashComponent(QWidget *parent) : QFrame(parent),
                                                            CDbMappingComponentAware(parent),
                                                            ui(new Ui::CDbStashComponent)
    {
        ui->setupUi(this);

        connect(ui->pb_Unstash, &QPushButton::pressed, this, &CDbStashComponent::onUnstashPressed);
        connect(ui->pb_Validate, &QPushButton::pressed, this, &CDbStashComponent::onValidatePressed);
        connect(ui->pb_RemoveInvalid, &QPushButton::pressed, this, &CDbStashComponent::onRemoveInvalidPressed);
        connect(ui->pb_Publish, &QPushButton::pressed, this, &CDbStashComponent::onPublishPressed);
        connect(ui->tvp_StashAircraftModels, &CAircraftModelView::modelChanged, this, &CDbStashComponent::stashedModelsChanged);
        connect(ui->tvp_StashAircraftModels, &CAircraftModelView::modelDataChanged, this, &CDbStashComponent::onRowCountChanged);

        // copy over buttons
        connect(ui->pb_AircraftIcao, &QPushButton::pressed, this, &CDbStashComponent::copyOverValuesToSelectedModels);
        connect(ui->pb_AirlineIcao, &QPushButton::pressed, this, &CDbStashComponent::copyOverValuesToSelectedModels);
        connect(ui->pb_Livery, &QPushButton::pressed, this, &CDbStashComponent::copyOverValuesToSelectedModels);
        connect(ui->pb_Distributor, &QPushButton::pressed, this, &CDbStashComponent::copyOverValuesToSelectedModels);
        connect(ui->pb_Model, &QPushButton::pressed, this, &CDbStashComponent::modifyModelDialog);

        ui->tvp_StashAircraftModels->setAircraftModelMode(CAircraftModelListModel::StashModel);
        ui->tvp_StashAircraftModels->allowDragDrop(false, true, true);
        ui->tvp_StashAircraftModels->setAcceptedMetaTypeIds();
        ui->tvp_StashAircraftModels->menuAddItems(CAircraftModelView::MenuLoadAndSave);
        ui->tvp_StashAircraftModels->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows);
        ui->tvp_StashAircraftModels->setHighlight(true);
        ui->tvp_StashAircraftModels->setHighlightColor(Qt::red);
        ui->tvp_StashAircraftModels->setSettingsDirectoryIndex(CDirectories::IndexDirLastModelStashJsonOrDefault);
        this->enableButtonRow();

        connect(sApp->getWebDataServices()->getDatabaseWriter(), &CDatabaseWriter::publishedModels, this, &CDbStashComponent::onPublishedModelsResponse, Qt::QueuedConnection);
        this->onUserChanged();
    }

    CDbStashComponent::~CDbStashComponent()
    {}

    CStatusMessage CDbStashComponent::validateStashModel(const CAircraftModel &model, bool allowReplace) const
    {
        if (!allowReplace && ui->tvp_StashAircraftModels->container().containsModelStringOrDbKey(model))
        {
            const QString msg("Model '%1' already stashed");
            return CStatusMessage(validationCategories(), CStatusMessage::SeverityError, msg.arg(model.getModelString()));
        }
        return CStatusMessage();
    }

    CStatusMessage CDbStashComponent::stashModel(const CAircraftModel &model, bool replace, bool consolidateWithDbData, bool clearHighlighting)
    {
        const CAircraftModel stashModel(consolidateWithDbData ? this->consolidateModel(model) : model);
        const CStatusMessage m(this->validateStashModel(stashModel, replace));
        if (!m.isWarningOrAbove())
        {
            if (clearHighlighting) { this->clearValidationHighlighting(); }
            if (replace)
            {
                ui->tvp_StashAircraftModels->replaceOrAdd(&CAircraftModel::getModelString, stashModel.getModelString(), stashModel);
            }
            else
            {
                ui->tvp_StashAircraftModels->insert(stashModel);
            }
        }
        return m;
    }

    CStatusMessageList CDbStashComponent::stashModels(const CAircraftModelList &models, bool replace, bool consolidateWithDbData, bool clearHighlighting)
    {
        if (models.isEmpty()) { return CStatusMessageList(); }
        CStatusMessageList msgs;
        int successfullyAdded = 0;
        for (const CAircraftModel &model : models)
        {
            const CStatusMessage m(this->stashModel(model, replace, consolidateWithDbData, false));
            if (m.isWarningOrAbove())
            {
                msgs.push_back(m);
            }
            else
            {
                successfullyAdded++;
            }
        }
        if (successfullyAdded > 0 && clearHighlighting) { this->clearValidationHighlighting(); }
        return msgs;
    }

    void CDbStashComponent::replaceModelsUnvalidated(const CAircraftModelList &models)
    {
        ui->tvp_StashAircraftModels->updateContainerMaybeAsync(models);
    }

    int CDbStashComponent::unstashModels(const QSet<int> &keys)
    {
        if (keys.isEmpty()) { return 0; }
        return ui->tvp_StashAircraftModels->removeDbKeys(keys);
    }

    int CDbStashComponent::unstashModels(const QStringList &modelStrings)
    {
        if (modelStrings.isEmpty()) { return 0; }
        return ui->tvp_StashAircraftModels->removeModelsWithModelString(modelStrings);
    }

    int CDbStashComponent::unstashModels(const CAircraftModelList &models)
    {
        if (models.isEmpty()) { return 0; }
        return ui->tvp_StashAircraftModels->removeModelsWithModelString(models);
    }

    CAircraftModelView *CDbStashComponent::view() const
    {
        return ui->tvp_StashAircraftModels;
    }

    bool CDbStashComponent::hasStashedModels() const
    {
        return !ui->tvp_StashAircraftModels->isEmpty();
    }

    int CDbStashComponent::getStashedModelsCount() const
    {
        return ui->tvp_StashAircraftModels->rowCount();
    }

    QStringList CDbStashComponent::getStashedModelStrings() const
    {
        return ui->tvp_StashAircraftModels->derivedModel()->getModelStrings(false);
    }

    const CAircraftModelList &CDbStashComponent::getStashedModels() const
    {
        return ui->tvp_StashAircraftModels->derivedModel()->container();
    }

    CAircraftModel CDbStashComponent::getStashedModel(const QString &modelString) const
    {
        if (modelString.isEmpty() || ui->tvp_StashAircraftModels->isEmpty()) { return CAircraftModel(); }
        return ui->tvp_StashAircraftModels->container().findFirstByModelStringOrDefault(modelString);
    }

    void CDbStashComponent::applyToSelected(const CLivery &livery, bool acceptWarnings)
    {
        if (!ui->tvp_StashAircraftModels->hasSelection()) { return; }
        CStatusMessageList msgs(livery.validate());
        if (this->showOverlayMessages(msgs, acceptWarnings)) { return; }
        ui->tvp_StashAircraftModels->applyToSelected(livery);
    }

    void CDbStashComponent::applyToSelected(const CAircraftIcaoCode &icao, bool acceptWarnings)
    {
        if (!ui->tvp_StashAircraftModels->hasSelection()) { return; }
        CStatusMessageList msgs(icao.validate());
        if (this->showOverlayMessages(msgs, acceptWarnings)) { return; }
        ui->tvp_StashAircraftModels->applyToSelected(icao);
    }

    void CDbStashComponent::applyToSelected(const CAirlineIcaoCode &icao, bool acceptWarnings)
    {
        if (!icao.hasValidDesignator())
        {
            static const CStatusMessage msg(CStatusMessage::SeverityError, u"No valid designator");
            this->showOverlayMessage(msg);
            return;
        }

        // retrieve the std livery
        const CLivery stdLivery(sApp->getWebDataServices()->getStdLiveryForAirlineCode(icao));
        if (!stdLivery.hasValidDbKey())
        {
            static const CStatusMessage msg(CStatusMessage::SeverityError, u"No valid standard livery for " % icao.getDesignator());
            this->showOverlayMessage(msg);
            return;
        }

        applyToSelected(stdLivery, acceptWarnings);
    }

    void CDbStashComponent::applyToSelected(const CDistributor &distributor, bool acceptWarnings)
    {
        if (!ui->tvp_StashAircraftModels->hasSelection()) { return; }
        CStatusMessageList msgs(distributor.validate());
        if (this->showOverlayMessages(msgs, acceptWarnings)) { return; }
        ui->tvp_StashAircraftModels->applyToSelected(distributor);
    }

    void CDbStashComponent::applyToSelected(const CPropertyIndexVariantMap &vm)
    {
        if (vm.isEmpty()) { return; }
        if (!ui->tvp_StashAircraftModels->hasSelection()) { return; }
        ui->tvp_StashAircraftModels->applyToSelected(vm);
    }

    void CDbStashComponent::onUnstashPressed()
    {
        ui->tvp_StashAircraftModels->removeSelectedRows();
    }

    void CDbStashComponent::onValidatePressed()
    {
        if (ui->tvp_StashAircraftModels->isEmpty()) { return; }
        CAircraftModelList validModels;
        CAircraftModelList invalidModels;
        this->validateAndDisplay(validModels, invalidModels, true);
    }

    void CDbStashComponent::onRemoveInvalidPressed()
    {
        if (ui->tvp_StashAircraftModels->isEmpty()) { return; }
        CAircraftModelList validModels;
        CAircraftModelList invalidModels;
        this->validate(validModels, invalidModels);
        this->unstashModels(invalidModels);
    }

    void CDbStashComponent::onPublishPressed()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        if (ui->tvp_StashAircraftModels->isEmpty()) { return; }
        if (!sGui->hasMinimumMappingVersion()) { return; }

        // get models right here, because later steps might affect selection
        const CAircraftModelList models(getSelectedOrAllModels());
        if (models.isEmpty()) { return; }

        // validate
        CAircraftModelList validModels;
        CAircraftModelList invalidModels;
        if (!this->validateAndDisplay(validModels, invalidModels)) { return; }
        CStatusMessageList msgs;
        if (validModels.size() > MaxModelPublished)
        {
            validModels.truncate(MaxModelPublished);
            msgs.push_back(CStatusMessage(validationCategories(), CStatusMessage::SeverityWarning, u"More than %1 values, values skipped") << MaxModelPublished);
        }

        msgs.push_back(sGui->getWebDataServices()->asyncPublishModels(validModels));
        if (msgs.hasWarningOrErrorMessages())
        {
            this->showOverlayMessages(msgs);
        }
        else
        {
            ui->tvp_StashAircraftModels->showLoadIndicator();
        }
    }

    void CDbStashComponent::onPublishedModelsResponse(const CAircraftModelList &publishedModels, const CAircraftModelList &skippedModels, const CStatusMessageList &msgs, bool sendingSuccesful, bool directWrite)
    {
        Q_UNUSED(skippedModels);
        ui->tvp_StashAircraftModels->hideLoadIndicator();
        if (!publishedModels.isEmpty() && sendingSuccesful)
        {
            emit this->modelsSuccessfullyPublished(publishedModels, directWrite);
        }

        if (!msgs.isEmpty())
        {
            if (publishedModels.isEmpty())
            {
                this->showOverlayMessages(msgs, false, false);
            }
            else
            {
                QPointer<CDbStashComponent> myself(this);
                const QString confirm("Remove %1 published models from stash?");
                auto lambda = [=]() {
                    if (!myself) { return; }
                    myself->unstashModels(publishedModels.getModelStringList(false));
                };
                this->showOverlayMessagesWithConfirmation(msgs, false, confirm.arg(publishedModels.size()), lambda, QMessageBox::Ok);
            }
        }
    }

    CStatusMessageList CDbStashComponent::validate(CAircraftModelList &validModels, CAircraftModelList &invalidModels) const
    {
        if (ui->tvp_StashAircraftModels->isEmpty()) { return CStatusMessageList(); }
        Q_ASSERT_X(sGui->getWebDataServices(), Q_FUNC_INFO, "No web services");
        const CAircraftModelList models(getSelectedOrAllModels());
        if (models.isEmpty()) { return CStatusMessageList(); }
        const bool ignoreEqual = ui->cb_ChangedOnly->isChecked();
        const CStatusMessageList msgs(sGui->getWebDataServices()->validateForPublishing(models, ignoreEqual, validModels, invalidModels));

        // OK?
        if (msgs.isEmpty())
        {
            return CStatusMessageList(
                { CStatusMessage(validationCategories(), CStatusMessage::SeverityInfo, u"No errors in %1 model(s)") << models.size() });
        }
        else
        {
            return msgs;
        }
    }

    bool CDbStashComponent::validateAndDisplay(CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool displayInfo)
    {
        const CStatusMessageList msgs(this->validate(validModels, invalidModels));
        if (msgs.hasWarningOrErrorMessages())
        {
            this->showOverlayMessages(msgs);
            ui->tvp_StashAircraftModels->setHighlightModelStrings(invalidModels.getModelStringList(false));
        }
        else
        {
            // delete highlighting because no errors
            ui->tvp_StashAircraftModels->clearHighlighting();
            if (displayInfo)
            {
                const QString no = QString::number(this->getStashedModelsCount());
                const CStatusMessage msg(validationCategories(), CStatusMessage::SeverityInfo, "Validation passed for " + no + " models");
                this->showOverlayMessage(msg);
            }
        }
        return !validModels.isEmpty(); // at least some valid objects
    }

    void CDbStashComponent::enableButtonRow()
    {
        const bool e = !ui->tvp_StashAircraftModels->isEmpty();
        ui->pb_AircraftIcao->setEnabled(e);
        ui->pb_AirlineIcao->setEnabled(e);
        ui->pb_Distributor->setEnabled(e);
        ui->pb_Livery->setEnabled(e);
        ui->pb_Unstash->setEnabled(e);
        ui->pb_Validate->setEnabled(e);
        ui->pb_RemoveInvalid->setEnabled(e);
        ui->pb_Model->setEnabled(e);
        ui->pb_Publish->setEnabled(e);
        this->onUserChanged();
    }

    const CLogCategoryList &CDbStashComponent::validationCategories() const
    {
        static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
        return cats;
    }

    CAircraftModelList CDbStashComponent::getSelectedOrAllModels() const
    {
        const bool selectedOnly = ui->cb_SelectedOnly->isChecked();
        const CAircraftModelList models(selectedOnly ? ui->tvp_StashAircraftModels->selectedObjects() : ui->tvp_StashAircraftModels->containerOrFilteredContainer());
        return models;
    }

    CAircraftModel CDbStashComponent::consolidateWithDbData(const CAircraftModel &model, bool forced) const
    {
        const CAircraftModel consolidatedModel = CDatabaseUtils::consolidateModelWithDbData(model, forced);
        return consolidatedModel;
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

    CAuthenticatedUser CDbStashComponent::getSwiftDbUser() const
    {
        return m_swiftDbUser.get();
    }

    CAircraftModel CDbStashComponent::consolidateModel(const CAircraftModel &model) const
    {
        CAircraftModel stashModel(model);
        const bool ownModel = stashModel.getModelType() == CAircraftModel::TypeOwnSimulatorModel;

        // merge/update with DB data if any
        // this is a forced update with DB data, as DB data can change
        stashModel = this->consolidateWithDbData(stashModel, true);

        // merge with own models if any
        if (!ownModel)
        {
            stashModel = this->consolidateWithOwnModels(stashModel);
        }

        return stashModel;
    }

    void CDbStashComponent::showChangedAttributes()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        if (sGui->isShuttingDown()) { return; }
        const CAircraftModelList models = ui->tvp_StashAircraftModels->selectedObjects();
        if (models.isEmpty()) { return; }

        CStatusMessageList msgs;
        for (const CAircraftModel &model : models)
        {
            CStatusMessageList modelMsgs;
            const bool equal = sGui->getWebDataServices()->isDbModelEqualForPublishing(model, &modelMsgs);
            if (equal)
            {
                msgs.push_back(CStatusMessage(this).info(u"Model '%1' has no change values") << model.getModelStringAndDbKey());
            }
            else
            {
                msgs.push_back(modelMsgs);
            }
        }
        this->showOverlayMessages(msgs);
    }

    void CDbStashComponent::clearValidationHighlighting()
    {
        ui->tvp_StashAircraftModels->clearHighlighting();
    }

    void CDbStashComponent::copyOverValuesToSelectedModels()
    {
        const QObject *sender = QObject::sender();
        SWIFT_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "Missing mapping component");
        if (!this->getMappingComponent()) { return; }
        if (!ui->tvp_StashAircraftModels->hasSelection()) { return; }

        const CAircraftModel model(this->getMappingComponent()->getEditorAircraftModel());
        if (sender == ui->pb_AircraftIcao)
        {
            this->applyToSelected(model.getAircraftIcaoCode());
        }
        else if (sender == ui->pb_AirlineIcao)
        {
            this->applyToSelected(model.getAirlineIcaoCode());
        }
        else if (sender == ui->pb_Distributor)
        {
            this->applyToSelected(model.getDistributor());
        }
        else if (sender == ui->pb_Livery)
        {
            this->applyToSelected(model.getLivery());
        }
    }

    void CDbStashComponent::modifyModelDialog()
    {
        if (this->getMappingComponent())
        {
            this->getMappingComponent()->modifyModelDialog();
        }
    }

    void CDbStashComponent::onRowCountChanged(int number, bool filter)
    {
        Q_UNUSED(number);
        Q_UNUSED(filter);
        this->enableButtonRow();
    }

    void CDbStashComponent::onUserChanged()
    {
        const CAuthenticatedUser user(this->getSwiftDbUser());
        if (!user.isAuthenticated())
        {
            ui->pb_Publish->setText(" Publish (login) ");
            ui->pb_Publish->setToolTip("Login first");
            ui->pb_Publish->setEnabled(false);
        }
        else if (user.canDirectlyWriteModels())
        {
            ui->pb_Publish->setText(" Publish (dir.) ");
            ui->pb_Publish->setToolTip("Models directly released");
            ui->pb_Publish->setEnabled(true);
        }
        else
        {
            ui->pb_Publish->setText(" Publish (CR) ");
            ui->pb_Publish->setToolTip("Models published as change request");
            ui->pb_Publish->setEnabled(true);
        }
    }

    bool CDbStashComponent::showOverlayMessages(const CStatusMessageList &msgs, bool onlyErrors, bool appendOldMessages, int timeoutMs)
    {
        if (msgs.isEmpty()) { return false; }
        if (!msgs.hasErrorMessages() && onlyErrors) { return false; }
        SWIFT_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
        if (!this->getMappingComponent()) { return false; }

        this->getMappingComponent()->showOverlayMessages(msgs, appendOldMessages, timeoutMs);
        return true;
    }

    bool CDbStashComponent::showOverlayMessagesWithConfirmation(
        const CStatusMessageList &msgs, bool appendOldMessages,
        const QString &confirmation, std::function<void()> okLambda, QMessageBox::StandardButton defaultButton, bool onlyErrors, int timeoutMs)
    {
        if (msgs.isEmpty()) { return false; }
        if (!msgs.hasErrorMessages() && onlyErrors) { return false; }
        SWIFT_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
        if (!this->getMappingComponent()) { return false; }
        this->getMappingComponent()->showOverlayMessagesWithConfirmation(msgs, appendOldMessages, confirmation, okLambda, defaultButton, timeoutMs);
        return true;
    }

    bool CDbStashComponent::showOverlayMessage(const CStatusMessage &msg, int timeoutMs)
    {
        if (msg.isEmpty()) { return false; }
        SWIFT_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
        if (!this->getMappingComponent()) { return false; }
        this->getMappingComponent()->showOverlayMessage(msg, timeoutMs);
        return true;
    }

    void CDbStashComponent::clearOverlayMessages()
    {
        SWIFT_VERIFY_X(this->getMappingComponent(), Q_FUNC_INFO, "missing mapping component");
        if (!this->getMappingComponent()) { return; }
        this->getMappingComponent()->clearOverlayMessages();
    }
} // ns
