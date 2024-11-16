// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/filters/statusmessagefilterdialog.h"

#include "ui_statusmessagefilterdialog.h"

#include "gui/filters/statusmessagefilterbar.h"
#include "misc/statusmessagelist.h"

using namespace swift::gui::models;
using namespace swift::gui::filters;
using namespace swift::misc;

namespace swift::gui::filters
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
} // namespace swift::gui::filters
