// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "setuploadingdialog.h"
#include "blackgui/components/copymodelsfromotherswiftversionsdialog.h"
#include "ui_setuploadingdialog.h"
#include "blackgui/guiapplication.h"
#include "blackcore/setupreader.h"
#include "blackmisc/swiftdirectories.h"

#include <QPushButton>
#include <QDesktopServices>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackGui::Components
{
    CSetupLoadingDialog::CSetupLoadingDialog(QWidget *parent) : QDialog(parent),
                                                                ui(new Ui::CSetupLoadingDialog)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");

        ui->setupUi(this);

        const QString bootstrapPath = CSwiftDirectories::bootstrapResourceFilePath();
        ui->lbl_ownBootstrap->setText("Your boostrap.json file is available at <a href=" + bootstrapPath + ">" + bootstrapPath + "</>");

        // hide unnecessary details
        ui->comp_Messages->hideFilterBar();
        ui->comp_Messages->showDetails(false);
    }
    CSetupLoadingDialog::CSetupLoadingDialog(const CStatusMessageList &msgs, QWidget *parent) : CSetupLoadingDialog(parent)
    {
        ui->comp_Messages->appendStatusMessagesToList(msgs);
    }

    CSetupLoadingDialog::~CSetupLoadingDialog()
    {}
} // ns
