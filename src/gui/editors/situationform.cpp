// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "situationform.h"

#include <QDoubleValidator>

#include "ui_situationform.h"

#include "core/context/contextownaircraft.h"
#include "gui/guiapplication.h"
#include "misc/logmessage.h"
#include "misc/pq/angle.h"
#include "misc/pq/pressure.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;

namespace swift::gui::editors
{
    CSituationForm::CSituationForm(QWidget *parent) : CForm(parent), ui(new Ui::CSituationForm)
    {
        ui->setupUi(this);

        ui->le_Bank->setValidator(new QDoubleValidator(-180.0 + CAngleUnit::deg().getEpsilon(), 180.0, 3, ui->le_Bank));
        ui->le_Pitch->setValidator(
            new QDoubleValidator(-180.0 + CAngleUnit::deg().getEpsilon(), 180.0, 3, ui->le_Pitch));
        ui->le_Pressure->setValidator(new QDoubleValidator(980.0, 1046.0, 2, ui->le_Pressure));

        connect(ui->hs_Bank, &QSlider::valueChanged, this, &CSituationForm::bankSliderChanged);
        connect(ui->hs_Pitch, &QSlider::valueChanged, this, &CSituationForm::pitchSliderChanged);
        connect(ui->hs_Heading, &QSlider::valueChanged, this, &CSituationForm::headingSliderChanged);
        connect(ui->hs_Pressure, &QSlider::valueChanged, this, &CSituationForm::pressureSliderChanged);
        connect(ui->le_Bank, &QLineEdit::editingFinished, this, &CSituationForm::bankEntered);
        connect(ui->le_Pitch, &QLineEdit::editingFinished, this, &CSituationForm::pitchEntered);
        connect(ui->le_Pressure, &QLineEdit::editingFinished, this, &CSituationForm::pressureEntered);
        connect(ui->le_Heading, &QLineEdit::editingFinished, this, &CSituationForm::headingEntered);

        connect(ui->tb_ResetBank, &QToolButton::clicked, this, &CSituationForm::resetBank);
        connect(ui->tb_ResetPitch, &QToolButton::clicked, this, &CSituationForm::resetPitch);
        connect(ui->tb_ResetHeading, &QToolButton::clicked, this, &CSituationForm::resetHeading);
        connect(ui->tb_ResetPressure, &QToolButton::clicked, this, &CSituationForm::resetPressure);
        connect(ui->pb_SetOwnAircraft, &QPushButton::released, this, &CSituationForm::changeAircraftSituation);
        connect(ui->pb_SetEnvironment, &QPushButton::released, this, &CSituationForm::changeAircraftSituation);
        connect(ui->pb_PresetOwnAircraft, &QPushButton::released, this, &CSituationForm::presetOwnAircraftSituation);
        connect(ui->comp_Coordinate, &CCoordinateForm::changedCoordinate, this, &CSituationForm::onCoordinateChanged);
    }

    CSituationForm::~CSituationForm() = default;

    void CSituationForm::setSituation(const swift::misc::aviation::CAircraftSituation &situation)
    {
        ui->comp_Coordinate->setCoordinate(situation);
        this->bankSliderChanged(situation.getBank().valueInteger(CAngleUnit::deg()));
        this->pitchSliderChanged(situation.getPitch().valueInteger(CAngleUnit::deg()));
        this->headingSliderChanged(situation.getHeading().valueInteger(CAngleUnit::deg()));
    }

    CAircraftSituation CSituationForm::getSituation() const
    {
        const CCoordinateGeodetic position = ui->comp_Coordinate->getCoordinate();
        const CAltitude pressureAltitude(
            position.geodeticHeight().toPressureAltitude(this->getBarometricPressureMsl()));

        CAircraftSituation s(position);
        s.setBank(this->getBankAngle());
        s.setPitch(this->getPitchAngle());
        s.setHeading(CHeading(this->getHeadingAngle(), CHeading::True));
        s.setGroundSpeed(this->getGroundSpeed());

        if (!pressureAltitude.isNull() && pressureAltitude.getAltitudeType() == CAltitude::PressureAltitude)
        {
            s.setPressureAltitude(pressureAltitude);
        }
        return s;
    }

