// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "rawfsdmessagesdialog.h"

#include "ui_rawfsdmessagesdialog.h"

namespace swift::gui::components
{
    CRawFsdMessagesDialog::CRawFsdMessagesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CRawFsdMessagesDialog)
    {
        ui->setupUi(this);
    }

    CRawFsdMessagesDialog::~CRawFsdMessagesDialog() {}
} // namespace swift::gui::components
