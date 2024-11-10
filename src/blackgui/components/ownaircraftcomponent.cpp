// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ownaircraftcomponent.h"
#include "ui_ownaircraftcomponent.h"

#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/webdataservices.h"
#include "blackcore/simulator.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/crashhandler.h"
#include "misc/logmessage.h"
#include "config/buildconfig.h"

#include <QPushButton>

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::network;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    COwnAircraftComponent::COwnAircraftComponent(QWidget *parent) : QFrame(parent),
                                                                    ui(new Ui::COwnAircraftComponent)
    {
        ui->setupUi(this);
        ui->selector_AircraftIcao->displayWithIcaoDescription(false);
        ui->selector_AirlineIcao->displayWithIcaoDescription(false);
        ui->selector_AircraftIcao->displayMode(CDbAircraftIcaoSelectorComponent::DisplayIcaoAndId);
        ui->selector_AirlineIcao->displayMode(CDbAirlineIcaoSelectorComponent::DisplayVDesignatorAndId);

        ui->cb_Model->setChecked(true);
        ui->cb_Livery->setChecked(true);

        // own aircraft
        ui->lblp_AircraftCombinedType->setToolTips("ok", "wrong");
        ui->lblp_AirlineIcao->setToolTips("ok", "wrong");
        ui->lblp_AircraftIcao->setToolTips("ok", "wrong");
        ui->lblp_Callsign->setToolTips("ok", "wrong");

        constexpr int MaxLength = 10;
        constexpr int MinLength = 0;
        CUpperCaseValidator *ucv = new CUpperCaseValidator(MinLength, MaxLength, ui->le_Callsign);
        ucv->setAllowedCharacters09AZ();
        ui->le_Callsign->setMaxLength(MaxLength);
        ui->le_Callsign->setValidator(ucv);

        ui->le_AircraftCombinedType->setMaxLength(3);
        ui->le_AircraftCombinedType->setValidator(new CUpperCaseValidator(this));
        ui->comp_ModelStringCompleter->setSourceVisible(CAircraftModelStringCompleter::OwnModels, false);
        ui->comp_ModelStringCompleter->selectSource(CAircraftModelStringCompleter::ModelSet);

        connect(ui->le_Callsign, &QLineEdit::editingFinished, this, &COwnAircraftComponent::validate);
        connect(ui->comp_ModelStringCompleter, &CAircraftModelStringCompleter::modelStringChanged, this, &COwnAircraftComponent::onModelStringSendChanged);
        connect(ui->le_AircraftCombinedType, &QLineEdit::editingFinished, this, &COwnAircraftComponent::validate);
        connect(ui->selector_AircraftIcao, &CDbAircraftIcaoSelectorComponent::changedAircraftIcao, this, &COwnAircraftComponent::changedAircraftIcao, Qt::QueuedConnection);
        connect(ui->selector_AirlineIcao, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &COwnAircraftComponent::changedAirlineIcao, Qt::QueuedConnection);
        connect(ui->pb_Clear, &QPushButton::clicked, this, &COwnAircraftComponent::clearLivery, Qt::QueuedConnection);

        if (sGui && sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::ownAircraftModelChanged, this, &COwnAircraftComponent::onSimulatorModelChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &COwnAircraftComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
        }

        if (sGui && sGui->getIContextOwnAircraft())
        {
            this->onSimulatorModelChanged(sGui->getIContextOwnAircraft()->getOwnAircraft().getModel());
        }
    }

    COwnAircraftComponent::~COwnAircraftComponent()
    {}

    void COwnAircraftComponent::setUser(const CUser &user)
    {
        if (user.hasCallsign())
        {
            ui->le_Callsign->setText(user.getCallsign().asString());
        }
        else if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            ui->le_Callsign->setText("SWIFT");
        }
        this->validate();
    }

    void COwnAircraftComponent::onSimulatorModelChanged(const CAircraftModel &model)
    {
        if (!sGui || !sGui->getIContextNetwork() || sApp->isShuttingDown()) { return; }
        const bool isNetworkConnected = sGui && sGui->getIContextNetwork()->isConnected();
        if (isNetworkConnected) { return; }

        // update with latest DB data
        CAircraftModel reverseModel(model);
        if (sGui->hasWebDataServices())
        {
            reverseModel = sGui->getWebDataServices()->getModelForModelString(model.getModelString());
            if (!reverseModel.isLoadedFromDb()) { reverseModel = model; } // reset if not found
        }

        const QString modelStr(reverseModel.hasModelString() ? reverseModel.getModelString() : "<unknown>");
        if (!reverseModel.hasModelString())
        {
            CLogMessage(this).validationInfo(u"Invalid lookup for '%1' successful: %2") << modelStr << reverseModel.toQString();
            CLogMessage(this).validationInfo(u"Hint: Are you using the emulated driver? Set a model if so!");
            return;
        }
        this->setOwnModelAndIcaoValues(reverseModel);

        emit this->aircraftDataChanged();
    }

    void COwnAircraftComponent::onModelStringSendChanged()
    {
        if (!this->hasValidContexts()) { return; }
        if (!sGui->hasWebDataServices()) { return; }
        const QString modelString = ui->comp_ModelStringCompleter->getModelString();
        if (modelString.isEmpty()) { return; } // keep last values
        const CAircraftModel model = sGui->getWebDataServices()->getModelForModelString(modelString);
        this->setGuiIcaoValues(model, false);
    }

    void COwnAircraftComponent::onSimulatorStatusChanged(int status)
    {
        ISimulator::SimulatorStatus s = static_cast<ISimulator::SimulatorStatus>(status);
        Q_UNUSED(s);
        if (!this->hasValidContexts()) { return; }
        if (sGui->getIContextNetwork()->isConnected())
        {
            // void
        }
    }

    CStatusMessageList COwnAircraftComponent::validate() const
    {
        CGuiAircraftValues values = this->getAircraftValuesFromGui();
        CStatusMessageList msgs;

        // fill in combined type if empty
        if (ui->le_AircraftCombinedType->text().isEmpty() && values.ownAircraftIcao.isLoadedFromDb())
        {
            ui->le_AircraftCombinedType->setText(values.ownAircraftIcao.getCombinedType());
            values.ownAircraftCombinedType = values.ownAircraftIcao.getCombinedType();
        }

        const bool validCombinedType = CAircraftIcaoCode::isValidCombinedType(values.ownAircraftCombinedType);
        ui->lblp_AircraftCombinedType->setTicked(validCombinedType);
        if (!validCombinedType) { msgs.addValidationMessage("Invalid combined type", CStatusMessage::SeverityError); }

        // airline is optional, e.g. C172 has no airline
        const bool validAirlineDesignator = values.ownAirlineIcao.hasValidDesignator() || values.ownAirlineIcao.getDesignator().isEmpty();
        ui->lblp_AirlineIcao->setTicked(validAirlineDesignator);
        if (!validAirlineDesignator) { msgs.addValidationMessage("Invalid airline designator", CStatusMessage::SeverityError); }

        const bool validAircraftDesignator = values.ownAircraftIcao.hasValidDesignator();
        ui->lblp_AircraftIcao->setTicked(validAircraftDesignator);
        if (!validAircraftDesignator) { msgs.addValidationMessage("Invalid aircraft designator", CStatusMessage::SeverityError); }

        const bool validCallsign = CCallsign::isValidAircraftCallsign(values.ownCallsign);
        ui->lblp_Callsign->setTicked(validCallsign);
        if (!validCallsign) { msgs.addValidationMessage("Invalid callsign", CStatusMessage::SeverityError); }

        // model intentionally ignored
        // return validCombinedType && validAirlineDesignator && validAircraftDesignator && validCallsign;

        return msgs;
    }

    void COwnAircraftComponent::changedAircraftIcao(const CAircraftIcaoCode &icao)
    {
        if (icao.isLoadedFromDb())
        {
            ui->le_AircraftCombinedType->setText(icao.getCombinedType());
        }
        this->validate();
    }

    COwnAircraftComponent::CGuiAircraftValues COwnAircraftComponent::getAircraftValuesFromGui() const
    {
        CGuiAircraftValues values;
        values.ownCallsign = CCallsign(ui->le_Callsign->text().trimmed().toUpper());
        values.ownAircraftIcao = ui->selector_AircraftIcao->getAircraftIcao();
        values.ownAirlineIcao = ui->selector_AirlineIcao->getAirlineIcao();
        values.ownAircraftCombinedType = ui->le_AircraftCombinedType->text().trimmed().toUpper();
        values.ownAircraftSimulatorModelString = ui->le_SimulatorModel->text().trimmed().toUpper();
        values.ownAircraftModelStringSend = ui->comp_ModelStringCompleter->getModelString().toUpper();
        values.ownLiverySend = ui->le_SendLivery->text().trimmed().toUpper();
        values.useLivery = ui->cb_Livery->isChecked();
        values.useModelString = ui->cb_Model->isChecked();
        return values;
    }

    CCallsign COwnAircraftComponent::getCallsignFromGui() const
    {
        const CCallsign cs(ui->le_Callsign->text().trimmed().toUpper());
        return cs;
    }

    void COwnAircraftComponent::changedAirlineIcao(const CAirlineIcaoCode &icao)
    {
        Q_UNUSED(icao);
        this->validate();
    }

    bool COwnAircraftComponent::setGuiIcaoValues(const CAircraftModel &model, bool onlyIfEmpty)
    {
        bool changed = false;
        if (!onlyIfEmpty || !ui->selector_AircraftIcao->isSet())
        {
            changed = ui->selector_AircraftIcao->setAircraftIcao(model.getAircraftIcaoCode());
        }
        if (!onlyIfEmpty || !ui->selector_AirlineIcao->isSet())
        {
            const bool c = ui->selector_AirlineIcao->setAirlineIcao(model.getAirlineIcaoCode());
            changed |= c;
        }
        if (!onlyIfEmpty || ui->le_AircraftCombinedType->text().trimmed().isEmpty())
        {
            const QString combined(model.getAircraftIcaoCode().getCombinedType());
            if (ui->le_AircraftCombinedType->text() != combined)
            {
                ui->le_AircraftCombinedType->setText(combined);
                changed = true;
            }
        }
        if (!onlyIfEmpty || ui->le_SendLivery->text().trimmed().isEmpty())
        {
            const QString ls(model.getSwiftLiveryString());
            if (ui->le_SendLivery->text() != ls)
            {
                ui->le_SendLivery->setText(ls);
                changed = true;
            }
        }

        const bool valid = this->validate().isSuccess();
        return valid ? changed : false;
    }

    void COwnAircraftComponent::highlightModelField(const CAircraftModel &model)
    {
        if (!model.hasModelString()) { ui->le_SimulatorModel->setProperty("validation", "error"); }
        else if (!model.isLoadedFromDb()) { ui->le_SimulatorModel->setProperty("validation", "warning"); }
        else { ui->le_SimulatorModel->setProperty("validation", "ok"); }
        ui->le_SimulatorModel->setStyleSheet(""); // force update
    }

    bool COwnAircraftComponent::hasValidContexts() const
    {
        if (!sGui || !sGui->supportsContexts()) { return false; }
        if (sGui->isShuttingDown()) { return false; }
        if (!sGui->getIContextSimulator()) { return false; }
        if (!sGui->getIContextNetwork()) { return false; }
        if (!sGui->getIContextOwnAircraft()) { return false; }
        return true;
    }

    void COwnAircraftComponent::clearLivery()
    {
        ui->le_SendLivery->clear();
    }

    CAircraftModel COwnAircraftComponent::getPrefillModel() const
    {
        const CAircraftModel model = m_lastAircraftModel.get();
        if (model.hasAircraftDesignator()) { return model; }
        return IContextOwnAircraft::getDefaultOwnAircraftModel();
    }

    void COwnAircraftComponent::setOwnModelAndIcaoValues(const CAircraftModel &ownModel)
    {
        if (!this->hasValidContexts()) { return; }
        CAircraftModel model = ownModel;
        const bool simulating = sGui->getIContextSimulator() &&
                                (sGui->getIContextSimulator()->getSimulatorStatus() & ISimulator::Simulating);
        if (simulating)
        {
            if (!model.hasModelString())
            {
                model = sGui->getIContextOwnAircraft()->getOwnAircraft().getModel();
            }
            const QString modelAndKey(model.getModelStringAndDbKey());

            ui->le_SimulatorModel->setText(modelAndKey);
            ui->le_SimulatorModel->home(false);
            this->highlightModelField(model);

            const CSimulatorInfo sim = sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator();
            ui->comp_ModelStringCompleter->setSimulator(sim);
            m_lastAircraftModel.set(model);

            const CSimulatorInternals simulatorInternals = sGui->getIContextSimulator()->getSimulatorInternals();
            const QString simStr = sim.toQString() + QStringLiteral(" ") + simulatorInternals.getSimulatorVersion();
            CCrashHandler::instance()->crashAndLogInfoSimulator(simStr);
        }
        else
        {
            if (!model.hasModelString())
            {
                model = this->getPrefillModel();
            }
            ui->le_SimulatorModel->clear();
            this->highlightModelField();
        }
        ui->le_SimulatorModel->setToolTip(model.asHtmlSummary());
        ui->comp_ModelStringCompleter->setText(model.getModelString());
        ui->le_SendLivery->setText(model.getSwiftLiveryString());

        // reset the model
        if (model.isLoadedFromDb() || (model.getAircraftIcaoCode().isLoadedFromDb() && model.getLivery().isLoadedFromDb()))
        {
            // full model from DB, take all values
            this->setGuiIcaoValues(model, false);
        }
        else
        {
            if (sGui->getIContextSimulator()->isSimulatorAvailable())
            {
                // sim. attached, but no model data from DB
                ui->le_AircraftCombinedType->clear();
                ui->selector_AircraftIcao->clear();
                ui->selector_AirlineIcao->clear();
            }
        }
    }

    bool COwnAircraftComponent::updateOwnAircaftIcaoValuesFromGuiValues()
    {
        if (!this->hasValidContexts()) { return false; }
        const CSimulatedAircraft ownAircraft(sGui->getIContextOwnAircraft()->getOwnAircraft());
        const CGuiAircraftValues aircraftValues = this->getAircraftValuesFromGui();

        CAircraftIcaoCode aircraftCode(ownAircraft.getAircraftIcaoCode());
        CAirlineIcaoCode airlineCode(ownAircraft.getAirlineIcaoCode());

        bool changedIcaoCodes = false;
        if (aircraftValues.ownAircraftIcao.hasValidDesignator() && aircraftValues.ownAircraftIcao != aircraftCode)
        {
            aircraftCode = aircraftValues.ownAircraftIcao;
            changedIcaoCodes = true;
        }
        if (aircraftValues.ownAirlineIcao.hasValidDesignator() && aircraftValues.ownAirlineIcao != airlineCode)
        {
            airlineCode = aircraftValues.ownAirlineIcao;
            changedIcaoCodes = true;
        }

        if (changedIcaoCodes)
        {
            sGui->getIContextOwnAircraft()->updateOwnIcaoCodes(aircraftCode, airlineCode);
        }

        return changedIcaoCodes;
    }

} // ns
