/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "afvmapdialog.h"
#include "ui_afvmapdialog.h"

namespace BlackGui
{
    namespace Components
    {
        CAfvMapDialog::CAfvMapDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CAfvMapDialog)
        {
            ui->setupUi(this);
        }

        CAfvMapDialog::~CAfvMapDialog() { }
    } // ns
} // ns
