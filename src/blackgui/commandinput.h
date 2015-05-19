/* Copyright (C) 2015
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMMANDINPUT_H
#define BLACKGUI_COMMANDINPUT_H

#include "blackguiexport.h"
#include <QLineEdit>
#include <QString>

namespace BlackGui
{

    //! Specialized LineEdit for command inputs
    class BLACKGUI_EXPORT CCommandInput : public QLineEdit
    {
        Q_OBJECT

    public:

        //! Constructor
        CCommandInput(QWidget *parent = nullptr);

        //! Destructor
        ~CCommandInput() {}

    signals:

        //! Command was entered
        void commandEntered(const QString &command, const QString &originator);

    private slots:

        //! Basic command validation
        void validateCommand();

    private:

        const QString &commandInputOriginator();
    };

}

#endif // CCOMMANDINPUT_H
