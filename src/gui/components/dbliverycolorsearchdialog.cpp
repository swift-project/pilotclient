// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbliverycolorsearchdialog.h"

#include "ui_dbliverycolorsearchdialog.h"

#include "core/webdataservices.h"
#include "gui/guiapplication.h"
#include "misc/aviation/liverylist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CDbLiveryColorSearchDialog::CDbLiveryColorSearchDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::CDbLiveryColorSearchDialog)
    {
        ui->setupUi(this);
        connect(this, &CDbLiveryColorSearchDialog::accepted, this, &CDbLiveryColorSearchDialog::onAccepted);
    }

    CDbLiveryColorSearchDialog::~CDbLiveryColorSearchDialog() = default;

    const CLivery &CDbLiveryColorSearchDialog::getLivery() const { return m_foundLivery; }

    void CDbLiveryColorSearchDialog::onAccepted() { m_foundLivery = ui->comp_LiverySearch->getLivery(); }
} // namespace swift::gui::components
