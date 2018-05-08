/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationsetupform.h"
#include "ui_interpolationsetupform.h"
#include "blackgui/guiutility.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Editors
    {
        CInterpolationSetupForm::CInterpolationSetupForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CInterpolationSetupForm)
        {
            ui->setupUi(this);
            m_allCheckBoxes = this->findChildren<QCheckBox *>(QString(), Qt::FindDirectChildrenOnly);
            for (QCheckBox *cb : m_allCheckBoxes)
            {
                connect(cb, &QCheckBox::stateChanged, this, &CInterpolationSetupForm::onCheckboxChanged);
            }

            connect(ui->co_InterpolatorMode, &QComboBox::currentTextChanged, this, &CInterpolationSetupForm::onInterpolatorModeChanged);
        }

        CInterpolationSetupForm::~CInterpolationSetupForm()
        { }

        void CInterpolationSetupForm::setValue(const CInterpolationAndRenderingSetupBase &setup)
        {
            ui->cb_DebugDriver->setChecked(setup.showSimulatorDebugMessages());
            ui->cb_LogInterpolation->setChecked(setup.logInterpolation());
            ui->cb_EnableParts->setChecked(setup.isAircraftPartsEnabled());
            ui->cb_ForceFullInterpolation->setChecked(setup.isForcingFullInterpolation());
            ui->cb_EnableGndFlag->setChecked(setup.isGndFlagEnabled());
            ui->cb_SendGndFlagToSim->setChecked(setup.isSendingGndFlagToSimulator());
            ui->cb_FixSceneryOffset->setChecked(setup.isFixingSceneryOffset());

            const QString im = setup.getInterpolatorModeAsString();
            if (im.contains("linear", Qt::CaseInsensitive)) { ui->co_InterpolatorMode->setCurrentIndex(1); }
            else { ui->co_InterpolatorMode->setCurrentIndex(0); }
        }

        CInterpolationAndRenderingSetupPerCallsign CInterpolationSetupForm::getValue() const
        {
            CInterpolationAndRenderingSetupPerCallsign setup;
            setup.setEnabledAircraftParts(ui->cb_EnableParts->isChecked());
            setup.setEnabledGndFLag(ui->cb_EnableGndFlag->isChecked());
            setup.setForceFullInterpolation(ui->cb_ForceFullInterpolation->isChecked());
            setup.setLogInterpolation(ui->cb_LogInterpolation->isChecked());
            setup.setSendingGndFlagToSimulator(ui->cb_SendGndFlagToSim->isChecked());
            setup.setSimulatorDebuggingMessages(ui->cb_DebugDriver->isChecked());
            setup.setFixingSceneryOffset(ui->cb_FixSceneryOffset->isChecked());
            setup.setInterpolatorMode(ui->co_InterpolatorMode->currentText());
            return setup;
        }

        void CInterpolationSetupForm::setReadOnly(bool readonly)
        {
            CGuiUtility::checkBoxReadOnly(ui->cb_DebugDriver, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_LogInterpolation, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_EnableParts, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ForceFullInterpolation, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_EnableGndFlag, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_SendGndFlagToSim, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_FixSceneryOffset, readonly);
            ui->co_InterpolatorMode->setEnabled(!readonly);
        }

        CStatusMessageList CInterpolationSetupForm::validate(bool nested) const
        {
            Q_UNUSED(nested);
            return CStatusMessageList();
        }

        void CInterpolationSetupForm::onCheckboxChanged(int state)
        {
            Q_UNUSED(state);
            emit this->valueChanged();
        }

        void CInterpolationSetupForm::onInterpolatorModeChanged(const QString &mode)
        {
            Q_UNUSED(mode);
            emit this->valueChanged();
        }
    } // ns
} // ns
