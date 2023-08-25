// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "firstmodelsetdialog.h"
#include "ui_firstmodelsetdialog.h"

namespace BlackGui::Components
{
    CFirstModelSetDialog::CFirstModelSetDialog(QWidget *parent) : QDialog(parent),
                                                                  ui(new Ui::CFirstModelSetDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CFirstModelSetDialog::~CFirstModelSetDialog()
    {}
} // ns
