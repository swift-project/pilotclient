/* Copyright (C) 2015
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COMMANDINPUT_H
#define BLACKGUI_COMPONENTS_COMMANDINPUT_H

#include "blackgui/lineedithistory.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"

#include <QLineEdit>
#include <QObject>
#include <QString>

class QWidget;

namespace BlackGui
{
    namespace Components
    {
        //! Specialized line edit for command inputs
        class BLACKGUI_EXPORT CCommandInput :
            public BlackGui::CLineEditHistory,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Constructor
            CCommandInput(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CCommandInput() {}

        signals:
            //! Command was entered
            void commandEntered(const QString &command, const BlackMisc::CIdentifier &originator);

            //! Text entered (which is not a command)
            void textEntered(const QString &command, const BlackMisc::CIdentifier &originator);

        private:
            //! Basic command validation
            void validateCommand();

            //! Command tooltip
            void setCommandTooltip();
        };
    } // ns
} // ns
#endif // guard
