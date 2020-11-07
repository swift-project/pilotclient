/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/statusmessageform.h"
#include "blackmisc/logpattern.h"
#include "ui_statusmessageform.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QString>

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
            ui->lbl_SeverityIcon->setPixmap(CIcon(message.toIcon()));
            const QString hrc(CLogPattern::humanReadableNamesFrom(message).join(", "));
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

        void CStatusMessageForm::clear()
        {
            ui->le_Timestamp->clear();
            ui->le_Categories->clear();
            ui->le_Severity->clear();
        }

        void CStatusMessageForm::toggleVisibility()
        {
            this->setVisible(!isVisible());
        }

        void CStatusMessageForm::setReducedInfo(bool reduced)
        {
            ui->le_Timestamp->setVisible(!reduced);
            ui->lbl_Timestamp->setVisible(!reduced);
            ui->le_Categories->setVisible(!reduced);
            ui->lbl_Categories->setVisible(!reduced);
            ui->le_Severity->setVisible(!reduced);
            ui->lbl_SeverityIcon->setVisible(!reduced);
            ui->lbl_Severity->setVisible(!reduced);
        }
    } // ns
} // ns
