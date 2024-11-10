// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "pbhsform.h"
#include "ui_pbhsform.h"
#include "misc/stringutils.h"
#include <QDoubleValidator>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;

namespace swift::gui::editors
{
    CPbhsForm::CPbhsForm(QWidget *parent) : CForm(parent),
                                            ui(new Ui::CPbhsForm)
    {
        ui->setupUi(this);

        ui->le_Bank->setValidator(new QDoubleValidator(-180.0 + CAngleUnit::deg().getEpsilon(), 180.0, 3, ui->le_Bank));
        ui->le_Pitch->setValidator(new QDoubleValidator(-180.0 + CAngleUnit::deg().getEpsilon(), 180.0, 3, ui->le_Pitch));

        connect(ui->hs_Bank, &QSlider::valueChanged, this, &CPbhsForm::bankSliderChanged);
        connect(ui->hs_Pitch, &QSlider::valueChanged, this, &CPbhsForm::pitchSliderChanged);
        connect(ui->hs_Heading, &QSlider::valueChanged, this, &CPbhsForm::headingSliderChanged);
        connect(ui->le_Bank, &QLineEdit::editingFinished, this, &CPbhsForm::bankEntered);
        connect(ui->le_Pitch, &QLineEdit::editingFinished, this, &CPbhsForm::pitchEntered);
        connect(ui->le_Heading, &QLineEdit::editingFinished, this, &CPbhsForm::headingEntered);
        connect(ui->tb_ResetBank, &QToolButton::clicked, this, &CPbhsForm::resetBank);
        connect(ui->tb_ResetPitch, &QToolButton::clicked, this, &CPbhsForm::resetPitch);
        connect(ui->tb_ResetHeading, &QToolButton::clicked, this, &CPbhsForm::resetHeading);
        connect(ui->pb_SetOwnAircraft, &QPushButton::released, this, &CPbhsForm::changeValues);
    }

    CPbhsForm::~CPbhsForm()
    {}

    CAngle CPbhsForm::getBankAngle() const
    {
        return CAngle(getBankAngleDegrees(), CAngleUnit::deg());
    }

    void CPbhsForm::setBankAngle(const CAngle &angle)
    {
        const int value = angle.valueInteger(CAngleUnit::deg());
        ui->le_Bank->setText(QString::number(value));
        ui->hs_Bank->setValue(value);
    }

    double CPbhsForm::getBankAngleDegrees() const
    {
        const QString v(ui->le_Bank->text().replace(',', '.'));
        bool ok;
        double vd = v.toDouble(&ok);
        if (!ok) { vd = 0.0; }
        return CAngle::normalizeDegrees180(vd, RoundDigits);
    }

    CAngle CPbhsForm::getPitchAngle() const
    {
        return CAngle(getPitchAngleDegrees(), CAngleUnit::deg());
    }

    void CPbhsForm::setPitchAngle(const CAngle &angle)
    {
        const int value = angle.valueInteger(CAngleUnit::deg());
        ui->le_Pitch->setText(QString::number(value));
        ui->hs_Pitch->setValue(value);
    }

    double CPbhsForm::getPitchAngleDegrees() const
    {
        const QString v(ui->le_Pitch->text().replace(',', '.'));
        bool ok;
        double vd = v.toDouble(&ok);
        if (!ok) { vd = 0.0; }
        return CAngle::normalizeDegrees180(vd, RoundDigits);
    }

    CAngle CPbhsForm::getHeadingAngle() const
    {
        return CAngle(getHeadingAngleDegrees(), CAngleUnit::deg());
    }

    CHeading CPbhsForm::getHeading() const
    {
        return CHeading(this->getHeadingAngle(), CHeading::True);
    }

    void CPbhsForm::setHeadingAngle(const CAngle &angle)
    {
        const int value = angle.valueInteger(CAngleUnit::deg());
        ui->le_Heading->setText(QString::number(value));
        ui->hs_Heading->setValue(value);
    }

    double CPbhsForm::getHeadingAngleDegrees() const
    {
        const QString v(ui->le_Heading->text().replace(',', '.'));
        bool ok;
        double vd = v.toDouble(&ok);
        if (!ok) { vd = 0.0; }
        return CAngle::normalizeDegrees180(vd, RoundDigits);
    }