    CAngle CSituationForm::getBankAngle() const { return CAngle(getBankAngleDegrees(), CAngleUnit::deg()); }

    double CSituationForm::getBankAngleDegrees() const
    {
        const QString v(ui->le_Bank->text().replace(',', '.'));
        bool ok;
        double vd = v.toDouble(&ok);
        if (!ok) { vd = 0.0; }
        return CAngle::normalizeDegrees180(vd, RoundDigits);
    }

    CAngle CSituationForm::getPitchAngle() const { return CAngle(getPitchAngleDegrees(), CAngleUnit::deg()); }

    double CSituationForm::getPitchAngleDegrees() const
    {
        const QString v(ui->le_Pitch->text().replace(',', '.'));
        bool ok;
        double vd = v.toDouble(&ok);
        if (!ok) { vd = 0.0; }
        return CAngle::normalizeDegrees180(vd, RoundDigits);
    }

    CAngle CSituationForm::getHeadingAngle() const { return CAngle(getHeadingAngleDegrees(), CAngleUnit::deg()); }

    double CSituationForm::getHeadingAngleDegrees() const
    {
        const QString v(ui->le_Heading->text().replace(',', '.'));
        bool ok;
        double vd = v.toDouble(&ok);
        if (!ok) { vd = 0.0; }
        return CAngle::normalizeDegrees180(vd, RoundDigits);
    }

    double CSituationForm::getBarometricPressureMslMillibar() const
    {
        const QString v(ui->le_Pressure->text().replace(',', '.'));
        bool ok;
        double vd = v.toDouble(&ok);
        if (!ok) { vd = CAltitude::standardISASeaLevelPressure().value(CPressureUnit::mbar()); }
        return vd;
    }

    CPressure CSituationForm::getBarometricPressureMsl() const
    {
        return CPressure(this->getBarometricPressureMslMillibar(), CPressureUnit::mbar());
    }

    CSpeed CSituationForm::getGroundSpeed() const
    {
        const int gsKts = ui->sb_GsKts->value();
        return CSpeed(gsKts, CSpeedUnit::kts());
    }

    void CSituationForm::setReadOnly(bool readonly)
    {
        ui->comp_Coordinate->setReadOnly(readonly);

        ui->le_Bank->setReadOnly(readonly);
        ui->le_Heading->setReadOnly(readonly);
        ui->le_Pitch->setReadOnly(readonly);
        ui->le_Pressure->setReadOnly(readonly);

        ui->hs_Bank->setEnabled(!readonly);
        ui->hs_Heading->setEnabled(!readonly);
        ui->hs_Pitch->setEnabled(!readonly);
        ui->hs_Pressure->setEnabled(!readonly);

        this->forceStyleSheetUpdate();
    }

    void CSituationForm::setSelectOnly() { this->setReadOnly(true); }

    CStatusMessageList CSituationForm::validate(bool nested) const
    {
        CStatusMessageList ml;
        if (nested) { ml.push_back(ui->comp_Coordinate->validate(nested)); }
        return ml;
    }

    void CSituationForm::showSetButton(bool visible) { ui->pb_SetOwnAircraft->setVisible(visible); }

    int clampAngle(int in) { return qBound(-179, in, 180); }

    void CSituationForm::bankSliderChanged(int value)
    {
        const int angle = clampAngle(qRound(this->getBankAngleDegrees()));
        if (value == angle) { return; } // avoid roundtrips
        ui->le_Bank->setText(QString::number(value));
    }

