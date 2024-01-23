// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "fsdsetupform.h"
#include "ui_fsdsetupform.h"
#include "blackmisc/stringutils.h"
#include "blackgui/guiutility.h"
#include <QCompleter>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui::Editors
{
    CFsdSetupForm::CFsdSetupForm(QWidget *parent) : CForm(parent),
                                                    ui(new Ui::CFsdSetupForm)
    {
        ui->setupUi(this);
        this->resetToDefaultValues();
        ui->cb_Override->setChecked(true);
        ui->le_TextCodec->setCompleter(new QCompleter(textCodecNames(true, true), this));
        connect(ui->cb_Override, &QCheckBox::toggled, this, &CFsdSetupForm::enabledToggled, Qt::QueuedConnection);
        connect(ui->pb_SetDefaults, &QPushButton::clicked, this, &CFsdSetupForm::resetToDefaultValues);
    }

    CFsdSetupForm::~CFsdSetupForm()
    {}

    CFsdSetup CFsdSetupForm::getValue() const
    {
        if (!this->isEnabled()) { return this->getDisabledValue(); }
        CFsdSetup s(ui->le_TextCodec->text().trimmed());
        s.setSendReceiveDetails(
            ui->cb_AircraftPartsSend->isChecked(), ui->cb_AircraftPartsReceive->isChecked(),
            ui->cb_GndFlagSend->isChecked(), ui->cb_GndFlagReceive->isChecked(),
            ui->cb_FastPositionSend->isChecked(), ui->cb_FastPositionReceive->isChecked(),
            ui->cb_VisualPositionSend->isChecked(), ui->cb_EuroscopeSimData->isChecked(), ui->cb_IcaoEquipment->isChecked());
        s.setForce3LetterAirlineCodes(ui->cb_3LetterAirlineICAO->isChecked());
        return s;
    }

    const CFsdSetup &CFsdSetupForm::getDisabledValue() const
    {
        static const CFsdSetup fsd;
        return fsd;
    }

    void CFsdSetupForm::setValue(const CFsdSetup &setup)
    {
        ui->le_TextCodec->setText(setup.getTextCodec());
        const CFsdSetup::SendReceiveDetails d = setup.getSendReceiveDetails();

        ui->cb_AircraftPartsReceive->setChecked(d & CFsdSetup::ReceiveAircraftParts);
        ui->cb_AircraftPartsSend->setChecked(d & CFsdSetup::SendAircraftParts);
        ui->cb_FastPositionReceive->setChecked(d & CFsdSetup::ReceiveInterimPositions);
        ui->cb_FastPositionSend->setChecked(d & CFsdSetup::SendInterimPositions);
        ui->cb_3LetterAirlineICAO->setChecked(setup.force3LetterAirlineCodes());
        ui->cb_EuroscopeSimData->setChecked(d & CFsdSetup::ReceiveEuroscopeSimData);
        ui->cb_IcaoEquipment->setChecked(d & CFsdSetup::SendFplWithIcaoEquipment);
    }

    void CFsdSetupForm::setAlwaysAllowOverride(bool allow)
    {
        m_alwaysAllowOverride = allow;
        if (allow)
        {
            ui->cb_Override->setEnabled(true);
            CGuiUtility::checkBoxReadOnly(ui->cb_Override, false);
        }
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
        m_visibleEnableInfo = visible;
        this->visibleEnableInfo(visible);
    }

    void CFsdSetupForm::setReadOnly(bool readonly)
    {
        ui->le_TextCodec->setReadOnly(readonly);
        ui->pb_SetDefaults->setEnabled(!readonly);
        CGuiUtility::checkBoxesReadOnly(this, readonly);
        if (m_alwaysAllowOverride)
        {
            ui->cb_Override->setEnabled(true);
            CGuiUtility::checkBoxReadOnly(ui->cb_Override, false);
        }

        /**
        if (readonly && ui->cb_Override->isChecked())
        {
            // this is no value which will be stored
            ui->cb_Override->setChecked(false);
        }
        **/

        this->forceStyleSheetUpdate();
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
    }

    void CFsdSetupForm::visibleEnableInfo(bool visible)
    {
        ui->cb_Override->setVisible(visible);
        ui->lbl_FsdSetup->setVisible(visible);
        ui->pb_SetDefaults->setVisible(visible);
    }

    void CFsdSetupForm::resetToDefaultValues()
    {
        CFsdSetup s;
        this->setValue(s);
    }
} // ns