    CSpeed CPbhsForm::getGroundSpeed() const
    {
        const int gsKts = ui->sb_GsKts->value();
        return CSpeed(gsKts, CSpeedUnit::kts());
    }

    void CPbhsForm::setSituation(const CAircraftSituation &situation)
    {
        this->setBankAngle(situation.getBank());
        this->setPitchAngle(situation.getPitch());
        this->setHeadingAngle(situation.getHeading());
    }

    void CPbhsForm::updateSituation(CAircraftSituation &situation)
    {
        situation.setPitch(this->getPitchAngle());
        situation.setBank(this->getBankAngle());
        situation.setHeading(this->getHeading());
        situation.setGroundSpeed(this->getGroundSpeed());
    }

    void CPbhsForm::setReadOnly(bool readOnly)
    {
        ui->le_Bank->setReadOnly(readOnly);
        ui->le_Heading->setReadOnly(readOnly);
        ui->le_Pitch->setReadOnly(readOnly);

        ui->hs_Bank->setEnabled(!readOnly);
        ui->hs_Heading->setEnabled(!readOnly);
        ui->hs_Pitch->setEnabled(!readOnly);
    }

    void CPbhsForm::showSetButton(bool visible)
    {
        ui->pb_SetOwnAircraft->setVisible(visible);
    }

    void CPbhsForm::bankSliderChanged(int value)
    {
        const int angle = clampAngle(qRound(this->getBankAngleDegrees()));
        if (value == angle) { return; } // avoid roundtrips
        ui->le_Bank->setText(QString::number(value));
    }

    void CPbhsForm::pitchSliderChanged(int value)
    {
        const int angle = clampAngle(qRound(this->getPitchAngleDegrees()));
        if (value == angle) { return; } // avoid roundtrips
        ui->le_Pitch->setText(QString::number(value));
    }

    void CPbhsForm::headingSliderChanged(int value)
    {
        const int angle = clampAngle(qRound(this->getHeadingAngleDegrees()));
        if (value == angle) { return; } // avoid roundtrips
        ui->le_Heading->setText(QString::number(value));
    }

    void CPbhsForm::bankEntered()
    {
        const double ad = this->getBankAngleDegrees();
        QString n = QString::number(ad, 'g', 3 + RoundDigits);
        if (ui->le_Pitch->validator()) { dotToLocaleDecimalPoint(n); }
        ui->le_Bank->setText(n);
        const int angle = clampAngle(qRound(ad));
        if (angle == ui->hs_Bank->value()) { return; } // avoid roundtrips
        ui->hs_Bank->setValue(angle);
    }

    void CPbhsForm::resetBank()
    {
        ui->le_Bank->setText("0");
        ui->hs_Bank->setValue(0);
    }

    void CPbhsForm::pitchEntered()
    {
        const double ad = this->getPitchAngleDegrees();
        QString n = QString::number(ad, 'g', 3 + RoundDigits);
        if (ui->le_Pitch->validator()) { dotToLocaleDecimalPoint(n); }
        ui->le_Pitch->setText(n);
        const int angle = clampAngle(qRound(ad));
        if (angle == ui->hs_Pitch->value()) { return; } // avoid roundtrips
        ui->hs_Pitch->setValue(angle);
    }

    void CPbhsForm::resetPitch()
    {
        ui->le_Pitch->setText("0");
        ui->hs_Pitch->setValue(0);
    }

    void CPbhsForm::headingEntered()
    {
        const double ad = this->getHeadingAngleDegrees();
        QString n = QString::number(ad, 'g', 3 + RoundDigits);
        if (ui->le_Heading->validator()) { dotToLocaleDecimalPoint(n); }
        ui->le_Heading->setText(n);
        const int angle = clampAngle(qRound(ad));
        if (angle == ui->hs_Heading->value()) { return; } // avoid roundtrips
        ui->hs_Heading->setValue(angle);
    }

    void CPbhsForm::resetHeading()
    {
        ui->le_Heading->setText("0");
        ui->hs_Heading->setValue(0);
    }

} // ns
