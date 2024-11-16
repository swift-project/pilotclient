// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "modelbrowserdialog.h"

#include "ui_modelbrowserdialog.h"

#include "gui/guiapplication.h"

namespace swift::gui::components
{
    CModelBrowserDialog::CModelBrowserDialog(QWidget *parent) : QDialog(parent),
                                                                ui(new Ui::CModelBrowserDialog)
    {
        ui->setupUi(this);
    }

    CModelBrowserDialog::~CModelBrowserDialog()
    {
        // void;
    }

    bool CModelBrowserDialog::event(QEvent *event)
    {
        if (CGuiApplication::triggerShowHelp(this, event)) { return true; }
        return QDialog::event(event);
    }

    void CModelBrowserDialog::done(int r)
    {
        ui->comp_ModelBrowser->close();
        QDialog::done(r);
    }

} // namespace swift::gui::components
