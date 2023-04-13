/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "rawfsdmessagesdialog.h"
#include "ui_rawfsdmessagesdialog.h"

namespace BlackGui::Components
{
    CRawFsdMessagesDialog::CRawFsdMessagesDialog(QWidget *parent) : QDialog(parent),
                                                                    ui(new Ui::CRawFsdMessagesDialog)
    {
        ui->setupUi(this);
    }

    CRawFsdMessagesDialog::~CRawFsdMessagesDialog()
    {}
} // ns
