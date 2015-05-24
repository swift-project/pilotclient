/* Copyright (C) 2015
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "commandinput.h"
#include <QDateTime>

using namespace BlackMisc;

namespace BlackGui
{

    CCommandInput::CCommandInput(QWidget *parent) :
        QLineEdit(parent)
    {
        connect(this, &CCommandInput::returnPressed, this, &CCommandInput::validateCommand);
    }

    void CCommandInput::validateCommand()
    {
        QString commandLine = text();
        setText(QStringLiteral(""));

        if (commandLine.startsWith('.'))
        {
            emit commandEntered(commandLine, commandInputOriginator());
        }
    }

    COriginator CCommandInput::commandInputOriginator()
    {
        if (m_originator.getName().isEmpty())
            m_originator =  COriginator(QStringLiteral("COMMANDINPUT"));

        return m_originator;
    }

}

