/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "voicesetupform.h"
#include "ui_voicesetupform.h"
#include <QIntValidator>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Editors
    {
        CVoiceSetupForm::CVoiceSetupForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CVoiceSetupForm)
        {
            ui->setupUi(this);
            ui->cb_Override->setChecked(true);
            QIntValidator *v = new QIntValidator(0, 65535, ui->le_VatsimUdpPort);
            ui->le_VatsimUdpPort->setValidator(v);
            this->resetToDefaultValues();
            connect(ui->cb_Override, &QCheckBox::toggled, this, &CVoiceSetupForm::enabledToggled, Qt::QueuedConnection);
            connect(ui->pb_SetDefaults, &QPushButton::clicked, this, &CVoiceSetupForm::resetToDefaultValues);
        }

        CVoiceSetupForm::~CVoiceSetupForm()
        { }

        CVoiceSetup CVoiceSetupForm::getValue() const
        {
            const CVoiceSetup s = CVoiceSetup(ui->le_VatsimUdpPort->text().toInt());
            return s;
        }

        const CVoiceSetup &CVoiceSetupForm::getDisabledValue() const
        {
            static const CVoiceSetup s;
            return s;
        }

        void CVoiceSetupForm::setValue(const CVoiceSetup &setup)
        {
            ui->le_VatsimUdpPort->setText(QString::number(setup.getVatsimUdpVoicePort()));
        }

        bool CVoiceSetupForm::isVoiceSetupEnabled() const
        {
            return ui->cb_Override->isChecked();
        }

        void CVoiceSetupForm::setVoiceSetupEnabled(bool enabled)
        {
            ui->cb_Override->setChecked(enabled);
        }

        void CVoiceSetupForm::showEnableInfo(bool visible)
        {
            m_visibleEnableInfo = visible;
            this->visibleEnableInfo(visible);
        }

        void CVoiceSetupForm::setReadOnly(bool readonly)
        {
            ui->pb_SetDefaults->setEnabled(!readonly);
            ui->le_VatsimUdpPort->setReadOnly(readonly);
            if (readonly && ui->cb_Override->isChecked())
            {
                ui->cb_Override->setChecked(false);
            }
            this->forceStyleSheetUpdate();
        }

        CStatusMessageList CVoiceSetupForm::validate(bool nested) const
        {
            Q_UNUSED(nested);
            const CVoiceSetup val(this->getValue());
            CStatusMessageList msgs(val.validate());
            if (this->isReadOnly())
            {
                // in readonly I cannot change the data anyway, so skip warnings
                msgs.removeWarningsAndBelow();
            }
            return msgs;
        }

        void CVoiceSetupForm::enabledToggled(bool enabled)
        {
            Q_UNUSED(enabled);
            this->setReadOnly(!enabled);
        }

        void CVoiceSetupForm::visibleEnableInfo(bool visible)
        {
            ui->cb_Override->setVisible(visible);
            ui->lbl_VoiceSetup->setVisible(visible);
            ui->pb_SetDefaults->setVisible(visible);
        }

        void CVoiceSetupForm::resetToDefaultValues()
        {
            CVoiceSetup s;
            this->setValue(s);
        }
    } // ns
} // ns
