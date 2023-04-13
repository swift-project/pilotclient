/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
