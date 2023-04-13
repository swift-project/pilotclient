/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/labelandicon.h"
#include "ui_labelandicon.h"

#include <QLabel>

namespace BlackGui
{
    CLabelAndIcon::CLabelAndIcon(QWidget *parent) : QFrame(parent),
                                                    ui(new Ui::CLabelAndIcon)
    {
        ui->setupUi(this);
    }

    CLabelAndIcon::~CLabelAndIcon()
    {}

    void CLabelAndIcon::set(const QPixmap &pixmap, const QString &text)
    {
        ui->lbl_Left->setPixmap(pixmap);
        ui->lbl_Right->setText(text);
    }

    void CLabelAndIcon::set(const QString &text, const QPixmap &pixmap)
    {
        ui->lbl_Right->setPixmap(pixmap);
        ui->lbl_Left->setText(text);
    }

} // ns
