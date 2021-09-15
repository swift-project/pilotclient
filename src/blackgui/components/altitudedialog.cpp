/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "altitudedialog.h"
#include "ui_altitudedialog.h"

#include "blackmisc/statusmessagelist.h"
#include "blackmisc/aviation/altitude.h"

#include <QValidator>
#include <QPushButton>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui::Components
{
    CAltitudeDialog::CAltitudeDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CAltitudeDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

        /** override default button, not working
        QPushButton *okBtn = ui->bb_AltitudeDialog->button(QDialogButtonBox::Ok);
        okBtn->setAutoDefault(false);
        okBtn->setDefault(false);
        QPushButton *caBtn = ui->bb_AltitudeDialog->button(QDialogButtonBox::Cancel);
        caBtn->setAutoDefault(false);
        caBtn->setDefault(false);
        **/
        ui->le_FLft->setFocus();

        // levels
        ui->le_FLft->setValidator(new QIntValidator(10, 999, ui->le_FLft));
        ui->le_AltitudeAFt->setValidator(new QIntValidator(10, 999, ui->le_AltitudeAFt));
        ui->le_AltitudeMm->setValidator(new QIntValidator(10, 9999, ui->le_AltitudeMm));
        ui->le_Sm->setValidator(new QIntValidator(10, 9999, ui->le_Sm));

        // plain altitudes in ft/m
        ui->le_AltitudeConvertedFt->setValidator(new QIntValidator(100, 99999, ui->le_AltitudeConvertedFt));
        ui->le_AltitudeFt->setValidator(new QIntValidator(100, 99999, ui->le_AltitudeFt));
        ui->le_AltitudeM->setValidator(new QIntValidator(100, 99999, ui->le_AltitudeM));

        connect(ui->le_AltitudeAFt,         &QLineEdit::editingFinished, this, &CAltitudeDialog::onEditFinished);
        connect(ui->le_AltitudeConvertedFt, &QLineEdit::editingFinished, this, &CAltitudeDialog::onEditFinished);
        connect(ui->le_AltitudeFt,          &QLineEdit::editingFinished, this, &CAltitudeDialog::onEditFinished);
        connect(ui->le_AltitudeM,           &QLineEdit::editingFinished, this, &CAltitudeDialog::onEditFinished);
        connect(ui->le_AltitudeMm,          &QLineEdit::editingFinished, this, &CAltitudeDialog::onEditFinished);
        connect(ui->le_FLft,                &QLineEdit::editingFinished, this, &CAltitudeDialog::onEditFinished);
        connect(ui->le_Sm,                  &QLineEdit::editingFinished, this, &CAltitudeDialog::onEditFinished);

        connect(ui->rb_VFR, &QRadioButton::toggled, this, &CAltitudeDialog::onVFRSelected);
        connect(ui->rb_StringOnly, &QRadioButton::toggled, this, &CAltitudeDialog::onStringOnlySelected);
        connect(ui->cb_SimplifiedVATSIMFormat, &QCheckBox::toggled, this, &CAltitudeDialog::onSimplifiedVATSIMFormatChanged);

        connect(ui->le_AltitudeAFt,         &QLineEdit::textEdited, this, &CAltitudeDialog::onTextEdit);
        connect(ui->le_AltitudeConvertedFt, &QLineEdit::textEdited, this, &CAltitudeDialog::onTextEdit);
        connect(ui->le_AltitudeFt,          &QLineEdit::textEdited, this, &CAltitudeDialog::onTextEdit);
        connect(ui->le_AltitudeM,           &QLineEdit::textEdited, this, &CAltitudeDialog::onTextEdit);
        connect(ui->le_AltitudeMm,          &QLineEdit::textEdited, this, &CAltitudeDialog::onTextEdit);
        connect(ui->le_FLft,                &QLineEdit::textEdited, this, &CAltitudeDialog::onTextEdit);
        connect(ui->le_Sm,                  &QLineEdit::textEdited, this, &CAltitudeDialog::onTextEdit);
    }

    CAltitudeDialog::~CAltitudeDialog()
    { }

    CAltitudeDialog::Mode CAltitudeDialog::getMode() const
    {
        if (ui->rb_AltitudeAFt->isChecked())         return AltitudeInHundredsOfFeet;
        if (ui->rb_VFR->isChecked())                 return VFR;
        if (ui->rb_AltitudeConvertedFt->isChecked()) return AltitudeInMetersConvertedToFeet;
        if (ui->rb_AltitudeFt->isChecked())          return AltitudeInFeet;
        if (ui->rb_AltitudeM->isChecked())           return AltitudeInMeters;
        if (ui->rb_AltitudeMm->isChecked())          return AltitudeInTensOfMeters;
        if (ui->rb_FLft->isChecked())                return FlightFlevelInFeet;
        if (ui->rb_Sm->isChecked())                  return MetricLevelInTensOfMeters;
        if (ui->rb_StringOnly->isChecked())          return StringOnly;
        return Unknown;
    }

    void CAltitudeDialog::setVatsim(bool vatsim)
    {
        ui->cb_SimplifiedVATSIMFormat->setChecked(vatsim);
    }

    bool CAltitudeDialog::isStringOnly() const
    {
        return ui->rb_StringOnly->isChecked();
    }

    void CAltitudeDialog::onEditFinished()
    {
        const Mode mode = this->getMode();
        switch (mode)
        {
        case VFR:
            m_altitudeStr = QStringLiteral("VFR");
            break;
        case FlightFlevelInFeet:
            m_altitudeStr = u"FL" % ui->le_FLft->text();
            break;
        case MetricLevelInTensOfMeters:
            m_altitudeStr = u'S' % ui->le_Sm->text();
            break;
        case AltitudeInHundredsOfFeet:
            m_altitudeStr = u'A' % ui->le_AltitudeAFt->text();
            break;
        case AltitudeInTensOfMeters:
            m_altitudeStr = u'M' % ui->le_AltitudeMm->text();
            break;
        case AltitudeInFeet:
            m_altitudeStr = ui->le_AltitudeFt->text() % u"ft";
            break;
        case AltitudeInMeters:
            m_altitudeStr = ui->le_AltitudeM->text() % u"m";
            break;
        case AltitudeInMetersConvertedToFeet:
            m_altitudeStr = ui->le_AltitudeConvertedFt->text() % u"m";
            break;
        case StringOnly:
            m_altitudeStr = ui->le_String->text();
            return;
        case Unknown:
        default:
            return;
        }

        CStatusMessageList msgs;
        m_altitude.parseFromFpAltitudeString(m_altitudeStr, &msgs);
        if (msgs.hasErrorMessages())
        {
            ui->le_String->setText(msgs.toSingleMessage().getMessage());
            m_altitudeStr.clear();
        }
        else
        {
            if (mode == AltitudeInMetersConvertedToFeet)
            {
                const int ft = CAltitude::findAltitudeForMetricAltitude(m_altitude.valueInteger(CLengthUnit::m()));
                m_altitude = ft < 0 ?
                                m_altitude.roundedToNearest100ft(true) :
                                m_altitude = CAltitude(ft, CAltitude::MeanSeaLevel, CLengthUnit::ft());
                m_altitudeStr = m_altitude.valueRoundedWithUnit(0);
            }

            if (ui->cb_SimplifiedVATSIMFormat->isChecked())
            {
                m_altitudeStr = m_altitude.asFpVatsimAltitudeString();
            }

            ui->le_String->setText(m_altitudeStr);
        }

        this->updateStyleSheet();
    }

    void CAltitudeDialog::onVFRSelected(bool selected)
    {
        if (!selected) { return; }
        this->onEditFinished();
    }

    void CAltitudeDialog::onStringOnlySelected(bool selected)
    {
        ui->le_String->setReadOnly(!selected);
        if (!selected) { return; }

        this->onEditFinished();
        this->updateStyleSheet();
    }

    void CAltitudeDialog::onTextEdit(const QString &text)
    {
        Q_UNUSED(text);
        const QObject *sender = QObject::sender();

        if (sender == ui->le_VFR)                 { ui->rb_VFR->setChecked(true); return; }
        if (sender == ui->le_AltitudeAFt)         { ui->rb_AltitudeAFt->setChecked(true); return; }
        if (sender == ui->le_AltitudeConvertedFt) { ui->rb_AltitudeConvertedFt->setChecked(true); return; }
        if (sender == ui->le_AltitudeFt)          { ui->rb_AltitudeFt->setChecked(true); return; }
        if (sender == ui->le_AltitudeM)           { ui->rb_AltitudeM->setChecked(true); return; }
        if (sender == ui->le_AltitudeMm)          { ui->rb_AltitudeMm->setChecked(true); return; }
        if (sender == ui->le_FLft)                { ui->rb_FLft->setChecked(true); return; }
        if (sender == ui->le_Sm)                  { ui->rb_Sm->setChecked(true); return; }
    }

    void CAltitudeDialog::onSimplifiedVATSIMFormatChanged(bool checked)
    {
        ui->rb_AltitudeAFt->setEnabled(!checked);
        ui->le_AltitudeAFt->setEnabled(!checked);
        ui->rb_AltitudeM->setEnabled(!checked);
        ui->le_AltitudeM->setEnabled(!checked);
        ui->rb_AltitudeMm->setEnabled(!checked);
        ui->le_AltitudeMm->setEnabled(!checked);
        ui->rb_Sm->setEnabled(!checked);
        ui->le_Sm->setEnabled(!checked);
        ui->rb_VFR->setEnabled(!checked);

        if (checked)
        {
            m_altitudeStr = m_altitude.asFpVatsimAltitudeString();
            ui->le_String->setText(m_altitudeStr);
            ui->rb_StringOnly->setChecked(true);
        }

        this->updateStyleSheet();
    }

    void CAltitudeDialog::updateStyleSheet()
    {
        const QString ss = this->styleSheet();
        this->setStyleSheet("");
        this->setStyleSheet(ss);
    }
} // ns
