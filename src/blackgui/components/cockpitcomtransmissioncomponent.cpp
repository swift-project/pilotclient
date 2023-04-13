/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "cockpitcomtransmissioncomponent.h"
#include "ui_cockpitcomtransmissioncomponent.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackGui::Components
{
    CCockpitComTransmissionComponent::CCockpitComTransmissionComponent(QWidget *parent) : QFrame(parent),
                                                                                          ui(new Ui::CCockpitComTransmissionComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_Com1Save, &QPushButton::released, this, &CCockpitComTransmissionComponent::onSave);
        connect(ui->pb_Com2Save, &QPushButton::released, this, &CCockpitComTransmissionComponent::onSave);
    }

    CCockpitComTransmissionComponent::~CCockpitComTransmissionComponent()
    {
        // void
    }

    void CCockpitComTransmissionComponent::updateComSystem(CComSystem &comSystem, CComSystem::ComUnit comUnit) const
    {
        switch (comUnit)
        {
        case CComSystem::Com1:
            comSystem.setVolumeReceive(ui->sb_Com1VolumeRec->value());
            comSystem.setVolumeTransmit(ui->sb_Com1VolumeTx->value());
            comSystem.setTransmitEnabled(ui->cb_Com1Tx->isChecked());
            comSystem.setReceiveEnabled(ui->cb_Com1Rec->isChecked());
            break;
        case CComSystem::Com2:
            comSystem.setVolumeReceive(ui->sb_Com2VolumeRec->value());
            comSystem.setVolumeTransmit(ui->sb_Com2VolumeTx->value());
            comSystem.setTransmitEnabled(ui->cb_Com2Tx->isChecked());
            comSystem.setReceiveEnabled(ui->cb_Com2Rec->isChecked());
            break;
        default:
            break;
        }
    }

    void CCockpitComTransmissionComponent::setComSystem(const CComSystem &comSystem, CComSystem::ComUnit comUnit)
    {
        switch (comUnit)
        {
        case CComSystem::Com1:
            ui->sb_Com1VolumeRec->setValue(comSystem.getVolumeReceive());
            ui->sb_Com1VolumeTx->setValue(comSystem.getVolumeTransmit());
            ui->cb_Com1Tx->setChecked(comSystem.isTransmitEnabled());
            ui->cb_Com1Rec->setChecked(comSystem.isReceiveEnabled());
            break;
        case CComSystem::Com2:
            ui->sb_Com2VolumeRec->setValue(comSystem.getVolumeReceive());
            ui->sb_Com2VolumeTx->setValue(comSystem.getVolumeTransmit());
            ui->cb_Com2Tx->setChecked(comSystem.isTransmitEnabled());
            ui->cb_Com2Rec->setChecked(comSystem.isReceiveEnabled());
            break;
        default:
            break;
        }
    }

    void CCockpitComTransmissionComponent::setComSystems(const CSimulatedAircraft &aircraft)
    {
        this->setComSystem(aircraft.getCom1System(), CComSystem::Com1);
        this->setComSystem(aircraft.getCom2System(), CComSystem::Com2);
    }

    void CCockpitComTransmissionComponent::onSave()
    {
        const QObject *s = QObject::sender();
        const CComSystem::ComUnit unit = (s == ui->pb_Com2Save) ? CComSystem::Com2 : CComSystem::Com1;
        emit this->changedValues(unit);
    }
} // ns
