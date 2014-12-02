/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "cockpitcomcomponent.h"
#include "ui_cockpitcomcomponent.h"
#include "../stylesheetutility.h"
#include "blackcore/context_network.h"
#include "blackcore/context_ownaircraft.h"
#include "blackcore/context_audio.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/voiceroomlist.h"
#include "blackmisc/logmessage.h"

#include <QDoubleSpinBox>
#include <QPushButton>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {
        CCockpitComComponent::CCockpitComComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CCockpitMainComponent)
        {
            ui->setupUi(this);
            this->initLeds();
            QObject::connect(this->ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderModeChanged, this, &CCockpitComComponent::transponderModeChanged);
            QObject::connect(this->ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderStateIdentEnded, this, &CCockpitComComponent::transponderStateIdentEnded);
        }

        CCockpitComComponent::~CCockpitComComponent()
        { }

        void CCockpitComComponent::setSelectedTransponderModeStateIdent()
        {
            this->ui->cbp_ComPanelTransponderMode->setSelectedTransponderModeStateIdent();
        }

        void CCockpitComComponent::paintEvent(QPaintEvent *event)
        {
            Q_UNUSED(event);
            CStyleSheetUtility::useStyleSheetInDerivedWidget(this);
        }

        void CCockpitComComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getIContextOwnAircraft());
            Q_ASSERT(this->getIContextAudio());

            // init from aircraft
            CAircraft ownAircraft = this->getOwnAircraft();
            this->ps_updateCockpitFromContext(ownAircraft, "dummyInitialValues");

            // SELCAL pairs in cockpit
            this->ui->frp_ComPanelSelcalBottom->clear();
            connect(this->ui->pb_ComPanelSelcalTest, &QPushButton::clicked, this, &CCockpitComComponent::ps_testSelcal);

            // COM GUI events
            connect(this->ui->pb_ComPanelCom1Toggle, &QPushButton::clicked, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->pb_ComPanelCom2Toggle, &QPushButton::clicked, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->ds_ComPanelCom1Active, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->ds_ComPanelCom2Active, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->ds_ComPanelCom1Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->ds_ComPanelCom2Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->sbp_ComPanelTransponder, &QDoubleSpinBox::editingFinished, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->cbp_ComPanelTransponderMode, &CTransponderModeSelector::transponderModeChanged, this, &CCockpitComComponent::ps_guiChangedCockpitValues);
            connect(this->ui->frp_ComPanelSelcalBottom, &CSelcalCodeSelector::valueChanged, this, &CCockpitComComponent::ps_guiChangedSelcal);

            // hook up with changes from own aircraft context
            this->connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CCockpitComComponent::ps_updateCockpitFromContext);
            this->connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedSelcal, this, &CCockpitComComponent::ps_onChangedSelcal);

            // hook up with audio context
            this->connect(this->getIContextAudio(), &IContextAudio::changedVoiceRooms, this, &CCockpitComComponent::ps_onChangedVoiceRoomStatus);
        }

        void CCockpitComComponent::ps_guiChangedCockpitValues()
        {
            QObject *sender = QObject::sender();
            if (sender == this->ui->pb_ComPanelCom1Toggle)
            {
                if (this->ui->ds_ComPanelCom1Standby->value() == this->ui->ds_ComPanelCom1Active->value()) return;
                double f = this->ui->ds_ComPanelCom1Active->value();
                this->ui->ds_ComPanelCom1Active->setValue(this->ui->ds_ComPanelCom1Standby->value());
                this->ui->ds_ComPanelCom1Standby->setValue(f);
            }
            else if (sender == this->ui->pb_ComPanelCom2Toggle)
            {
                if (this->ui->ds_ComPanelCom2Standby->value() == this->ui->ds_ComPanelCom2Active->value()) return;
                double f = this->ui->ds_ComPanelCom2Active->value();
                this->ui->ds_ComPanelCom2Active->setValue(this->ui->ds_ComPanelCom2Standby->value());
                this->ui->ds_ComPanelCom2Standby->setValue(f);
            }

            const CAircraft ownAircraft = this->cockpitValuesToAircraftObject();
            this->updateOwnCockpitInContext(ownAircraft);
        }

        void CCockpitComComponent::ps_guiChangedSelcal()
        {
            this->getIContextOwnAircraft()->updateSelcal(this->getSelcal(), cockpitOriginator());
        }

        void CCockpitComComponent::ps_updateCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
        {
            if (originator == CCockpitComComponent::cockpitOriginator()) return; // comes from myself

            // update GUI elements
            // avoid unnecessary change events as far as possible
            const CComSystem com1 = ownAircraft.getCom1System(); // aircraft just updated or set from context
            const CComSystem com2 = ownAircraft.getCom2System();
            const CTransponder transponder = ownAircraft.getTransponder();

            // update the frequencies
            this->updateFrequencyDisplaysFromComSystems(com1, com2);

            // update transponder
            int tc = transponder.getTransponderCode();
            if (tc != this->ui->sbp_ComPanelTransponder->value())
            {
                this->ui->sbp_ComPanelTransponder->setValue(tc);
            }

            this->ui->cbp_ComPanelTransponderMode->setSelectedTransponderMode(transponder.getTransponderMode());

            if (this->getIContextNetwork())
            {
                CAtcStationList selectedStations = this->getIContextNetwork()->getSelectedAtcStations();
                CAtcStation com1Station = selectedStations[0];
                CAtcStation com2Station = selectedStations[1];
                if (com1Station.getCallsign().isEmpty())
                {
                    this->ui->lbl_ComPanelCom1Active->setToolTip("");
                    this->ui->led_ComPanelCom1->setOn(false);
                }
                else
                {
                    this->ui->lbl_ComPanelCom1Active->setToolTip(com1Station.getCallsign().getStringAsSet());
                    this->ui->led_ComPanelCom1->setOn(true);

                }
                if (com2Station.getCallsign().isEmpty())
                {
                    this->ui->lbl_ComPanelCom2Active->setToolTip("");
                    this->ui->led_ComPanelCom2->setOn(false);
                }
                else
                {
                    this->ui->lbl_ComPanelCom2Active->setToolTip(com2Station.getCallsign().getStringAsSet());
                    this->ui->led_ComPanelCom2->setOn(true);
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
            else if (this->getIContextAudio())
            {
                this->getIContextAudio()->playSelcalTone(selcal);
            }
            else
            {
                CLogMessage().validationWarning("No audio available");
            }
        }

        void CCockpitComComponent::ps_onChangedSelcal(const CSelcal &selcal, const QString &originator)
        {
            if (originator == CCockpitComComponent::cockpitOriginator()) return; // comes from myself
            this->ui->frp_ComPanelSelcalBottom->setSelcalCode(selcal);
        }

        CSelcal CCockpitComComponent::getSelcal() const
        {
            return ui->frp_ComPanelSelcalBottom->getSelcal();
        }

        void CCockpitComComponent::initLeds()
        {
            CLedWidget::LedShape shape = CLedWidget::Rounded;
            this->ui->led_ComPanelCom1->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM1 connected", "COM1 disconnected", 14);
            this->ui->led_ComPanelCom2->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "COM2 connected", "COM2 disconnected", 14);
        }

        CAircraft CCockpitComComponent::cockpitValuesToAircraftObject()
        {
            CAircraft ownAircraft = this->getOwnAircraft();
            CTransponder transponder = ownAircraft.getTransponder();
            CComSystem com1 = ownAircraft.getCom1System();
            CComSystem com2 = ownAircraft.getCom2System();

            //
            // Transponder
            //
            QString transponderCode = QString::number(this->ui->sbp_ComPanelTransponder->value());
            if (CTransponder::isValidTransponderCode(transponderCode))
            {
                transponder.setTransponderCode(transponderCode);
            }
            else
            {
                CLogMessage().validationWarning("Wrong transponder code, reset");
                this->ui->sbp_ComPanelTransponder->setValue(transponder.getTransponderCode());
            }
            transponder.setTransponderMode(this->ui->cbp_ComPanelTransponderMode->getSelectedTransponderMode());

            //
            // COM units
            //
            com1.setFrequencyActiveMHz(this->ui->ds_ComPanelCom1Active->value());
            com1.setFrequencyStandbyMHz(this->ui->ds_ComPanelCom1Standby->value());
            com2.setFrequencyActiveMHz(this->ui->ds_ComPanelCom2Active->value());
            com2.setFrequencyStandbyMHz(this->ui->ds_ComPanelCom2Standby->value());
            this->updateFrequencyDisplaysFromComSystems(com1, com2); // back annotation after rounding

            ownAircraft.setCom1System(com1);
            ownAircraft.setCom2System(com2);
            ownAircraft.setTransponder(transponder);
            return ownAircraft;
        }

        CAircraft CCockpitComComponent::getOwnAircraft() const
        {
            Q_ASSERT(this->getIContextOwnAircraft());
            if (!this->getIContextOwnAircraft()) return CAircraft();
            return this->getIContextOwnAircraft()->getOwnAircraft();
        }

        bool CCockpitComComponent::updateOwnCockpitInContext(const CAircraft &ownAircraft)
        {
            return this->getIContextOwnAircraft()->updateOwnCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), ownAircraft.getTransponder(), CCockpitComComponent::cockpitOriginator());
        }

        void CCockpitComComponent::updateFrequencyDisplaysFromComSystems(const CComSystem &com1, const CComSystem &com2)
        {
            double freq = com1.getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != this->ui->ds_ComPanelCom1Active->value())
            {
                this->ui->ds_ComPanelCom1Active->setValue(freq);
            }

            freq = com2.getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != this->ui->ds_ComPanelCom2Active->value())
            {
                this->ui->ds_ComPanelCom2Active->setValue(freq);
            }

            freq = com1.getFrequencyStandby().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != this->ui->ds_ComPanelCom1Standby->value())
            {
                this->ui->ds_ComPanelCom1Standby->setValue(freq);
            }

            freq = com2.getFrequencyStandby().valueRounded(CFrequencyUnit::MHz(), 3);
            if (freq != this->ui->ds_ComPanelCom2Standby->value())
            {
                this->ui->ds_ComPanelCom2Standby->setValue(freq);
            }
        }

        void CCockpitComComponent::ps_onChangedVoiceRoomStatus(const CVoiceRoomList &selectedVoiceRooms, bool connected)
        {
            Q_ASSERT(selectedVoiceRooms.size() == 2);
            CVoiceRoom room1 = selectedVoiceRooms[0];
            CVoiceRoom room2 = selectedVoiceRooms[1];
            this->ui->led_ComPanelCom1->setOn(room1.isConnected());
            this->ui->led_ComPanelCom2->setOn(room2.isConnected());
            Q_UNUSED(connected);
        }

        const QString &CCockpitComComponent::cockpitOriginator()
        {
            // string is generated once, the timestamp allows to use multiple
            // components (as long as they are not generated at the same ms)
            static const QString o = QString("COCKPITCOMCOMPONENT:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
            return o;
        }

    } // namespace
} // namespace