    void CSituationForm::pitchSliderChanged(int value)
    {
        const int angle = clampAngle(qRound(this->getPitchAngleDegrees()));
        if (value == angle) { return; } // avoid roundtrips
        ui->le_Pitch->setText(QString::number(value));
    }

    void CSituationForm::headingSliderChanged(int value)
    {
        const int angle = clampAngle(qRound(this->getHeadingAngleDegrees()));
        if (value == angle) { return; } // avoid roundtrips
        ui->le_Heading->setText(QString::number(value));
    }

    void CSituationForm::pressureSliderChanged(int value)
    {
        const int pressure = qRound(this->getBarometricPressureMslMillibar());
        if (value == pressure) { return; } // avoid roundtrips
        ui->le_Pressure->setText(QString::number(value));
    }

    void CSituationForm::bankEntered()
    {
        const double ad = this->getBankAngleDegrees();
        QString n = QString::number(ad, 'g', 3 + RoundDigits);
        if (ui->le_Pitch->validator()) { dotToLocaleDecimalPoint(n); }
        ui->le_Bank->setText(n);
        const int angle = clampAngle(qRound(ad));
        if (angle == ui->hs_Bank->value()) { return; } // avoid roundtrips
        ui->hs_Bank->setValue(angle);
    }

    void CSituationForm::resetBank()
    {
        ui->le_Bank->setText("0");
        ui->hs_Bank->setValue(0);
    }

    void CSituationForm::pitchEntered()
    {
        const double ad = this->getPitchAngleDegrees();
        QString n = QString::number(ad, 'g', 3 + RoundDigits);
        if (ui->le_Pitch->validator()) { dotToLocaleDecimalPoint(n); }
        ui->le_Pitch->setText(n);
        const int angle = clampAngle(qRound(ad));
        if (angle == ui->hs_Pitch->value()) { return; } // avoid roundtrips
        ui->hs_Pitch->setValue(angle);
    }

    void CSituationForm::resetPitch()
    {
        ui->le_Pitch->setText("0");
        ui->hs_Pitch->setValue(0);
    }

    void CSituationForm::headingEntered()
    {
        const double ad = this->getHeadingAngleDegrees();
        QString n = QString::number(ad, 'g', 3 + RoundDigits);
        if (ui->le_Heading->validator()) { dotToLocaleDecimalPoint(n); }
        ui->le_Heading->setText(n);
        const int angle = clampAngle(qRound(ad));
        if (angle == ui->hs_Heading->value()) { return; } // avoid roundtrips
        ui->hs_Heading->setValue(angle);
    }

    void CSituationForm::resetHeading()
    {
        ui->le_Heading->setText("0");
        ui->hs_Heading->setValue(0);
    }

    void CSituationForm::pressureEntered()
    {
        const double pd = this->getBarometricPressureMslMillibar();
        QString n = QString::number(pd, 'g', 4 + RoundDigits);
        if (ui->le_Pressure->validator()) { dotToLocaleDecimalPoint(n); }
        ui->le_Pressure->setText(n);
        const int pi = qRound(pd);
        if (pi == ui->hs_Pressure->value()) { return; } // avoid roundtrips
        ui->hs_Pressure->setValue(pi);
    }

    void CSituationForm::resetPressure()
    {
        static const int v = CAltitude::standardISASeaLevelPressure().valueInteger(CPressureUnit::mbar());
        static const QString vs(dotToLocaleDecimalPoint(
            QString::number(CAltitude::standardISASeaLevelPressure().valueRounded(CPressureUnit::mbar(), 2))));
        ui->le_Pressure->setText(vs);
        ui->hs_Pressure->setValue(v);
    }

    void CSituationForm::presetOwnAircraftSituation()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        const CAircraftSituation s = sGui->getIContextOwnAircraft()->getOwnAircraftSituation();
        this->setSituation(s);
    }

    void CSituationForm::onCoordinateChanged() { emit this->changeAircraftSituation(); }
} // namespace swift::gui::editors
