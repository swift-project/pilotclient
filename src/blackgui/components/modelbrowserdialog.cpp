/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "modelbrowserdialog.h"
#include "ui_modelbrowserdialog.h"
#include "blackgui/guiapplication.h"

namespace BlackGui::Components
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

} // ns
