// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/labelandicon.h"

#include <QLabel>

#include "ui_labelandicon.h"

namespace swift::gui
{
    CLabelAndIcon::CLabelAndIcon(QWidget *parent) : QFrame(parent), ui(new Ui::CLabelAndIcon) { ui->setupUi(this); }

    CLabelAndIcon::~CLabelAndIcon() {}

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

} // namespace swift::gui
