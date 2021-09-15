/* Copyright (C) 2017
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "copyconfigurationdialog.h"
#include "ui_copyconfigurationdialog.h"
#include "blackgui/guiapplication.h"

namespace BlackGui::Components
{
    CCopyConfigurationDialog::CCopyConfigurationDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CCopyConfigurationDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CCopyConfigurationDialog::~CCopyConfigurationDialog()
    { }

    void CCopyConfigurationDialog::setCacheMode()
    {
        ui->comp_CopyConfiguration->setCacheMode();
    }

    void CCopyConfigurationDialog::setSettingsMode()
    {
        ui->comp_CopyConfiguration->setSettingsMode();
    }

    void CCopyConfigurationDialog::selectAll()
    {
        ui->comp_CopyConfiguration->selectAll();
    }

    void CCopyConfigurationDialog::setNameFilterDisables(bool disable)
    {
        ui->comp_CopyConfiguration->setNameFilterDisables(disable);
    }

    void CCopyConfigurationDialog::setWithBootstrapFile(bool withBootstrapFile)
    {
        ui->comp_CopyConfiguration->setWithBootstrapFile(withBootstrapFile);
    }

    bool CCopyConfigurationDialog::event(QEvent *event)
    {
        if (CGuiApplication::triggerShowHelp(this, event)) { return true; }
        return QDialog::event(event);
    }
} // ns
