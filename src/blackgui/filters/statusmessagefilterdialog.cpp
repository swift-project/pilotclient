/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
