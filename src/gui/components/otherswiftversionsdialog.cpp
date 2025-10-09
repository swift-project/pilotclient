// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "otherswiftversionsdialog.h"

#include "ui_otherswiftversionsdialog.h"

namespace swift::gui::components
{
    COtherSwiftVersionsDialog::COtherSwiftVersionsDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::COtherSwiftVersionsDialog)
    {
        ui->setupUi(this);
    }

    COtherSwiftVersionsDialog::~COtherSwiftVersionsDialog() = default;
} // namespace swift::gui::components
