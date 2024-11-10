// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "cockpitcomform.h"
#include "ui_cockpitcomform.h"
#include "gui/guiapplication.h"
#include "core/context/contextsimulator.h"
#include "misc/math/mathutils.h"
#include "misc/logmessage.h"
#include "misc/icons.h"

#include <QPushButton>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::audio;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::math;
using namespace swift::misc::simulation;
using namespace swift::core::context;
using namespace swift::gui::components;

namespace swift::gui::editors
{
    CCockpitComForm::CCockpitComForm(QWidget *parent) : CForm(parent),
                                                        ui(new Ui::CCockpitComForm)
    {
        ui->setupUi(this);
        this->alignUiElementsHeight();

        // SELCAL pairs in cockpit
        ui->frp_ComPanelSelcalSelector->clear();
        connect(ui->tb_ComPanelSelcalTest, &QPushButton::clicked, this, &CCockpitComForm::testSelcal);
        connect(ui->frp_ComPanelSelcalSelector, &CSelcalCodeSelector::valueChanged, this, &CCockpitComForm::onSelcalChanged);

        // XPDR
        connect(ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderModeChanged, this, &CCockpitComForm::transponderModeChanged);
        connect(ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderStateIdentEnded, this, &CCockpitComForm::transponderStateIdentEnded);

        // COM GUI events
        connect(ui->tb_ComPanelCom1Toggle, &QPushButton::clicked, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->tb_ComPanelCom2Toggle, &QPushButton::clicked, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->ds_ComPanelCom1Active, &QDoubleSpinBox::editingFinished, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->ds_ComPanelCom2Active, &QDoubleSpinBox::editingFinished, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->ds_ComPanelCom1Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->ds_ComPanelCom2Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->sbp_ComPanelTransponder, &QDoubleSpinBox::editingFinished, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderModeChanged, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->frp_ComPanelSelcalSelector, &CSelcalCodeSelector::valueChanged, this, &CCockpitComForm::onGuiChangedCockpitValues);
        connect(ui->tb_RequestTextMessageCom1, &QToolButton::released, this, &CCockpitComForm::requestCom1TextMessage);
        connect(ui->tb_RequestTextMessageCom2, &QToolButton::released, this, &CCockpitComForm::requestCom2TextMessage);

        ui->tb_RequestTextMessageCom1->setIcon(CIcons::appTextMessages16());
        ui->tb_RequestTextMessageCom2->setIcon(CIcons::appTextMessages16());

        this->initLeds();
    }

    CCockpitComForm::~CCockpitComForm()
    {}

    void CCockpitComForm::setReadOnly(bool readonly)
    {
        ui->ds_ComPanelCom1Active->setReadOnly(readonly);
        ui->ds_ComPanelCom2Active->setReadOnly(readonly);
        ui->ds_ComPanelCom1Standby->setReadOnly(readonly);
        ui->ds_ComPanelCom2Standby->setReadOnly(readonly);
        ui->cbp_ComPanelTransponderMode->setDisabled(!readonly);
        ui->tb_ComPanelCom1Toggle->setEnabled(!readonly);
        ui->tb_ComPanelCom2Toggle->setEnabled(!readonly);
        this->forceStyleSheetUpdate();
    }

    void CCockpitComForm::setSelectOnly()
    {
        this->setReadOnly(true);
    }

    CStatusMessageList CCockpitComForm::validate(bool nested) const
    {
        Q_UNUSED(nested)
        CStatusMessageList msgs;
        return msgs;
    }

    void CCockpitComForm::setTransponderModeStateIdent()
    {
        ui->cbp_ComPanelTransponderMode->setSelectedTransponderModeStateIdent();
    }

    void CCockpitComForm::setValue(const CSimulatedAircraft &aircraft)
    {
        this->setFrequencies(aircraft.getCom1System(), aircraft.getCom2System());
        this->setSelcal(aircraft.getSelcal());
        this->setTransponder(aircraft.getTransponder());
        this->alignUiElementsHeight();
        this->updateIntegratedFlagFromSimulatorContext();
    }

