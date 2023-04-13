/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "otherswiftversionsdialog.h"
#include "ui_otherswiftversionsdialog.h"

namespace BlackGui::Components
{
    COtherSwiftVersionsDialog::COtherSwiftVersionsDialog(QWidget *parent) : QDialog(parent),
                                                                            ui(new Ui::COtherSwiftVersionsDialog)
    {
        ui->setupUi(this);
    }

    COtherSwiftVersionsDialog::~COtherSwiftVersionsDialog()
    {}
} // ns
