/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessageformsmall.h"
#include "ui_statusmessageformsmall.h"
#include <QLabel>

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
            ui->lbl_SeverityIcon->setPixmap(message.toPixmap());
            ui->le_Categories->setText(message.getHumanOrTechnicalCategoriesAsString());
            ui->le_Severity->setText(message.getSeverityAsString());
            ui->le_Timestamp->setText(message.getFormattedUtcTimestampYmdhms());
        }

        void CStatusMessageFormSmall::toggleVisibility()
        {
            this->setVisible(!isVisible());
        }
    } // ns
} // ns
