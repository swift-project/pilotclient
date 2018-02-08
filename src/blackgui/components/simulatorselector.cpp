/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/simulatorselector.h"
#include "blackmisc/compare.h"
#include "ui_simulatorselector.h"

#include <QCheckBox>
#include <QRadioButton>
#include <QWidget>
#include <QtGlobal>

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CSimulatorSelector::CSimulatorSelector(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSimulatorSelector)
        {
            ui->setupUi(this);
            this->setMode(CheckBoxes);

            connect(ui->rb_FS9, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
            connect(ui->rb_FSX, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
            connect(ui->rb_P3D, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
            connect(ui->rb_XPlane, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);

            connect(ui->cb_FS9, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
            connect(ui->cb_FSX, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
            connect(ui->cb_P3D, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
            connect(ui->cb_XPlane, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
        }

        CSimulatorSelector::~CSimulatorSelector()
        { }

        void CSimulatorSelector::setMode(CSimulatorSelector::Mode mode)
        {
            m_mode = mode;
            switch (mode)
            {
            default:
            case CheckBoxes:
                ui->wi_CheckBoxes->setVisible(true);
                ui->wi_RadioButtons->setVisible(false);
                break;
            case RadioButtons:
                ui->wi_CheckBoxes->setVisible(false);
                ui->wi_RadioButtons->setVisible(true);
                break;
            }
        }

        CSimulatorInfo CSimulatorSelector::getValue() const
        {
            if (m_noSelectionMeansAll && this->isUnselected())
            {
                return CSimulatorInfo::allSimulators();
            }

            switch (m_mode)
            {
            default:
            case CheckBoxes:
                return CSimulatorInfo(ui->cb_FSX->isChecked(), ui->cb_FS9->isChecked(),
                                      ui->cb_XPlane->isChecked(), ui->cb_P3D->isChecked());
            case RadioButtons:
                return CSimulatorInfo(ui->rb_FSX->isChecked(), ui->rb_FS9->isChecked(),
                                      ui->rb_XPlane->isChecked(), ui->rb_P3D->isChecked());
            }
        }

        void CSimulatorSelector::setValue(const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo current(getValue());
            if (simulator == current) { return; } // avoid unnecessary signals

            // checkboxes
            ui->cb_FSX->setChecked(simulator.fsx());
            ui->cb_FS9->setChecked(simulator.fs9());
            ui->cb_XPlane->setChecked(simulator.xplane());
            ui->cb_P3D->setChecked(simulator.p3d());

            // radio buttons
            if (simulator.fsx())    { ui->rb_FSX->setChecked(simulator.fsx()); return; }
            if (simulator.fs9())    { ui->rb_FS9->setChecked(simulator.fs9()); return; }
            if (simulator.xplane()) { ui->rb_XPlane->setChecked(simulator.xplane()); return; }
            if (simulator.p3d())    { ui->rb_P3D->setChecked(simulator.p3d()); return; }
        }

        void CSimulatorSelector::setToLastSelection()
        {
            const CSimulatorInfo sim = m_currentSimulator.get();
            this->setValue(sim);
        }

        void CSimulatorSelector::setAll()
        {
            // checkboxes
            ui->cb_FSX->setChecked(true);
            ui->cb_FS9->setChecked(true);
            ui->cb_XPlane->setChecked(true);
            ui->cb_P3D->setChecked(true);

            // radio
            ui->rb_FSX->setChecked(true);
        }

        bool CSimulatorSelector::isUnselected() const
        {
            bool c = false;
            switch (m_mode)
            {
            default:
            case CheckBoxes:
                c = ui->cb_FSX->isChecked() || ui->cb_FS9->isChecked() ||
                    ui->cb_XPlane->isChecked() || ui->cb_P3D->isChecked();
                break;
            case RadioButtons:
                c = ui->rb_FSX->isChecked() || ui->rb_FS9->isChecked() ||
                    ui->rb_XPlane->isChecked() || ui->cb_P3D->isChecked();
                break;
            }
            return !c;
        }

        bool CSimulatorSelector::areAllSelected() const
        {
            bool c = false;
            switch (m_mode)
            {
            default:
            case CheckBoxes:
                c = ui->cb_FSX->isChecked() && ui->cb_FS9->isChecked() &&
                    ui->cb_XPlane->isChecked() && ui->cb_P3D->isChecked();
                break;
            case RadioButtons:
                // actually this should never be true
                c = false;
                break;
            }
            return c;
        }

        void CSimulatorSelector::setLeftMargin(int margin)
        {
            QMargins m = ui->hl_RadioButtons->contentsMargins();
            m.setLeft(margin);
            ui->hl_RadioButtons->setContentsMargins(m);

            m = ui->hl_CheckBoxes->contentsMargins();
            m.setLeft(margin);
            ui->hl_CheckBoxes->setContentsMargins(m);
        }

        void CSimulatorSelector::radioButtonChanged(bool checked)
        {
            if (m_mode != RadioButtons) { return; }
            if (!checked) { return; } // only the checked ones are relevant, as the unchecked ones are accompanied with checked events
            emit this->changed(this->getValue());
        }

        void CSimulatorSelector::checkBoxChanged(bool checked)
        {
            if (m_mode != CheckBoxes) { return; }
            Q_UNUSED(checked);
            emit this->changed(this->getValue());
        }
    } // ns
} // ns
