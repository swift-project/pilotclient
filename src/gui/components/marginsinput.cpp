// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "marginsinput.h"
#include "ui_marginsinput.h"
#include "gui/stylesheetutility.h"
#include <QIntValidator>

namespace swift::gui::components
{
    CMarginsInput::CMarginsInput(QWidget *parent) : QFrame(parent),
                                                    ui(new Ui::CMarginsInput)
    {
        ui->setupUi(this);
        connect(ui->pb_Ok, &QPushButton::clicked, this, &CMarginsInput::confirmed);
        connect(ui->le_Bottom, &QLineEdit::returnPressed, this, &CMarginsInput::confirmed);
        connect(ui->le_Left, &QLineEdit::returnPressed, this, &CMarginsInput::confirmed);
        connect(ui->le_Right, &QLineEdit::returnPressed, this, &CMarginsInput::confirmed);
        connect(ui->le_Top, &QLineEdit::returnPressed, this, &CMarginsInput::confirmed);

        QIntValidator *v = new QIntValidator(0, 100, this);
        ui->le_Bottom->setValidator(v);
        ui->le_Left->setValidator(v);
        ui->le_Right->setValidator(v);
        ui->le_Top->setValidator(v);
        this->setMargins(QMargins());
    }

    CMarginsInput::~CMarginsInput()
    {}

    void CMarginsInput::setMargins(const QMargins &margins)
    {
        ui->le_Left->setText(QString::number(margins.left()));
        ui->le_Right->setText(QString::number(margins.right()));
        ui->le_Top->setText(QString::number(margins.top()));
        ui->le_Bottom->setText(QString::number(margins.bottom()));
    }

    QMargins CMarginsInput::getMargins() const
    {
        int t = 0, b = 0, l = 0, r = 0;
        const QString sl(ui->le_Left->text().trimmed());
        const QString st(ui->le_Top->text().trimmed());
        const QString sr(ui->le_Right->text().trimmed());
        const QString sb(ui->le_Bottom->text().trimmed());
        bool ok = false;
        l = sl.toInt(&ok);
        l = ok ? l : 0;
        r = sr.toInt(&ok);
        r = ok ? r : 0;
        b = sb.toInt(&ok);
        b = ok ? b : 0;
        t = st.toInt(&ok);
        t = ok ? t : 0;
        const QMargins m(l, t, r, b);
        return m;
    }

    void CMarginsInput::confirmed()
    {
        const QMargins m(this->getMargins());
        emit this->changedMargins(m);
    }

} // ns
