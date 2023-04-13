/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"

namespace BlackGui::Components
{
    CAboutDialog::CAboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CAboutDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        this->init();
    }

    CAboutDialog::~CAboutDialog()
    {}

    void CAboutDialog::init()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        ui->lbl_VersionValue->setText(sGui->getApplicationNameVersionDetailed());
        ui->pte_Info->setPlainText(sGui->getInfoString("\n"));
        ui->pte_Info->appendPlainText("\nSetup follows:\n----");
        ui->pte_Info->appendPlainText(sGui->getGlobalSetup().convertToQString("\n", true));
        ui->pte_Info->appendPlainText("\nScreen info follows:\n----");
        ui->pte_Info->appendPlainText(CGuiUtility::screenInformation("\n"));
    }
} // ns
