// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "stringlistdialog.h"

#include "ui_stringlistdialog.h"

namespace swift::gui::components
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
} // namespace swift::gui::components
