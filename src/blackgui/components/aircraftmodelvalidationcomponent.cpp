/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmodelvalidationcomponent.h"
#include "ui_aircraftmodelvalidationcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"

#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CAircraftModelValidationComponent::CAircraftModelValidationComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CAircraftModelValidationComponent)
        {
            ui->setupUi(this);
            ui->comp_Simulator->setMode(CSimulatorSelector::ComboBox);
            ui->comp_Simulator->setRememberSelection(false);
            ui->comp_Messages->setNoSorting(); // keep order
            ui->tvp_InvalidModels->setValidationContextMenu(false);

            const CAircraftMatcherSetup setup = m_matchingSettings.get();
            ui->cb_EnableStartupCheck->setChecked(setup.doVerificationAtStartup());
            connect(ui->cb_EnableStartupCheck,  &QCheckBox::toggled,    this, &CAircraftModelValidationComponent::onCheckAtStartupChanged);
            connect(ui->pb_TempDisableInvalid,  &QPushButton::released, this, &CAircraftModelValidationComponent::onButtonClicked);
            connect(ui->pb_TempDisableSelected, &QPushButton::released, this, &CAircraftModelValidationComponent::onButtonClicked);
            connect(ui->pb_TriggerValidation,   &QPushButton::released, this, &CAircraftModelValidationComponent::triggerValidation);

            // 1st init when running in distributed environment
            QPointer<CAircraftModelValidationComponent> myself(this);

            const qint64 lastResults = m_lastResults;
            QTimer::singleShot(2500, this, [ = ]
            {
                if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                if (m_lastResults > lastResults) { return; } // values received in meantime
                myself->requestLastResults();
            });
        }

        CAircraftModelValidationComponent::~CAircraftModelValidationComponent()
        { }

        void CAircraftModelValidationComponent::validatedModelSet(const CSimulatorInfo &simulator, const CAircraftModelList &valid, const CAircraftModelList &invalid, bool stopped, const CStatusMessageList &msgs)
        {
            Q_UNUSED(simulator);
            Q_UNUSED(valid);
            m_lastResults = QDateTime::currentMSecsSinceEpoch();
            ui->tvp_InvalidModels->updateContainerMaybeAsync(invalid);
            ui->comp_Simulator->setValue(simulator);
            ui->comp_Messages->clear();

            if (!msgs.isEmpty())
            {
                ui->comp_Messages->appendStatusMessagesToList(msgs.isSortedLatestLast() ? CStatusMessageList(msgs.reversed()) : msgs);
            }

            const QString msg = stopped ?
                                QStringLiteral("Validation for '%1' stopped, maybe your models are not accessible").arg(simulator.toQString(true)) :
                                QStringLiteral("Validated for '%1'. Valid: %2 Invalid: %3").arg(simulator.toQString(true)).arg(valid.size()).arg(invalid.size());
            ui->lbl_Summay->setText(msg);
            if (stopped) { this->showOverlayHTMLMessage(msg, 5000); }

            const CAircraftMatcherSetup setup = m_matchingSettings.get();
            ui->cb_EnableStartupCheck->setChecked(setup.doVerificationAtStartup());
            ui->pb_TempDisableInvalid->setEnabled(!invalid.isEmpty());
            ui->pb_TempDisableSelected->setEnabled(!invalid.isEmpty());
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
            Q_UNUSED(msg);
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

        void CAircraftModelValidationComponent::onButtonClicked()
        {
            const QObject *sender = QObject::sender();
            CAircraftModelList disabledModels;
            if (sender == ui->pb_TempDisableInvalid)       { disabledModels = ui->tvp_InvalidModels->container(); }
            else if (sender == ui->pb_TempDisableSelected) { disabledModels = ui->tvp_InvalidModels->selectedObjects(); }

            this->tempDisableModels(disabledModels);

            if (disabledModels.isEmpty())
            {
                this->showOverlayHTMLMessage("No models disabled");
            }
            else
            {
                this->showOverlayHTMLMessage(QStringLiteral("%1 models disabled").arg(disabledModels.size()));
            }
        }

        void CAircraftModelValidationComponent::saveInvalidModels(const CAircraftModelList &models) const
        {
            const CStatusMessage m = models.saveInvalidModels();
            Q_UNUSED(m);
        }
    } // ns
} // ns
