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

    const QString &CCommandInput::commandInputOriginator()
    {
        // string is generated once, the timestamp allows to use multiple
        // components (as long as they are not generated at the same ms)
        static const QString o = QStringLiteral("COMMANDINPUT:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
        return o;
    }

}

