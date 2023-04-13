/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "stringlistdialog.h"
#include "ui_stringlistdialog.h"

namespace BlackGui::Components
{
    CStringListDialog::CStringListDialog(QWidget *parent) : QDialog(parent),
                                                            ui(new Ui::CStringListDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        ui->lw_StringList->setSelectionMode(QAbstractItemView::SingleSelection);
    }

    CStringListDialog::~CStringListDialog()
    {}

    void CStringListDialog::setStrings(const QStringList &strings)
    {
        ui->lw_StringList->clear();
        ui->lw_StringList->addItems(strings);
    }

    QString CStringListDialog::getSelectedValue() const
    {
        const QList<QListWidgetItem *> selectedItems = ui->lw_StringList->selectedItems();
        if (selectedItems.isEmpty()) { return {}; }
        return selectedItems.front()->text();
    }
} // ns
