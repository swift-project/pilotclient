/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "texteditdialog.h"
#include "ui_texteditdialog.h"

namespace BlackGui
{
    namespace Components
    {
        CTextEditDialog::CTextEditDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CTextEditDialog)
        {
            ui->setupUi(this);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        }

        CTextEditDialog::~CTextEditDialog()
        { }

        QTextEdit *CTextEditDialog::textEdit() const
        {
            return ui->te_TextEdit;
        }
    } // ns
} // ns
