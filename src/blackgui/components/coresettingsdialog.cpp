/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "coresettingsdialog.h"
#include "ui_coresettingsdialog.h"
#include "blackgui/guiapplication.h"

namespace BlackGui
{
    namespace Components
    {
        CCoreSettingsDialog::CCoreSettingsDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CCoreSettingsDialog)
        {
            Q_ASSERT(sGui);
            ui->setupUi(this);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        }

        CCoreSettingsDialog::~CCoreSettingsDialog()
        { }
    } // ns
} // ns
