/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "situationform.h"
#include "ui_situationform.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/stringutils.h"
#include <QDoubleValidator>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Editors
    {
        CSituationForm::CSituationForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CSituationForm)
        {
            ui->setupUi(this);

            ui->le_Bank->setValidator(new QDoubleValidator(-180.0 + CAngleUnit::deg().getEpsilon(), 180.0, 3, ui->le_Bank));
            ui->le_Pitch->setValidator(new QDoubleValidator(-180.0 + CAngleUnit::deg().getEpsilon(), 180.0, 3, ui->le_Pitch));

            connect(ui->hs_Bank, &QSlider::valueChanged, this, &CSituationForm::bankSliderChanged);
            connect(ui->hs_Pitch, &QSlider::valueChanged, this, &CSituationForm::pitchSliderChanged);
            connect(ui->le_Bank, &QLineEdit::editingFinished, this, &CSituationForm::bankEntered);
            connect(ui->le_Pitch, &QLineEdit::editingFinished, this, &CSituationForm::pitchEntered);
            connect(ui->tb_ResetBank, &QToolButton::clicked, this, &CSituationForm::resetBank);
            connect(ui->tb_ResetPitch, &QToolButton::clicked, this, &CSituationForm::resetPitch);
            connect(ui->pb_Set, &QPushButton::clicked, this, &CSituationForm::changeAircraftSituation);
            connect(ui->comp_Coordinate, &CCoordinateForm::changeCoordinate, this, &CSituationForm::changeAircraftSituation);
        }

        CSituationForm::~CSituationForm()
        { }

        void CSituationForm::setSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
        {
            ui->comp_Coordinate->setCoordinate(situation);
        }

        CAircraftSituation CSituationForm::getSituation() const
        {
            CAircraftSituation s(ui->comp_Coordinate->getCoordinate());
            s.setBank(this->getBankAngle());
            s.setPitch(this->getPitchAngle());
            return s;
        }

        CAngle CSituationForm::getBankAngle() const
        {
            return CAngle(getBankAngleDegrees(), CAngleUnit::deg());
        }

        double CSituationForm::getBankAngleDegrees() const
        {
            const QString v(ui->le_Bank->text().replace(',', '.'));
            bool ok;
            double vd = v.toDouble(&ok);
            if (!ok) { vd = 0.0; }
            return CAngle::normalizeDegrees180(vd, RoundDigits);
        }

        CAngle CSituationForm::getPitchAngle() const
        {
            return CAngle(getPitchAngleDegrees(), CAngleUnit::deg());
        }

        double CSituationForm::getPitchAngleDegrees() const
        {
            const QString v(ui->le_Pitch->text().replace(',', '.'));
            bool ok;
            double vd = v.toDouble(&ok);
            if (!ok) { vd = 0.0; }
            return CAngle::normalizeDegrees180(vd, RoundDigits);
        }

        void CSituationForm::setReadOnly(bool readonly)
        {
            ui->comp_Coordinate->setReadOnly(readonly);
        }

        void CSituationForm::setSelectOnly()
        {
            this->setReadOnly(true);
        }

        CStatusMessageList CSituationForm::validate(bool nested) const
        {
            CStatusMessageList ml;
            if (nested)
            {
                ml.push_back(ui->comp_Coordinate->validate(nested));
            }
            return ml;
        }

        void CSituationForm::showSetButton(bool visible)
        {
            ui->pb_Set->setVisible(visible);
        }

        int clampAngle(int in)
        {
            return qBound(-179, in, 180);
        }

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
    } // ns
} // ns
