// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "scalescreenfactor.h"
#include "ui_scalescreenfactor.h"
#include "blackgui/guiutility.h"

#include <QDesktopWidget>
#include <QScreen>
#include <QIntValidator>

namespace BlackGui::Components
{
    CScaleScreenFactor::CScaleScreenFactor(QWidget *parent) : QFrame(parent),
                                                              ui(new Ui::CScaleScreenFactor)
    {
        ui->setupUi(this);
        this->setMinMax(50, 150);

        connect(ui->hs_Factor, &QSlider::valueChanged, this, &CScaleScreenFactor::onSliderChanged);
        connect(ui->le_Factor, &QLineEdit::editingFinished, this, &CScaleScreenFactor::onEditFinished);

        const QDesktopWidget *w = qApp->desktop();
        const QString tt = QStringLiteral(
                               "Scaling only works on High DPI screens\n"
                               "Width/height: %1/%2 Ratio: %3\n"
                               "Logical DPI x/y: %4/%5\n"
                               "Physical DPI x/y: %6/%7\n"
                               "width/height MM: %8/%9")
                               .arg(w->width())
                               .arg(w->height())
                               .arg(QString::number(CGuiUtility::mainApplicationWidgetPixelRatio(), 'f', 2))
                               .arg(w->logicalDpiX())
                               .arg(w->logicalDpiY())
                               .arg(w->physicalDpiX())
                               .arg(w->physicalDpiY())
                               .arg(w->widthMM())
                               .arg(w->heightMM());

        this->setToolTip(tt);
    }

    CScaleScreenFactor::~CScaleScreenFactor()
    {}

    void CScaleScreenFactor::setMinMax(int min, int max)
    {
        ui->hs_Factor->setMinimum(min);
        ui->hs_Factor->setMaximum(max);
        ui->le_Factor->setValidator(new QIntValidator(min, max, ui->le_Factor));

        const QString tt = QStringLiteral("%1-%2").arg(min).arg(max);
        ui->le_Factor->setToolTip(tt);
        ui->le_Factor->setPlaceholderText(tt);
        ui->hs_Factor->setToolTip(tt);

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
        const QString v = ui->le_Factor->text();
        if (v.isEmpty()) { return; }
        bool ok;
        const int value = v.toInt(&ok);
        if (!ok) { return; }
        if (ui->hs_Factor->value() == value) { return; } // avoid signal roundtrips
        ui->hs_Factor->setValue(value);
    }
} // ns
