/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "scalescreenfactor.h"
#include "ui_scalescreenfactor.h"
#include <QIntValidator>

namespace BlackGui
{
    namespace Components
    {
        CScaleScreenFactor::CScaleScreenFactor(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CScaleScreenFactor)
        {
            ui->setupUi(this);
            this->setMinMax(50, 150);

            connect(ui->hs_Factor, &QSlider::valueChanged, this, &CScaleScreenFactor::onSliderChanged);
            connect(ui->le_Factor, &QLineEdit::editingFinished, this, &CScaleScreenFactor::onEditFinished);
        }

        CScaleScreenFactor::~CScaleScreenFactor()
        { }

        void CScaleScreenFactor::setMinMax(int min, int max)
        {
            ui->hs_Factor->setMinimum(min);
            ui->hs_Factor->setMaximum(max);
            ui->le_Factor->setValidator(new QIntValidator(min, max, ui->le_Factor));

            ui->le_Factor->setToolTip(QStringLiteral("%1-%2").arg(min).arg(max));
            ui->le_Factor->setPlaceholderText(QStringLiteral("%1-%2").arg(min).arg(max));
            ui->hs_Factor->setToolTip(QStringLiteral("%1-%2").arg(min, max));

            const int v = (min + max) / 2;
            ui->hs_Factor->setValue(v);
            ui->le_Factor->setText(QString::number(v));
        }

        qreal CScaleScreenFactor::getScaleFactor() const
        {
            return 0.01 * ui->hs_Factor->value();
        }

        QString CScaleScreenFactor::getScaleFactorAsString() const
        {
            const QString sf = QString::number(this->getScaleFactor(), 'f', 2);
            return sf;
        }

        void CScaleScreenFactor::onSliderChanged(int value)
        {
            const QString v = QString::number(value);
            if (ui->le_Factor->text() == v) { return; } // avoid signal roundtrips
            ui->le_Factor->setText(v);
        }

        void CScaleScreenFactor::onEditFinished()
        {
            const QString v =  ui->le_Factor->text();
            if (v.isEmpty()) { return; }
            bool ok;
            const int value = v.toInt(&ok);
            if (!ok) { return; }
            if (ui->hs_Factor->value() == value) { return; } // avoid signal roundtrips
            ui->hs_Factor->setValue(value);
        }
    } // ns
} // ns
