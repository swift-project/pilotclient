// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COMMANDINPUT_H
#define SWIFT_GUI_COMPONENTS_COMMANDINPUT_H

#include "gui/lineedithistory.h"
#include "gui/swiftguiexport.h"
#include "misc/digestsignal.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"

#include <QLineEdit>
#include <QObject>
#include <QString>

namespace swift::misc
{
    namespace network
    {
        class CServer;
    }
    namespace simulation
    {
        class CSimulatorPluginInfo;
    }
}
namespace swift::gui::components
{
    //! Specialized line edit for command inputs
    class SWIFT_GUI_EXPORT CCommandInput :
        public CLineEditHistory,
        public swift::misc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Constructor
        CCommandInput(QWidget *parent = nullptr);

        //! Show tooltip
        void showToolTip(bool show);

    signals:
        //! Command was entered
        void commandEntered(const QString &command, const swift::misc::CIdentifier &originator);

        //! Text entered (which is not a command)
        void textEntered(const QString &command, const swift::misc::CIdentifier &originator);

    private:
        //! Basic command validation
        void validateCommand();

        //! Command tooltip
        void setCommandToolTip();

        //! Simulator plugin loaded / unloaded (default info)
        void onSimulatorPluginChanged(const swift::misc::simulation::CSimulatorPluginInfo &info);

        //! Connected network server has been changed
        void onConnectedServerChanged(const swift::misc::network::CServer &server);

        bool m_showToolTip = true;
        swift::misc::CDigestSignal m_dsCommandTooltip { this, &CCommandInput::setCommandToolTip, 5000, 2 };
    };
} // ns
#endif // guard
