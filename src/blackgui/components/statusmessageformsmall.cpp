/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/statusmessageformsmall.h"
#include "ui_statusmessageformsmall.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CStatusMessageFormSmall::CStatusMessageFormSmall(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CStatusMessageFormSmall)
        {
            ui->setupUi(this);
        }

        CStatusMessageFormSmall::~CStatusMessageFormSmall()
        { }

        void CStatusMessageFormSmall::setVariant(const CVariant &messageVariant)
        {
            if (!this->isVisible()) { return; }
            this->setValue(messageVariant.value<CStatusMessage>());
        }

        void CStatusMessageFormSmall::setValue(const CStatusMessage &message)
        {
            ui->te_Message->setPlainText(message.getMessage());
            ui->lbl_SeverityIcon->setPixmap(CIcon(message.toIcon()));
            ui->le_Categories->setText(message.getHumanOrTechnicalCategoriesAsString());
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
} // ns