    CSelcal CCockpitComForm::getSelcal() const
    {
        return ui->frp_ComPanelSelcalSelector->getSelcal();
    }

    void CCockpitComForm::setSelcal(const CSelcal &selcal)
    {
        ui->frp_ComPanelSelcalSelector->setSelcal(selcal);
    }

    void CCockpitComForm::initLeds()
    {
        const CLedWidget::LedShape shape = CLedWidget::Rounded;

        ui->led_ComPanelCom1R->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM1 receive (sim)", "COM1 not receiving", 14);
        ui->led_ComPanelCom1T->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM1 transmit (sim)", "COM1 not transmitting", 14);
        ui->led_ComPanelCom1R->setTriStateValues(CLedWidget::Blue, "receive not synced");
        ui->led_ComPanelCom1T->setTriStateValues(CLedWidget::Blue, "transmit not synced");

        ui->led_ComPanelCom2R->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM2 receive (sim)", "COM2 not receiving", 14);
        ui->led_ComPanelCom2T->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM2 transmit (sim)", "COM2 not transmitting", 14);
        ui->led_ComPanelCom2R->setTriStateValues(CLedWidget::Blue, "receive not synced");
        ui->led_ComPanelCom2T->setTriStateValues(CLedWidget::Blue, "transmit not synced");
    }

    CSimulatedAircraft CCockpitComForm::cockpitValuesToAircraftObject()
    {
        CSimulatedAircraft comAircraft;
        CTransponder transponder = comAircraft.getTransponder();
        CComSystem com1 = comAircraft.getCom1System();
        CComSystem com2 = comAircraft.getCom2System();

        //
        // Transponder
        //
        const QString transponderCode = QString::number(ui->sbp_ComPanelTransponder->value());
        if (CTransponder::isValidTransponderCode(transponderCode))
        {
            transponder.setTransponderCode(transponderCode);
        }
        else
        {
            CLogMessage(this).validationWarning(u"Wrong transponder code, reset");
            ui->sbp_ComPanelTransponder->setValue(transponder.getTransponderCode());
        }
        transponder.setTransponderMode(ui->cbp_ComPanelTransponderMode->getSelectedTransponderMode());

        //
        // COM units
        //
        com1.setFrequencyActiveMHz(ui->ds_ComPanelCom1Active->value());
        com1.setFrequencyStandbyMHz(ui->ds_ComPanelCom1Standby->value());
        com2.setFrequencyActiveMHz(ui->ds_ComPanelCom2Active->value());
        com2.setFrequencyStandbyMHz(ui->ds_ComPanelCom2Standby->value());
        this->setFrequencies(com1, com2); // back annotation after rounding

        comAircraft.setCom1System(com1);
        comAircraft.setCom2System(com2);
        comAircraft.setTransponder(transponder);
        return comAircraft;
    }

    void CCockpitComForm::setFrequencies(const CComSystem &com1, const CComSystem &com2)
    {
        double freq = com1.getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3);
        if (!isFrequenceEqual(freq, ui->ds_ComPanelCom1Active->value()))
        {
            ui->ds_ComPanelCom1Active->setValue(freq);
        }

