/* Copyright (C) 2015
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/commandinput.h"

class QWidget;

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CCommandInput::CCommandInput(QWidget *parent) :
            QLineEdit(parent),
            CIdentifiable(this)
        {
            connect(this, &CCommandInput::returnPressed, this, &CCommandInput::ps_validateCommand);
        }

        void CCommandInput::ps_validateCommand()
        {
            QString commandLine(this->text().trimmed());
            this->setText(QString());
            if (commandLine.startsWith('.'))
            {
                emit commandEntered(commandLine, identifier());
            }
        }
    } // ns
} // ns
