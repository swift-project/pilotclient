// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/commandinput.h"

#include "core/context/contextapplication.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "misc/simplecommandparser.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core::context;

namespace swift::gui::components
{
    CCommandInput::CCommandInput(QWidget *parent) : CLineEditHistory(parent), CIdentifiable(this)
    {
        if (!CSimpleCommandParser::registered("swift::gui::components::CCommandInput"))
        {
            CSimpleCommandParser::registerCommand({ ".tooltip", "toggle dot command tooltip" });
            CSimpleCommandParser::registerCommand({ ".help", "show help" });
        }

        if (this->placeholderText().isEmpty()) { this->setPlaceholderText(".dot commands"); }

        const QPointer<CCommandInput> myself(this);
        QTimer::singleShot(5000, this, [=, this] {
            if (!myself) { return; }
            m_dsCommandTooltip.inputSignal();
        });

        if (sGui && sGui->supportsContexts())
        {
            if (sGui->getIContextSimulator())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this,
                        &CCommandInput::onSimulatorPluginChanged, Qt::QueuedConnection);
            }
            if (sGui->getIContextNetwork())
            {
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectedServerChanged, this,
                        &CCommandInput::onConnectedServerChanged, Qt::QueuedConnection);
            }
        }
        connect(this, &CCommandInput::returnPressedUnemptyLine, this, &CCommandInput::validateCommand,
                Qt::QueuedConnection);
    }

    void CCommandInput::showToolTip(bool show)
    {
        m_showToolTip = show;
        this->setCommandToolTip();
    }

    void CCommandInput::validateCommand()
    {
        const QString c(this->getLastEnteredLineFormatted());
        if (c.isEmpty()) { return; }
        if (c.startsWith('.'))
        {
            if (c.contains("help", Qt::CaseInsensitive))
            {
                this->setCommandToolTip();
                return;
            }
            if (c.contains("tooltip", Qt::CaseInsensitive))
            {
                this->showToolTip(!m_showToolTip);
                return;
            }
            emit this->commandEntered(c, this->identifier());
        }
        else { emit this->textEntered(c, this->identifier()); }
    }

    void CCommandInput::setCommandToolTip()
    {
        if (m_showToolTip) { this->setToolTip(CSimpleCommandParser::commandsHtmlHelp()); }
        else { this->setToolTip(""); }
    }

    void CCommandInput::onSimulatorPluginChanged(const CSimulatorPluginInfo &info)
    {
        Q_UNUSED(info)

        // different simulators have different commands
        m_dsCommandTooltip.inputSignal();
    }

    void CCommandInput::onConnectedServerChanged(const network::CServer &server)
    {
        Q_UNUSED(server)

        // commands of network
        m_dsCommandTooltip.inputSignal();
    }
} // namespace swift::gui::components
