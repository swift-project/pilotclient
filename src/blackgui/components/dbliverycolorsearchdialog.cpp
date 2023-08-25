// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbliverycolorsearchdialog.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/liverylist.h"
#include "ui_dbliverycolorsearchdialog.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui::Components
{
    CDbLiveryColorSearchDialog::CDbLiveryColorSearchDialog(QWidget *parent) : QDialog(parent),
                                                                              ui(new Ui::CDbLiveryColorSearchDialog)
    {
        ui->setupUi(this);
        connect(this, &CDbLiveryColorSearchDialog::accepted, this, &CDbLiveryColorSearchDialog::onAccepted);
    }

    CDbLiveryColorSearchDialog::~CDbLiveryColorSearchDialog()
    {}

    const CLivery &CDbLiveryColorSearchDialog::getLivery() const
    {
        return m_foundLivery;
    }

    void CDbLiveryColorSearchDialog::onAccepted()
    {
        m_foundLivery = ui->comp_LiverySearch->getLivery();
    }
} // ns
