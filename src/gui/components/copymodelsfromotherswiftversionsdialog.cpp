// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "copymodelsfromotherswiftversionsdialog.h"

#include "ui_copymodelsfromotherswiftversionsdialog.h"

namespace swift::gui::components
{
    CCopyModelsFromOtherSwiftVersionsDialog::CCopyModelsFromOtherSwiftVersionsDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::CCopyModelsFromOtherSwiftVersionsDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CCopyModelsFromOtherSwiftVersionsDialog::~CCopyModelsFromOtherSwiftVersionsDialog() {}
} // namespace swift::gui::components
