/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "interpolationsetupform.h"
#include "ui_interpolationsetupform.h"
#include "blackgui/guiutility.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
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
            for (QCheckBox *cb : std::as_const(m_allCheckBoxes))
            {
                connect(cb, &QCheckBox::stateChanged, this, &CInterpolationSetupForm::onCheckboxChanged);
            }

            // one connect is enough, otherwise 2 change signals
            connect(ui->rb_Linear, &QRadioButton::toggled, this, &CInterpolationSetupForm::onInterpolatorModeChanged);

            // pitch
            connect(ui->le_PitchOnGround, &QLineEdit::editingFinished, this, &CInterpolationSetupForm::onPitchChanged);
        }

        CInterpolationSetupForm::~CInterpolationSetupForm()
        { }

        void CInterpolationSetupForm::setValue(const CInterpolationAndRenderingSetupBase &setup)
        {
            ui->cb_DebugDriver->setChecked(setup.showSimulatorDebugMessages());
            ui->cb_LogInterpolation->setChecked(setup.logInterpolation());
            ui->cb_EnableParts->setChecked(setup.isAircraftPartsEnabled());
            ui->cb_ForceFullInterpolation->setChecked(setup.isForcingFullInterpolation());
            ui->cb_SendGndFlagToSim->setChecked(setup.isSendingGndFlagToSimulator());
            ui->cb_FixSceneryOffset->setChecked(setup.isFixingSceneryOffset());
            this->setInterpolatorMode(setup.getInterpolatorMode());
            this->displayPitchOnGround(setup.getPitchOnGround());
        }

        CInterpolationAndRenderingSetupPerCallsign CInterpolationSetupForm::getValue() const
        {
            CInterpolationAndRenderingSetupPerCallsign setup;
            setup.setEnabledAircraftParts(ui->cb_EnableParts->isChecked());
            setup.setForceFullInterpolation(ui->cb_ForceFullInterpolation->isChecked());
            setup.setLogInterpolation(ui->cb_LogInterpolation->isChecked());
            setup.setSendingGndFlagToSimulator(ui->cb_SendGndFlagToSim->isChecked());
            setup.setSimulatorDebuggingMessages(ui->cb_DebugDriver->isChecked());
            setup.setFixingSceneryOffset(ui->cb_FixSceneryOffset->isChecked());
            setup.setInterpolatorMode(this->getInterpolatorMode());
            setup.setPitchOnGround(this->getPitchOnGround());
            return setup;
        }

        void CInterpolationSetupForm::setReadOnly(bool readonly)
        {
            CGuiUtility::checkBoxReadOnly(ui->cb_DebugDriver, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_LogInterpolation, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_EnableParts, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ForceFullInterpolation, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_SendGndFlagToSim, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_FixSceneryOffset, readonly);
            ui->le_PitchOnGround->setReadOnly(readonly);

            const bool enabled = !readonly;
            ui->rb_Linear->setEnabled(enabled);
            ui->rb_Spline->setEnabled(enabled);
        }

        CStatusMessageList CInterpolationSetupForm::validate(bool nested) const
        {
            Q_UNUSED(nested)
            return CStatusMessageList();
        }

        void CInterpolationSetupForm::onCheckboxChanged(int state)
        {
            Q_UNUSED(state)
            emit this->valueChanged();
        }

        CInterpolationAndRenderingSetupBase::InterpolatorMode CInterpolationSetupForm::getInterpolatorMode() const
        {
            if (ui->rb_Linear->isChecked()) { return CInterpolationAndRenderingSetupBase::Linear; }
            return CInterpolationAndRenderingSetupBase::Spline;
        }

        void CInterpolationSetupForm::setInterpolatorMode(CInterpolationAndRenderingSetupBase::InterpolatorMode mode)
        {
            switch (mode)
            {
            case CInterpolationAndRenderingSetupBase::Linear : ui->rb_Linear->setChecked(true); break;
            case CInterpolationAndRenderingSetupBase::Spline:
            default:
                ui->rb_Spline->setChecked(true);
                break;
            }
        }

        void CInterpolationSetupForm::onInterpolatorModeChanged(bool checked)
        {
            Q_UNUSED(checked)
            emit this->valueChanged();
        }

        void CInterpolationSetupForm::onPitchChanged()
        {
            const CAngle pitchOnGround = this->getPitchOnGround();
            this->displayPitchOnGround(pitchOnGround);
        }

        CAngle CInterpolationSetupForm::getPitchOnGround() const
        {
            CAngle pitch;
            const QString p = ui->le_PitchOnGround->text().trimmed();
            pitch.parseFromString(p, CPqString::SeparatorBestGuess, CAngleUnit::deg());
            return pitch;
        }

        void CInterpolationSetupForm::displayPitchOnGround(const CAngle &pitchOnGround)
        {
            const QString p = pitchOnGround.valueRoundedWithUnit(CAngleUnit::deg(), 1, true);
            ui->le_PitchOnGround->setText(p);
        }
    } // ns
} // ns
