// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/filters/statusmessagefilterdialog.h"
#include "blackgui/filters/statusmessagefilterbar.h"
#include "blackmisc/statusmessagelist.h"
#include "ui_statusmessagefilterdialog.h"

using namespace BlackGui::Models;
using namespace BlackGui::Filters;
using namespace BlackMisc;

namespace BlackGui::Filters
{
    CStatusMessageFilterDialog::CStatusMessageFilterDialog(QWidget *parent) : CFilterDialog(parent),
                                                                              ui(new Ui::CStatusMessageFilterDialog)
    {
        ui->setupUi(this);
        this->setWindowTitle("Filter status messages");
        this->resize(480, 75);
        ui->filter_Messages->displayCount(false);
        connect(ui->filter_Messages, &CStatusMessageFilterBar::rejectDialog, this, &CStatusMessageFilterDialog::reject);
    }

    CStatusMessageFilterDialog::~CStatusMessageFilterDialog()
    {}

    std::unique_ptr<IModelFilter<CStatusMessageList>> CStatusMessageFilterDialog::createModelFilter() const
    {
        return ui->filter_Messages->createModelFilter();
    }
} // namespace
