// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/simulatorselector.h"

#include <QCheckBox>
#include <QPointer>
#include <QRadioButton>
#include <QWidget>
#include <QtGlobal>

#include "ui_simulatorselector.h"

#include "config/buildconfig.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/icons.h"
#include "misc/mixin/mixincompare.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::core::context;

namespace swift::gui::components
{
    CSimulatorSelector::CSimulatorSelector(QWidget *parent) : QFrame(parent), ui(new Ui::CSimulatorSelector)
    {
        ui->setupUi(this);

        this->addComboxBoxValues();
        this->setMode(CheckBoxes, true);

        connect(ui->rb_FS9, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
        connect(ui->rb_FSX, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
        connect(ui->rb_P3D, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
        connect(ui->rb_FG, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
        connect(ui->rb_XPlane, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);
        connect(ui->rb_MSFS, &QRadioButton::toggled, this, &CSimulatorSelector::radioButtonChanged);

        connect(ui->cb_FS9, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
        connect(ui->cb_FSX, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
        connect(ui->cb_P3D, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
        connect(ui->cb_FG, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
        connect(ui->cb_XPlane, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);
        connect(ui->cb_MSFS, &QRadioButton::toggled, this, &CSimulatorSelector::checkBoxChanged);

        connect(ui->cb_Simulators, &QComboBox::currentTextChanged, this, &CSimulatorSelector::comboBoxChanged);
    }

    CSimulatorSelector::~CSimulatorSelector() {}

    void CSimulatorSelector::setMode(CSimulatorSelector::Mode mode, bool forced)
    {
        if (m_mode == mode && !forced) { return; }
        m_mode = mode;

        ui->wi_CheckBoxes->setVisible(false);
        ui->wi_RadioButtons->setVisible(false);
        ui->wi_ComboBox->setVisible(false);

        switch (mode)
        {
        default:
        case CheckBoxes: ui->wi_CheckBoxes->setVisible(true); break;
        case RadioButtons: ui->wi_RadioButtons->setVisible(true); break;
        case ComboBox: ui->wi_ComboBox->setVisible(true); break;
        }
        this->setToLastSelection();
    }

    CSimulatorInfo CSimulatorSelector::getValue() const
    {
        if (m_noSelectionMeansAll && this->isUnselected()) { return CSimulatorInfo::allSimulators(); }

        switch (m_mode)
        {
        default:
        case CheckBoxes:
            return CSimulatorInfo(ui->cb_FSX->isChecked(), ui->cb_FS9->isChecked(), ui->cb_XPlane->isChecked(),
                                  ui->cb_P3D->isChecked(), ui->cb_FG->isChecked(), ui->cb_MSFS->isChecked());
        case RadioButtons:
            return CSimulatorInfo(ui->rb_FSX->isChecked(), ui->rb_FS9->isChecked(), ui->rb_XPlane->isChecked(),
                                  ui->rb_P3D->isChecked(), ui->rb_FG->isChecked(), ui->rb_MSFS->isChecked());
        case ComboBox: return CSimulatorInfo(ui->cb_Simulators->currentText());
        }
    }

    void CSimulatorSelector::setValue(const CSimulatorInfo &simulator)
    {
        const CSimulatorInfo current(this->getValue());
        if (simulator == current) { return; } // avoid unnecessary signals

        // checkboxes
        ui->cb_FSX->setChecked(simulator.isFSX());
        ui->cb_FS9->setChecked(simulator.isFS9());
        ui->cb_XPlane->setChecked(simulator.isXPlane());
        ui->cb_P3D->setChecked(simulator.isP3D());
        ui->cb_FG->setChecked(simulator.isFG());
        ui->cb_MSFS->setChecked(simulator.isMSFS());

        // Combo
        ui->cb_Simulators->setCurrentText(simulator.toQString(true));

        // radio buttons
        if (simulator.isFSX())
        {
            ui->rb_FSX->setChecked(simulator.isFSX());
            return;
        }
        if (simulator.isFS9())
        {
            ui->rb_FS9->setChecked(simulator.isFS9());
            return;
        }
        if (simulator.isXPlane())
        {
            ui->rb_XPlane->setChecked(simulator.isXPlane());
            return;
        }
        if (simulator.isP3D())
        {
            ui->rb_P3D->setChecked(simulator.isP3D());
            return;
        }
        if (simulator.isFG())
        {
            ui->rb_FG->setChecked(simulator.isFG());
            return;
        }
        if (simulator.isMSFS())
        {
            ui->rb_MSFS->setChecked(simulator.isMSFS());
            return;
        }
    }

    void CSimulatorSelector::setToLastSelection()
    {
        const CSimulatorInfo simulator =
            (m_mode == RadioButtons || m_mode == ComboBox) ? m_currentSimulator.get() : m_currentSimulators.get();
        this->setValue(simulator);
    }

    void CSimulatorSelector::setToConnectedSimulator(bool makeReadOnly)
    {
        if (sGui && sGui->supportsContexts() && sGui->getIContextSimulator())
        {
            const CSimulatorPluginInfo pluginInfo = sGui->getIContextSimulator()->getSimulatorPluginInfo();
            if (!this->isSingleSelection()) { this->setMode(RadioButtons); } // only one sim can be connected

            if (pluginInfo.isValid())
            {
                this->setReadOnly(makeReadOnly);
                this->setValue(pluginInfo.getSimulator());
            }
            else
            {
                if (makeReadOnly) { this->setReadOnly(false); }
                const CSimulatorInfo simulator = sGui->getIContextSimulator()->getModelSetLoaderSimulator();
                if (simulator.isSingleSimulator()) { this->setValue(simulator); }
            }
        }
        else
        {
            if (makeReadOnly) { this->setReadOnly(false); }
        }
    }

    void CSimulatorSelector::setToConnectedSimulator(int deferredMs, bool makeReadOnly)
    {
        if (deferredMs < 1)
        {
            this->setToConnectedSimulator(makeReadOnly);
            return;
        }

        if (!sGui || sGui->isShuttingDown()) { return; }
        QPointer<CSimulatorSelector> myself(this);
        QTimer::singleShot(deferredMs, this, [=] {
            if (!sGui || sGui->isShuttingDown() || !myself) { return; }
            this->setToConnectedSimulator(makeReadOnly);
        });
    }

    void CSimulatorSelector::setFsxP3DOnly()
    {
        ui->cb_FS9->setVisible(false);
        ui->cb_XPlane->setVisible(false);
        ui->cb_FG->setVisible(false);
        ui->rb_FS9->setVisible(false);
        ui->rb_XPlane->setVisible(false);
        ui->rb_FG->setVisible(false);
    }

    void CSimulatorSelector::enableFG(bool enabled)
    {
        ui->cb_FG->setVisible(enabled);
        ui->rb_FG->setVisible(enabled);
        ui->cb_FG->setChecked(false);
        ui->rb_FG->setChecked(false);
    }

    void CSimulatorSelector::checkAll()
    {
        // checkboxes
        ui->cb_FSX->setChecked(true);
        ui->cb_FS9->setChecked(true);
        ui->cb_XPlane->setChecked(true);
        ui->cb_P3D->setChecked(true);
        ui->cb_FG->setChecked(true);
        ui->cb_MSFS->setChecked(true);

        // radio
        ui->rb_P3D->setChecked(true);
    }

    void CSimulatorSelector::uncheckAll()
    {
        // checkboxes
        ui->cb_FSX->setChecked(false);
        ui->cb_FS9->setChecked(false);
        ui->cb_XPlane->setChecked(false);
        ui->cb_P3D->setChecked(false);
        ui->cb_FG->setChecked(false);
        ui->cb_MSFS->setChecked(false);
    }

    bool CSimulatorSelector::isUnselected() const
    {
        bool c = false;
        switch (m_mode)
        {
        default:
        case CheckBoxes:
            c = ui->cb_FSX->isChecked() || ui->cb_FS9->isChecked() || ui->cb_XPlane->isChecked() ||
                ui->cb_P3D->isChecked() || ui->cb_FG->isChecked() || ui->cb_MSFS->isChecked();
            break;
        case RadioButtons:
            c = ui->rb_FSX->isChecked() || ui->rb_FS9->isChecked() || ui->rb_XPlane->isChecked() ||
                ui->rb_P3D->isChecked() || ui->rb_FG->isChecked() || ui->rb_MSFS->isChecked();
            break;
        case ComboBox:
            const int i = ui->cb_Simulators->currentIndex();
            c = i < 0;
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
            c = ui->cb_FSX->isChecked() && ui->cb_FS9->isChecked() && ui->cb_XPlane->isChecked() &&
                ui->cb_P3D->isChecked() && ui->cb_FG->isChecked() && ui->cb_MSFS->isChecked();
            break;
        case RadioButtons:
            // actually this should never be true
            c = false;
            break;
        case ComboBox:
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

    void CSimulatorSelector::setRememberSelectionAndSetToLastSelection()
    {
        this->setRememberSelection(true);
        this->setToLastSelection();
    }

    void CSimulatorSelector::clear()
    {
        if (m_mode == CheckBoxes) { this->uncheckAll(); }
    }

    bool CSimulatorSelector::isSingleSelection() const { return m_mode == RadioButtons || m_mode == ComboBox; }

    void CSimulatorSelector::setReadOnly(bool readOnly)
    {
        CGuiUtility::checkBoxesReadOnly(this, readOnly);
        ui->rb_FSX->setEnabled(!readOnly);
        ui->rb_FS9->setEnabled(!readOnly);
        ui->rb_XPlane->setEnabled(!readOnly);
        ui->rb_P3D->setEnabled(!readOnly);
        ui->rb_FG->setEnabled(!readOnly);
        ui->rb_MSFS->setEnabled(!readOnly);

        ui->cb_Simulators->setEnabled(!readOnly);

        this->setEnabled(!readOnly);
    }

    void CSimulatorSelector::radioButtonChanged(bool checked)
    {
        if (m_mode != RadioButtons) { return; }
        if (!checked)
        {
            return;
        } // only the checked ones are relevant, as the unchecked ones are accompanied with checked events
        m_digestButtonsChanged.inputSignal();
    }

    void CSimulatorSelector::checkBoxChanged(bool checked)
    {
        if (m_mode != CheckBoxes) { return; }
        Q_UNUSED(checked);
        m_digestButtonsChanged.inputSignal();
    }

    void CSimulatorSelector::comboBoxChanged(const QString &value)
    {
        if (m_mode != ComboBox) { return; }
        Q_UNUSED(value);
        m_digestButtonsChanged.inputSignal();
    }

    void CSimulatorSelector::rememberSelection()
    {
        if (!m_rememberSelection) { return; }
        if (this->isSingleSelection())
        {
            // single
            const CSimulatorInfo sim = this->getValue();
            m_currentSimulator.set(sim);
        }
        else
        {
            // multiple
            const CSimulatorInfo sim = this->getValue();
            m_currentSimulators.set(sim);
        }
    }

    void CSimulatorSelector::changedLastSelection()
    {
        // force decoupled update
        this->triggerSetToLastSelection();
    }

    void CSimulatorSelector::changedLastSelectionRb()
    {
        // force decoupled update
        if (m_mode != RadioButtons) { return; }
        this->triggerSetToLastSelection();
    }

    void CSimulatorSelector::changedLastSelectionCb()
    {
        // force decoupled update
        if (m_mode != CheckBoxes) { return; }
        this->triggerSetToLastSelection();
    }

    void CSimulatorSelector::triggerSetToLastSelection()
    {
        QPointer<CSimulatorSelector> myself(this);
        QTimer::singleShot(100, this, [=] {
            if (!myself) { return; }
            this->setToLastSelection();
        });
    }

    void CSimulatorSelector::emitChangedSignal()
    {
        const CSimulatorInfo simulator(this->getValue());
        this->rememberSelection();
        emit this->changed(simulator);
    }

    void CSimulatorSelector::addComboxBoxValues()
    {
        int cbi = 0;
        ui->cb_Simulators->clear();
        ui->cb_Simulators->insertItem(cbi++, CSimulatorInfo::fs9().toQString());
        ui->cb_Simulators->insertItem(cbi++, CSimulatorInfo::fsx().toQString());
        ui->cb_Simulators->insertItem(cbi++, CSimulatorInfo::p3d().toQString());
        ui->cb_Simulators->insertItem(cbi++, CSimulatorInfo::xplane().toQString());
        ui->cb_Simulators->insertItem(cbi++, CSimulatorInfo::fg().toQString());
        ui->cb_Simulators->insertItem(cbi++, CSimulatorInfo::msfs().toQString());
    }
} // namespace swift::gui::components
