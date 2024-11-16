// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/filters/filterbarbuttons.h"

#include <QLineEdit>
#include <QString>
#include <QToolButton>

#include "ui_filterbarbuttons.h"

namespace swift::gui::filters
{
    CFilterBarButtons::CFilterBarButtons(QWidget *parent) : QFrame(parent), ui(new Ui::CFilterBarButtons)
    {
        ui->setupUi(this);
        connect(ui->tb_ClearForm, &QToolButton::clicked, this, &CFilterBarButtons::ps_buttonClicked);
        connect(ui->tb_Filter, &QToolButton::clicked, this, &CFilterBarButtons::ps_buttonClicked);
        connect(ui->tb_RemoveFilter, &QToolButton::clicked, this, &CFilterBarButtons::ps_buttonClicked);
        connect(ui->tb_New, &QToolButton::clicked, this, &CFilterBarButtons::ps_buttonClicked);
    }

    CFilterBarButtons::~CFilterBarButtons() {}

    void CFilterBarButtons::displayCount(bool show) { ui->le_Count->setVisible(show); }

    void CFilterBarButtons::ps_buttonClicked()
    {
        const QObject *sender = QObject::sender();
        if (sender == ui->tb_ClearForm) { emit buttonClicked(ClearForm); }
        else if (sender == ui->tb_Filter) { emit buttonClicked(Filter); }
        else if (sender == ui->tb_RemoveFilter) { emit buttonClicked(RemoveFilter); }
        else if (sender == ui->tb_New)
        {
            emit buttonClicked(RemoveFilter);
            emit buttonClicked(ClearForm);
        }
    }

    void CFilterBarButtons::onRowCountChanged(int count, bool withFilter)
    {
        QString v = QString::number(count);
        if (withFilter) { v += "F"; }
        ui->le_Count->setText(v);
    }

    void CFilterBarButtons::clickButton(CFilterBarButtons::FilterButton filterButton)
    {
        switch (filterButton)
        {
        case ClearForm: ui->tb_ClearForm->click(); break;
        case RemoveFilter: ui->tb_RemoveFilter->click(); break;
        case Filter: ui->tb_Filter->click(); break;
        default: break;
        }
    }

} // namespace swift::gui::filters
