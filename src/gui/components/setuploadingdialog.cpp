// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "setuploadingdialog.h"

#include <QDesktopServices>
#include <QPushButton>

#include "ui_setuploadingdialog.h"

#include "core/setupreader.h"
#include "gui/components/copymodelsfromotherswiftversionsdialog.h"
#include "gui/guiapplication.h"
#include "misc/swiftdirectories.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::core;
using namespace swift::core::data;

namespace swift::gui::components
{
    CSetupLoadingDialog::CSetupLoadingDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CSetupLoadingDialog)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");

        ui->setupUi(this);

        const QString bootstrapPath = CSwiftDirectories::bootstrapResourceFilePath();
        ui->lbl_ownBootstrap->setText("Your boostrap.json file is available at <a href=" + bootstrapPath + ">" +
                                      bootstrapPath + "</>");

        // hide unnecessary details
        ui->comp_Messages->hideFilterBar();
        ui->comp_Messages->showDetails(false);
    }
    CSetupLoadingDialog::CSetupLoadingDialog(const CStatusMessageList &msgs, QWidget *parent)
        : CSetupLoadingDialog(parent)
    {
        ui->comp_Messages->appendStatusMessagesToList(msgs);
    }

    CSetupLoadingDialog::~CSetupLoadingDialog() {}
} // namespace swift::gui::components
