/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackgui/components/cockpitcomcomponent.h"
#include "blackgui/components/selcalcodeselector.h"
#include "blackgui/components/transpondercodespinbox.h"
#include "blackgui/components/transpondermodeselector.h"
#include "blackgui/guiapplication.h"
#include "blackgui/led.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/sequence.h"
#include "blackmisc/verify.h"
#include "ui_cockpitcomcomponent.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QtGlobal>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {
        CCockpitComComponent::CCockpitComComponent(QWidget *parent) :
            QFrame(parent),
            BlackMisc::CIdentifiable(this),
            ui(new Ui::CCockpitMainComponent)
        {
            ui->setupUi(this);
            this->initLeds();
            connect(ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderModeChanged, this, &CCockpitComComponent::transponderModeChanged);
            connect(ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderStateIdentEnded, this, &CCockpitComComponent::transponderStateIdentEnded);

            // init from aircraft
            CSimulatedAircraft ownAircraft = this->getOwnAircraft();
            this->ps_updateCockpitFromContext(ownAircraft, CIdentifier("dummyInitialValues")); // intentionally different name here

            // SELCAL pairs in cockpit
            ui->frp_ComPanelSelcalBottom->clear();
            connect(ui->tb_ComPanelSelcalTest, &QPushButton::clicked, this, &CCockpitComComponent::ps_testSelcal);

            // COM GUI events
            connect(ui->tb_ComPanelCom1Toggle, &QPushButton::clicked, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->tb_ComPanelCom2Toggle, &QPushButton::clicked, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->ds_ComPanelCom1Active, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->ds_ComPanelCom2Active, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->ds_ComPanelCom1Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->ds_ComPanelCom2Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->sbp_ComPanelTransponder, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderModeChanged, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(ui->frp_ComPanelSelcalBottom, &CSelcalCodeSelector::valueChanged, this, &CCockpitComComponent::ps_guiChangedSelcal);

            // hook up with changes from own aircraft context
            this->connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CCockpitComComponent::ps_updateCockpitFromContext);
            this->connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedSelcal, this, &CCockpitComComponent::ps_onChangedSelcal);

            // hook up with audio context
            this->connect(sGui->getIContextAudio(), &IContextAudio::changedVoiceRooms, this, &CCockpitComComponent::ps_onChangedVoiceRoomStatus);
        }

        CCockpitComComponent::~CCockpitComComponent()
        { }

        void CCockpitComComponent::setSelectedTransponderModeStateIdent()
        {
            ui->cbp_ComPanelTransponderMode->setSelectedTransponderModeStateIdent();
        }

        void CCockpitComComponent::paintEvent(QPaintEvent *event)
        {
            Q_UNUSED(event);
            CStyleSheetUtility::useStyleSheetInDerivedWidget(this);
        }

        void CCockpitComComponent::ps_guiChangedCockpitValues()
        {
            QObject *sender = QObject::sender();
            if (sender == ui->tb_ComPanelCom1Toggle)
            {
                if (ui->ds_ComPanelCom1Standby->value() == ui->ds_ComPanelCom1Active->value()) return;
                double f = ui->ds_ComPanelCom1Active->value();
                ui->ds_ComPanelCom1Active->setValue(ui->ds_ComPanelCom1Standby->value());
                ui->ds_ComPanelCom1Standby->setValue(f);
            }
            else if (sender == ui->tb_ComPanelCom2Toggle)
            {
                if (ui->ds_ComPanelCom2Standby->value() == ui->ds_ComPanelCom2Active->value()) return;
                double f = ui->ds_ComPanelCom2Active->value();
                ui->ds_ComPanelCom2Active->setValue(ui->ds_ComPanelCom2Standby->value());
                ui->ds_ComPanelCom2Standby->setValue(f);
            }

            const CSimulatedAircraft ownAircraft = this->cockpitValuesToAircraftObject();
            this->updateOwnCockpitInContext(ownAircraft);
        }

        void CCockpitComComponent::ps_guiChangedSelcal()
        {
            sGui->getIContextOwnAircraft()->updateSelcal(this->getSelcal(), identifier());
        }

        void CCockpitComComponent::ps_updateCockpitFromContext(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
        {
            if (isMyIdentifier(originator)) { return; } // comes from myself

            // update GUI elements
            // avoid unnecessary change events as far as possible
            const CComSystem com1 = ownAircraft.getCom1System(); // aircraft just updated or set from context
            const CComSystem com2 = ownAircraft.getCom2System();
            const CTransponder transponder = ownAircraft.getTransponder();

            // update the frequencies
            this->updateFrequencyDisplaysFromComSystems(com1, com2);

            // update transponder
            int tc = transponder.getTransponderCode();
            if (tc != ui->sbp_ComPanelTransponder->value())
            {
                ui->sbp_ComPanelTransponder->setValue(tc);
            }

            ui->cbp_ComPanelTransponderMode->setSelectedTransponderMode(transponder.getTransponderMode());

            if (sGui->getIContextNetwork())
            {
                CAtcStationList selectedStations = sGui->getIContextNetwork()->getSelectedAtcStations();
                CAtcStation com1Station = selectedStations.size() > 0 ? selectedStations[0] : CAtcStation();
                CAtcStation com2Station = selectedStations.size() > 1 ? selectedStations[1] : CAtcStation();
                if (com1Station.getCallsign().isEmpty())
                {
                    ui->lbl_ComPanelCom1Active->setToolTip("");
                    ui->led_ComPanelCom1->setOn(false);
                }
                else
                {
                    ui->lbl_ComPanelCom1Active->setToolTip(com1Station.getCallsign().getStringAsSet());
                    ui->led_ComPanelCom1->setOn(true);

                }
                if (com2Station.getCallsign().isEmpty())
                {
                    ui->lbl_ComPanelCom2Active->setToolTip("");
                    ui->led_ComPanelCom2->setOn(false);
                }
                else
                {
                    ui->lbl_ComPanelCom2Active->setToolTip(com2Station.getCallsign().getStringAsSet());
                    ui->led_ComPanelCom2->setOn(true);
                }
            }
        }

        void CCockpitComComponent::ps_testSelcal()
        {
            CSelcal selcal = this->getSelcal();
            if (!selcal.isValid())
            {
                CLogMessage().validationWarning("Invalid SELCAL code");
            }
            else if (sGui->getIContextAudio())
            {
                sGui->getIContextAudio()->playSelcalTone(selcal);
            }
            else
            {
                CLogMessage().validationWarning("No audio available");
            }
        }

        void CCockpitComComponent::ps_onChangedSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            if (isMyIdentifier(originator)) { return; } // comes from myself
            ui->frp_ComPanelSelcalBottom->setSelcalCode(selcal);
        }

        CSelcal CCockpitComComponent::getSelcal() const
        {
            return ui->frp_ComPanelSelcalBottom->getSelcal();
        }

        void CCockpitComComponent::initLeds()
        {
            CLedWidget::LedShape shape = CLedWidget::Rounded;
            ui->led_ComPanelCom1->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM1 connected", "COM1 disconnected", 14);
            ui->led_ComPanelCom2->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM2 connected", "COM2 disconnected", 14);
        }

        CSimulatedAircraft CCockpitComComponent::cockpitValuesToAircraftObject()
        {
            CSimulatedAircraft ownAircraft = this->getOwnAircraft();
            CTransponder transponder = ownAircraft.getTransponder();
            CComSystem com1 = ownAircraft.getCom1System();
            CComSystem com2 = ownAircraft.getCom2System();

            //
            // Transponder
            //
            QString transponderCode = QString::number(ui->sbp_ComPanelTransponder->value());
            if (CTransponder::isValidTransponderCode(transponderCode))
            {
                transponder.setTransponderCode(transponderCode);
            }
            else
            {
                CLogMessage().validationWarning("Wrong transponder code, reset");
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
            this->updateFrequencyDisplaysFromComSystems(com1, com2); // back annotation after rounding

            ownAircraft.setCom1System(com1);
            ownAircraft.setCom2System(com2);
            ownAircraft.setTransponder(transponder);
            return ownAircraft;
        }

        CSimulatedAircraft CCockpitComComponent::getOwnAircraft() const
        {
            // unavailable context during shutdown possible
            // mostly when client runs with DBus, but DBus is down
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return CSimulatedAircraft(); }
            return sGui->getIContextOwnAircraft()->getOwnAircraft();
        }

        bool CCockpitComComponent::updateOwnCockpitInContext(const CSimulatedAircraft &ownAircraft)
        {
            // unavailable context during shutdown possible
            // mostly when client runs with DBus, but DBus is down
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return false; }
            return sGui->getIContextOwnAircraft()->updateCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), ownAircraft.getTransponder(), identifier());
        }

        void CCockpitComComponent::updateFrequencyDisplaysFromComSystems(const CComSystem &com1, const CComSystem &com2)
        {
            double freq = com1.getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != ui->ds_ComPanelCom1Active->value())
            {
                ui->ds_ComPanelCom1Active->setValue(freq);
            }

            freq = com2.getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != ui->ds_ComPanelCom2Active->value())
            {
                ui->ds_ComPanelCom2Active->setValue(freq);
            }

            freq = com1.getFrequencyStandby().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != ui->ds_ComPanelCom1Standby->value())
            {
                ui->ds_ComPanelCom1Standby->setValue(freq);
            }

            freq = com2.getFrequencyStandby().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != ui->ds_ComPanelCom2Standby->value())
            {
                ui->ds_ComPanelCom2Standby->setValue(freq);
            }
        }

        void CCockpitComComponent::ps_onChangedVoiceRoomStatus(const CVoiceRoomList &selectedVoiceRooms, bool connected)
        {
            Q_ASSERT(selectedVoiceRooms.size() == 2);
            CVoiceRoom room1 = selectedVoiceRooms[0];
            CVoiceRoom room2 = selectedVoiceRooms[1];
            ui->led_ComPanelCom1->setOn(room1.isConnected());
            ui->led_ComPanelCom2->setOn(room2.isConnected());
            Q_UNUSED(connected);
        }
    } // namespace
} // namespace
