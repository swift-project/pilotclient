// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "autopublishdialog.h"

#include "ui_autopublishdialog.h"

#include "gui/guiapplication.h"

namespace swift::gui::components
{
    CAutoPublishDialog::CAutoPublishDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CAutoPublishDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CAutoPublishDialog::~CAutoPublishDialog() {}

    int CAutoPublishDialog::readFiles() { return ui->comp_AutoPublish->readFiles(); }

    int CAutoPublishDialog::readAndShow()
    {
        const int r = ui->comp_AutoPublish->readFiles();
        this->show();
        CGuiApplication::modalWindowToFront();
        return r;
    }
} // namespace swift::gui::components
