// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/statusmessageformsmall.h"
#include "blackmisc/logpattern.h"
#include "ui_statusmessageformsmall.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>

using namespace BlackMisc;

namespace BlackGui::Components
{
    CStatusMessageFormSmall::CStatusMessageFormSmall(QWidget *parent) : QFrame(parent),
                                                                        ui(new Ui::CStatusMessageFormSmall)
    {
        ui->setupUi(this);
    }

    CStatusMessageFormSmall::~CStatusMessageFormSmall()
    {}

    void CStatusMessageFormSmall::setVariant(const CVariant &messageVariant)
    {
        if (!this->isVisible()) { return; }
        this->setValue(messageVariant.value<CStatusMessage>());
    }

    void CStatusMessageFormSmall::setValue(const CStatusMessage &message)
    {
        ui->te_Message->setPlainText(message.getMessage());
        ui->lbl_SeverityIcon->setPixmap(CIcon(message.toIcon()));
        ui->le_Categories->setText(CLogPattern::humanOrTechnicalCategoriesFrom(message).join(", "));
        ui->le_Severity->setText(message.getSeverityAsString());
        ui->le_Timestamp->setText(message.getFormattedUtcTimestampYmdhms());
    }

    void CStatusMessageFormSmall::toggleVisibility()
    {
        this->setVisible(!isVisible());
    }

    void CStatusMessageFormSmall::setReducedInfo(bool minimized)
    {
        ui->le_Timestamp->setVisible(!minimized);
        ui->lbl_Timestamp->setVisible(!minimized);
        ui->le_Categories->setVisible(!minimized);
        ui->lbl_Categories->setVisible(!minimized);
        ui->le_Severity->setVisible(!minimized);
        ui->lbl_SeverityIcon->setVisible(!minimized);
        ui->lbl_Severity->setVisible(!minimized);
    }
} // ns
