// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "texteditdialog.h"
#include "ui_texteditdialog.h"

namespace swift::gui::components
{
    CTextEditDialog::CTextEditDialog(QWidget *parent) : QDialog(parent),
                                                        ui(new Ui::CTextEditDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CTextEditDialog::~CTextEditDialog()
    {}

    QTextEdit *CTextEditDialog::textEdit() const
    {
        return ui->te_TextEdit;
    }

    void CTextEditDialog::setReadOnly()
    {
        ui->te_TextEdit->setReadOnly(true);
    }
} // ns
