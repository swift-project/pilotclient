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
            ui->cb_SendGndFlagToSim->setChecked(setup.sendGndFlagToSimulator());
        }

        CInterpolationAndRenderingSetupPerCallsign CInterpolationSetupForm::getValue() const
        {
            CInterpolationAndRenderingSetupPerCallsign setup;
            setup.setEnabledAircraftParts(ui->cb_EnableParts->isChecked());
            setup.setEnabledGndFLag(ui->cb_EnableGndFlag->isChecked());
            setup.setForceFullInterpolation(ui->cb_ForceFullInterpolation->isChecked());
            setup.setLogInterpolation(ui->cb_LogInterpolation->isChecked());
            setup.setSendGndFlagToSimulator(ui->cb_SendGndFlagToSim->isChecked());
            setup.setSimulatorDebuggingMessages(ui->cb_DebugDriver->isChecked());
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
    } // ns
} // ns
