/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "logcomponent.h"
#include "ui_logcomponent.h"
#include "../stylesheetutility.h"

using namespace BlackGui;

namespace BlackGui
{
    namespace Components
    {

        CLogComponent::CLogComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CLogComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_StatusMessages->setAutoResizeFrequency(3);
        }

        CLogComponent::~CLogComponent()
        {
            delete ui;
        }

        void CLogComponent::appendStatusMessageToConsole(const CStatusMessage &statusMessage)
        {
            if (statusMessage.isEmpty()) return;
            this->ui->te_StatusPageConsole->appendHtml(statusMessage.toHtml());
        }

        void CLogComponent::appendPlainTextToConsole(const QString &text)
        {
            this->ui->te_StatusPageConsole->appendPlainText(text);
        }

        void CLogComponent::appendStatusMessageToList(const CStatusMessage &statusMessage)
        {
            if (statusMessage.isEmpty()) return;
            this->ui->tvp_StatusMessages->insert(statusMessage);
        }
    }
}