        freq = com2.getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3);
        if (!isFrequenceEqual(freq, ui->ds_ComPanelCom2Active->value()))
        {
            ui->ds_ComPanelCom2Active->setValue(freq);
        }

        freq = com1.getFrequencyStandby().valueRounded(CFrequencyUnit::MHz(), 3);
        if (!isFrequenceEqual(freq, ui->ds_ComPanelCom1Standby->value()))
        {
            ui->ds_ComPanelCom1Standby->setValue(freq);
        }

        freq = com2.getFrequencyStandby().valueRounded(CFrequencyUnit::MHz(), 3);
        if (!isFrequenceEqual(freq, ui->ds_ComPanelCom2Standby->value()))
        {
            ui->ds_ComPanelCom2Standby->setValue(freq);
        }

        this->updateIntegratedFlagFromSimulatorContext();
        this->updateActiveCOMUnitLEDs(m_integratedWithSim, com1.isTransmitEnabled(), com1.isReceiveEnabled(), com2.isTransmitEnabled(), com2.isReceiveEnabled());
    }

    void CCockpitComForm::setTransponder(const CTransponder &transponder)
    {
        const int tc = transponder.getTransponderCode();
        if (tc != ui->sbp_ComPanelTransponder->value())
        {
            ui->sbp_ComPanelTransponder->setValue(tc);
        }

        if (transponder.getTransponderMode() != ui->cbp_ComPanelTransponderMode->getSelectedTransponderMode())
        {
            ui->cbp_ComPanelTransponderMode->setSelectedTransponderMode(transponder.getTransponderMode());
            ui->comp_TransponderLeds->setMode(transponder.getTransponderMode());
        }
    }

    void CCockpitComForm::onGuiChangedCockpitValues()
    {
        const QObject *sender = QObject::sender();
        if (sender == ui->tb_ComPanelCom1Toggle)
        {
            if (isFrequenceEqual(ui->ds_ComPanelCom1Standby->value(), ui->ds_ComPanelCom1Active->value())) { return; }
            const double f = ui->ds_ComPanelCom1Active->value();
            ui->ds_ComPanelCom1Active->setValue(ui->ds_ComPanelCom1Standby->value());
            ui->ds_ComPanelCom1Standby->setValue(f);
        }
        else if (sender == ui->tb_ComPanelCom2Toggle)
        {
            if (isFrequenceEqual(ui->ds_ComPanelCom2Standby->value(), ui->ds_ComPanelCom2Active->value())) { return; }
            const double f = ui->ds_ComPanelCom2Active->value();
            ui->ds_ComPanelCom2Active->setValue(ui->ds_ComPanelCom2Standby->value());
            ui->ds_ComPanelCom2Standby->setValue(f);
        }

        this->alignUiElementsHeight();
        const CSimulatedAircraft aircraft = this->cockpitValuesToAircraftObject();
        emit this->changedCockpitValues(aircraft);
    }

    void CCockpitComForm::onSelcalChanged()
    {
        const CSelcal selcal = ui->frp_ComPanelSelcalSelector->getSelcal();
        emit this->changedSelcal(selcal);
    }

    void CCockpitComForm::alignUiElementsHeight()
    {
        const int selcalH = ui->frp_ComPanelSelcalSelector->getComboBoxHeight();
        if (selcalH > ui->ds_ComPanelCom1Standby->height())
        {
            ui->ds_ComPanelCom1Standby->setMinimumHeight(selcalH);
            ui->ds_ComPanelCom2Standby->setMinimumHeight(selcalH);
        }

        const int xpdrH = ui->cbp_ComPanelTransponderMode->height();
        if (xpdrH > ui->ds_ComPanelCom1Active->height())
        {
            ui->ds_ComPanelCom1Active->setMinimumHeight(xpdrH);
            ui->ds_ComPanelCom2Active->setMinimumHeight(xpdrH);
            ui->sbp_ComPanelTransponder->setMinimumHeight(xpdrH);
        }
    }

    void CCockpitComForm::updateActiveCOMUnitLEDs(bool integratedWithSim, bool com1T, bool com1R, bool com2T, bool com2R)
    {
        if (!integratedWithSim)
        {
            ui->led_ComPanelCom1R->setTriState();
            ui->led_ComPanelCom1T->setTriState();
            ui->led_ComPanelCom2R->setTriState();
            ui->led_ComPanelCom2T->setTriState();
        }
        else
        {
            ui->led_ComPanelCom1R->setOn(com1R);
            ui->led_ComPanelCom1T->setOn(com1T);
            ui->led_ComPanelCom2R->setOn(com2R);
            ui->led_ComPanelCom2T->setOn(com2T);
        }
    }

    void CCockpitComForm::updateIntegratedFlagFromSimulatorContext()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator())
        {
            m_integratedWithSim = false;
            return;
        }
        const bool integrated = sGui->getIContextSimulator()->getSimulatorSettings().isComIntegrated();
        m_integratedWithSim = integrated;
    }

    bool CCockpitComForm::isFrequenceEqual(double f1, double f2)
    {
        return CMathUtils::epsilonEqual(f1, f2);
    }
} // ns
