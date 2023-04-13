/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "statusmessagefilterbar.h"
#include "blackmisc/logpattern.h"
#include "ui_statusmessagefilterbar.h"
#include "blackgui/models/statusmessagefilter.h"
#include <QCompleter>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Filters
{
    CStatusMessageFilterBar::CStatusMessageFilterBar(QWidget *parent) : CFilterWidget(parent),
                                                                        ui(new Ui::CStatusMessageFilterBar)
    {
        ui->setupUi(this);
        this->setButtonsAndCount(ui->filter_Buttons);
        connect(ui->le_Category, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_MessageText, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

        connect(ui->rb_Error, &QRadioButton::released, this, &CStatusMessageFilterBar::radioButtonChanged);
        connect(ui->rb_Info, &QRadioButton::released, this, &CStatusMessageFilterBar::radioButtonChanged);
        connect(ui->rb_Warning, &QRadioButton::released, this, &CStatusMessageFilterBar::radioButtonChanged);

        ui->le_Category->setCompleter(new QCompleter(CLogPattern::allHumanReadableNames(), this));

        // reset form
        this->clearForm();
    }

    void CStatusMessageFilterBar::displayCount(bool show)
    {
        ui->filter_Buttons->displayCount(show);
    }

    CStatusMessageFilterBar::~CStatusMessageFilterBar()
    {}

    void CStatusMessageFilterBar::useRadioButtonDescriptiveIcons(bool oneCharacterText)
    {
        CStatusMessage msg;
        msg.setSeverity(CStatusMessage::SeverityError);
        ui->rb_Error->setIcon(CIcon(msg.toIcon()).toQIcon());
        ui->rb_Error->setToolTip(msg.getSeverityAsString());
        ui->rb_Error->setText(oneCharacterText ? msg.getSeverityAsString().left(1) : "");

        msg.setSeverity(CStatusMessage::SeverityWarning);
        ui->rb_Warning->setIcon(CIcon(msg.toIcon()).toQIcon());
        ui->rb_Warning->setToolTip(msg.getSeverityAsString());
        ui->rb_Warning->setText(oneCharacterText ? msg.getSeverityAsString().left(1) : "");

        msg.setSeverity(CStatusMessage::SeverityInfo);
        ui->rb_Info->setIcon(CIcon(msg.toIcon()).toQIcon());
        ui->rb_Info->setToolTip(msg.getSeverityAsString());
        ui->rb_Info->setText(oneCharacterText ? msg.getSeverityAsString().left(1) : "");
    }

    std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::CStatusMessageList>> CStatusMessageFilterBar::createModelFilter() const
    {
        return std::make_unique<CStatusMessageFilter>(
            this->getSelectedSeverity(),
            ui->le_MessageText->text(),
            ui->le_Category->text());
    }

    void CStatusMessageFilterBar::onRowCountChanged(int count, bool withFilter)
    {
        ui->filter_Buttons->onRowCountChanged(count, withFilter);
    }

    void CStatusMessageFilterBar::clearForm()
    {
        ui->le_MessageText->clear();
        ui->le_Category->clear();
        ui->rb_Info->setChecked(true);
    }

    void CStatusMessageFilterBar::radioButtonChanged()
    {
        this->triggerFilter();
    }

    CStatusMessage::StatusSeverity CStatusMessageFilterBar::getSelectedSeverity() const
    {
        if (ui->rb_Error->isChecked()) { return CStatusMessage::SeverityError; }
        if (ui->rb_Warning->isChecked()) { return CStatusMessage::SeverityWarning; }
        return CStatusMessage::SeverityInfo;
    }
} // ns
