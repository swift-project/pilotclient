/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "fsdsetupform.h"
#include "ui_fsdsetupform.h"
#include "blackmisc/stringutils.h"
#include "blackgui/guiutility.h"
#include <QCompleter>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Editors
    {
        CFsdSetupForm::CFsdSetupForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CFsdSetupForm)
        {
            ui->setupUi(this);
            this->resetToDefaultValues();
            ui->cb_Override->setChecked(true);
            ui->le_TextCodec->setCompleter(new QCompleter(textCodecNames(true, true), this));
            connect(ui->cb_Override, &QCheckBox::toggled, this, &CFsdSetupForm::enabledToggled);
            connect(ui->pb_SetDefaults, &QPushButton::clicked, this, &CFsdSetupForm::resetToDefaultValues);
        }

        CFsdSetupForm::~CFsdSetupForm()
        { }

        CFsdSetup CFsdSetupForm::getValue() const
        {
            if (!this->isEnabled()) { return this->getDisabledValue(); }
            CFsdSetup s(ui->le_TextCodec->text().trimmed().toLower());
            s.setSendReceiveDetails(
                ui->cb_AircraftPartsSend->isChecked(), ui->cb_AircraftPartsReceive->isChecked(),
                ui->cb_FastPositionSend->isChecked(), ui->cb_FastPositionReceive->isChecked()
            );
            return s;
        }

        const CFsdSetup &CFsdSetupForm::getDisabledValue() const
        {
            static const CFsdSetup fsd;
            return fsd;
        }

        void CFsdSetupForm::setValue(const BlackMisc::Network::CFsdSetup &setup)
        {
            ui->le_TextCodec->setText(setup.getTextCodec());
            const CFsdSetup::SendReceiveDetails d = setup.getSendReceiveDetails();

            ui->cb_AircraftPartsReceive->setChecked(d & CFsdSetup::ReceiveAircraftParts);
            ui->cb_AircraftPartsSend->setChecked(d & CFsdSetup::SendAircraftParts);
            ui->cb_FastPositionReceive->setChecked(d & CFsdSetup::ReceiveInterimPositions);
            ui->cb_FastPositionSend->setChecked(d & CFsdSetup::SendIterimPositions);
        }

        bool CFsdSetupForm::isFsdSetupEnabled() const
        {
            return ui->cb_Override->isChecked();
        }

        void CFsdSetupForm::setFsdSetupEnabled(bool enabled)
        {
            ui->cb_Override->setChecked(enabled);
        }

        void CFsdSetupForm::showEnableInfo(bool visible)
        {
            ui->cb_Override->setVisible(visible);
            ui->lbl_FsdSetup->setVisible(visible);
            ui->pb_SetDefaults->setVisible(visible);
        }

        void CFsdSetupForm::setReadOnly(bool readonly)
        {
            this->showEnableInfo(!readonly);
            ui->le_TextCodec->setReadOnly(!readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_AircraftPartsReceive, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_AircraftPartsSend, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_FastPositionReceive, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_FastPositionSend, readonly);
        }

        CStatusMessageList CFsdSetupForm::validate(bool nested) const
        {
            Q_UNUSED(nested);
            const CFsdSetup val(getValue());
            CStatusMessageList msgs(val.validate());
            if (this->isReadOnly())
            {
                // in readonly I cannot change the data anyway, so skip warnings
                msgs.removeWarningsAndBelow();
            }
            return msgs;
        }

        void CFsdSetupForm::enabledToggled(bool enabled)
        {
            Q_UNUSED(enabled);
            this->setReadOnly(!enabled);
            if (!enabled)
            {
                this->setValue(CFsdSetup());
            }
        }

        void CFsdSetupForm::resetToDefaultValues()
        {
            ui->cb_AircraftPartsReceive->setChecked(true);
            ui->cb_AircraftPartsSend->setChecked(true);
            ui->cb_FastPositionReceive->setChecked(true);
            ui->cb_FastPositionSend->setChecked(true);
            ui->le_TextCodec->setText("latin1");
        }
    } // ns
} // ns
