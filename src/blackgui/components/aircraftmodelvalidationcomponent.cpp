// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ui_aircraftmodelvalidationcomponent.h"
#include "aircraftmodelvalidationcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"

#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CAircraftModelValidationComponent::CAircraftModelValidationComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                                            ui(new Ui::CAircraftModelValidationComponent)
    {
        ui->setupUi(this);
        ui->comp_Simulator->setMode(CSimulatorSelector::ComboBox);
        ui->comp_Simulator->setRememberSelection(false);
        ui->comp_Messages->setNoSorting(); // keep order
        ui->tvp_InvalidModels->setValidationContextMenu(false);

        const CAircraftMatcherSetup setup = m_matchingSettings.get();
        ui->cb_EnableStartupCheck->setChecked(setup.doVerificationAtStartup());
        ui->cb_OnlyIfErrorsOrWarnings->setChecked(setup.onlyShowVerificationWarningsAndErrors());

        connect(ui->cb_EnableStartupCheck, &QCheckBox::toggled, this, &CAircraftModelValidationComponent::onCheckAtStartupChanged);
        connect(ui->cb_OnlyIfErrorsOrWarnings, &QCheckBox::toggled, this, &CAircraftModelValidationComponent::onOnlyErrorWarningChanged);

        connect(ui->pb_TempDisableInvalid, &QPushButton::released, this, &CAircraftModelValidationComponent::onTempDisabledButtonClicked, Qt::QueuedConnection);
        connect(ui->pb_TempDisableSelected, &QPushButton::released, this, &CAircraftModelValidationComponent::onTempDisabledButtonClicked, Qt::QueuedConnection);
        connect(ui->pb_RemoveInvalid, &QPushButton::released, this, &CAircraftModelValidationComponent::onRemoveButtonClicked, Qt::QueuedConnection);
        connect(ui->pb_RemoveSelected, &QPushButton::released, this, &CAircraftModelValidationComponent::onRemoveButtonClicked, Qt::QueuedConnection);

        connect(ui->pb_TriggerValidation, &QPushButton::released, this, &CAircraftModelValidationComponent::triggerValidation, Qt::QueuedConnection);
        connect(ui->pb_Help, &QPushButton::released, this, &CAircraftModelValidationComponent::showHelp, Qt::QueuedConnection);

        // 1st init when running in distributed environment
        QPointer<CAircraftModelValidationComponent> myself(this);
        const qint64 lastResults = m_lastResults;
        QTimer::singleShot(2500, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            if (m_lastResults > lastResults) { return; } // values received in meantime
            myself->requestLastResults();
        });
    }

    CAircraftModelValidationComponent::~CAircraftModelValidationComponent()
    {}

    void CAircraftModelValidationComponent::validatedModelSet(const CSimulatorInfo &simulator, const CAircraftModelList &valid, const CAircraftModelList &invalid, bool stopped, const CStatusMessageList &msgs)
    {
        // BlackMisc::Simulation::CBackgroundValidation
        Q_UNUSED(simulator)
        Q_UNUSED(valid)

        constexpr int MsgTimeout = 15000;
        m_lastResults = QDateTime::currentMSecsSinceEpoch();
        ui->tvp_InvalidModels->updateContainerMaybeAsync(invalid);
        ui->comp_Simulator->setValue(simulator);
        ui->comp_Messages->clear();
        if (!msgs.isEmpty()) { ui->comp_Messages->appendStatusMessagesToList(msgs); }

        // pre-select tab
        if (invalid.isEmpty() && !msgs.isEmpty())
        {
            // messages but no invalid models
            ui->tw_CAircraftModelValidationComponent->setCurrentWidget(ui->tb_Messages);
        }
        else
        {
            ui->tw_CAircraftModelValidationComponent->setCurrentWidget(ui->tb_InvalidModels);
        }

        const CAircraftMatcherSetup setup = m_matchingSettings.get();
        ui->cb_EnableStartupCheck->setChecked(setup.doVerificationAtStartup());
        ui->pb_TempDisableInvalid->setEnabled(!invalid.isEmpty());
        ui->pb_TempDisableSelected->setEnabled(!invalid.isEmpty());
        ui->pb_RemoveInvalid->setEnabled(!invalid.isEmpty());
        ui->pb_RemoveSelected->setEnabled(!invalid.isEmpty());

        const QString msg = stopped ?
                                QStringLiteral("Validation for '%1' stopped, maybe your models are not accessible or too many issues").arg(simulator.toQString(true)) :
                                QStringLiteral("Validated for '%1'. Valid: %2 Invalid: %3").arg(simulator.toQString(true)).arg(valid.size()).arg(invalid.size());
        ui->lbl_Summay->setText(msg);
        if (stopped)
        {
            this->showOverlayHTMLMessage(msg, MsgTimeout);
        }
        else if (msgs.hasWarningOrErrorMessages() || !invalid.isEmpty())
        {
            this->showOverlayHTMLMessage(u"There are warnings or errors, please check the messages and invalid models.", MsgTimeout);
        }
        else
        {
            this->showOverlayHTMLMessage(u"There are NO warnings or errors, your set looks good.", MsgTimeout);
        }
    }

    void CAircraftModelValidationComponent::tempDisableModels(const CAircraftModelList &models)
    {
        if (models.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->supportsContexts()) { return; }
        if (!sGui->getIContextSimulator()) { return; }
        sGui->getIContextSimulator()->disableModelsForMatching(models, true);
        this->saveInvalidModels(models);
    }

    void CAircraftModelValidationComponent::onCheckAtStartupChanged(bool checked)
    {
        CAircraftMatcherSetup setup = m_matchingSettings.get();
        if (setup.doVerificationAtStartup() == checked) { return; }
        setup.setVerificationAtStartup(checked);
        const CStatusMessage msg = m_matchingSettings.setAndSave(setup);
        Q_UNUSED(msg)
    }

    void CAircraftModelValidationComponent::onOnlyErrorWarningChanged(bool checked)
    {
        CAircraftMatcherSetup setup = m_matchingSettings.get();
        if (setup.onlyShowVerificationWarningsAndErrors() == checked) { return; }
        setup.setOnlyShowVerificationWarningsAndErrors(checked);
        const CStatusMessage msg = m_matchingSettings.setAndSave(setup);
        Q_UNUSED(msg)
    }

    void CAircraftModelValidationComponent::triggerValidation()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->supportsContexts()) { return; }
        if (!sGui->getIContextSimulator()) { return; }
        if (sGui->getIContextSimulator()->isValidationInProgress())
        {
            this->showOverlayHTMLMessage("Validation in progress", 5000);
            return;
        }

        const CSimulatorInfo simulator = ui->comp_Simulator->getValue();
        if (sGui->getIContextSimulator()->triggerModelSetValidation(simulator))
        {
            this->showOverlayHTMLMessage(QStringLiteral("Triggered validation for '%1'").arg(simulator.toQString(true)), 5000);
        }
        else
        {
            this->showOverlayHTMLMessage(QStringLiteral("Cannot trigger validation for '%1'").arg(simulator.toQString(true)), 5000);
        }
    }

    void CAircraftModelValidationComponent::requestLastResults()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->supportsContexts()) { return; }
        if (!sGui->getIContextSimulator()) { return; }
        if (sGui->getIContextSimulator()->isValidationInProgress())
        {
            this->showOverlayHTMLMessage("Validation in progress", 5000);
            return;
        }
        sGui->getIContextSimulator()->triggerModelSetValidation(CSimulatorInfo());
    }

    void CAircraftModelValidationComponent::onTempDisabledButtonClicked()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        CAircraftModelList disableModels;
        const QObject *sender = QObject::sender();
        if (sender == ui->pb_TempDisableInvalid) { disableModels = ui->tvp_InvalidModels->container(); }
        else if (sender == ui->pb_TempDisableSelected) { disableModels = ui->tvp_InvalidModels->selectedObjects(); }

        if (disableModels.isEmpty())
        {
            this->showOverlayHTMLMessage("No models disabled", 4000);
        }
        else
        {
            this->tempDisableModels(disableModels);
            this->showOverlayHTMLMessage(QStringLiteral("%1 models disabled").arg(disableModels.size()));
        }
    }

    void CAircraftModelValidationComponent::onRemoveButtonClicked()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextSimulator()) { return; }

        CAircraftModelList removeModels;
        const QObject *sender = QObject::sender();
        if (sender == ui->pb_RemoveInvalid) { removeModels = ui->tvp_InvalidModels->container(); }
        else if (sender == ui->pb_RemoveSelected) { removeModels = ui->tvp_InvalidModels->selectedObjects(); }

        if (removeModels.isEmpty())
        {
            this->showOverlayHTMLMessage("No models removed", 4000);
        }
        else
        {
            const QMessageBox::StandardButton ret = QMessageBox::question(this,
                                                                          tr("Model validation"),
                                                                          tr("Do you really want to delete %1 models from model set?").arg(removeModels.sizeInt()),
                                                                          QMessageBox::Ok | QMessageBox::Cancel);
            if (ret != QMessageBox::Ok) { return; }

            const int r = sGui->getIContextSimulator()->removeModelsFromSet(removeModels);
            this->showOverlayHTMLMessage(QStringLiteral("%1 models removed").arg(r));
        }
    }

    void CAircraftModelValidationComponent::showHelp()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        sGui->showHelp(this);
    }

    void CAircraftModelValidationComponent::saveInvalidModels(const CAircraftModelList &models) const
    {
        const CStatusMessage m = models.saveInvalidModels();
        Q_UNUSED(m)
    }
} // ns
