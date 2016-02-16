/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessageform.h"
#include "ui_statusmessageform.h"
#include <QLabel>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CStatusMessageForm::CStatusMessageForm(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CStatusMessageForm)
        {
            ui->setupUi(this);
        }

        CStatusMessageForm::~CStatusMessageForm()
        { }

        void CStatusMessageForm::setVariant(const CVariant &messageVariant)
        {
            if (!this->isVisible()) { return; }
            this->setValue(messageVariant.value<CStatusMessage>());
        }

        void CStatusMessageForm::setValue(const CStatusMessage &message)
        {
            ui->te_Message->setPlainText(message.getMessage());
            ui->lbl_SeverityIcon->setPixmap(message.toPixmap());
            const QString hrc(message.getHumanReadablePattern());
            if (hrc.isEmpty())
            {
                ui->le_Categories->setText(message.getCategories().toQString());
                ui->le_Categories->setToolTip("");
            }
            else
            {
                ui->le_Categories->setText(hrc);
                ui->le_Categories->setToolTip(message.getCategories().toQString());
            }
            ui->le_Severity->setText(message.getSeverityAsString());
            ui->le_Timestamp->setText(message.getFormattedUtcTimestampYmdhms());
        }

        void CStatusMessageForm::toggleVisibility()
        {
            this->setVisible(!isVisible());
        }
    } // ns
} // ns
