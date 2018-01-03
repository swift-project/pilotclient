/* Copyright (C) 2015
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/commandinput.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextapplication.h"
#include "blackmisc/simplecommandparser.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CCommandInput::CCommandInput(QWidget *parent) :
            CLineEditHistory(parent),
            CIdentifiable(this)
        {
            if (this->placeholderText().isEmpty())
            {
                this->setPlaceholderText(".dot commands");
            }

            connect(this, &CCommandInput::returnPressed, this, &CCommandInput::validateCommand);

            // set tooltip: shorty after, and later when application is initialized
            QTimer::singleShot(5000, this, &CCommandInput::setCommandTooltip);
            QTimer::singleShot(30000, this, &CCommandInput::setCommandTooltip);
        }

        void CCommandInput::validateCommand()
        {
            const QString c(this->getLastEnteredLineFormatted());
            if (c.isEmpty()) { return; }
            if (c.startsWith('.'))
            {
                if (c.toLower().contains("help"))
                {
                    this->setCommandTooltip();
                    return;
                }
                emit this->commandEntered(c, this->identifier());
            }
            else
            {
                emit this->textEntered(c, this->identifier());
            }
        }

        void CCommandInput::setCommandTooltip()
        {
            const bool context = (sGui && sGui->getIContextApplication());
            this->setToolTip(context ?
                             sGui->getIContextApplication()->dotCommandsHtmlHelp() :
                             CSimpleCommandParser::commandsHtmlHelp());
        }
    } // ns
} // ns
