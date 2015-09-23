/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorselector.h"
#include "ui_simulatorselector.h"

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    CSimulatorSelector::CSimulatorSelector(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CSimulatorSelector)
    {
        ui->setupUi(this);
        this->setMode(CheckBoxes);
    }

    CSimulatorSelector::~CSimulatorSelector()
    { }

    void CSimulatorSelector::setMode(CSimulatorSelector::Mode mode)
    {
        this->m_mode = mode;
        switch (mode)
        {
        default:
        case CheckBoxes:
            this->ui->wi_CheckBoxes->setVisible(true);
            this->ui->wi_RadioButtons->setVisible(false);
            break;
        case RadioButtons:
            this->ui->wi_CheckBoxes->setVisible(false);
            this->ui->wi_RadioButtons->setVisible(true);
            break;
        }
    }

    CSimulatorInfo CSimulatorSelector::getValue() const
    {
        switch (this->m_mode)
        {
        default:
        case CheckBoxes:
            return CSimulatorInfo(this->ui->cb_FSX->isChecked(), this->ui->cb_FS9->isChecked(),
                                  this->ui->cb_XPlane->isChecked(), this->ui->cb_P3D->isChecked());
        case RadioButtons:
            return CSimulatorInfo(this->ui->rb_FSX->isChecked(), this->ui->rb_FS9->isChecked(),
                                  this->ui->rb_XPlane->isChecked(), this->ui->cb_P3D->isChecked());
        }
    }

    void CSimulatorSelector::setValue(const CSimulatorInfo &info)
    {
        this->ui->cb_FSX->setChecked(info.fsx());
        this->ui->cb_FS9->setChecked(info.fs9());
        this->ui->cb_XPlane->setChecked(info.xplane());
        this->ui->cb_P3D->setChecked(info.p3d());

        if (info.fsx()) { this->ui->cb_FSX->setChecked(info.fsx()); return; }
        if (info.fs9()) { this->ui->cb_FS9->setChecked(info.fs9()); return; }
        if (info.xplane()) { this->ui->cb_XPlane->setChecked(info.xplane()); return; }
        if (info.p3d()) { this->ui->cb_P3D->setChecked(info.p3d()); return; }
    }

    void CSimulatorSelector::setAll()
    {
        this->ui->cb_FSX->setChecked(true);
        this->ui->cb_FS9->setChecked(true);
        this->ui->cb_XPlane->setChecked(true);
        this->ui->cb_P3D->setChecked(true);

        this->ui->cb_FSX->setChecked(true);
    }
}
