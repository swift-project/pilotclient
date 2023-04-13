/* Copyright (C) 2015
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/commandinput.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/simplecommandparser.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CCommandInput::CCommandInput(QWidget *parent) : CLineEditHistory(parent),
                                                    CIdentifiable(this)
    {
        if (!CSimpleCommandParser::registered("BlackGui::Components::CCommandInput"))
        {
            CSimpleCommandParser::registerCommand({ ".tooltip", "toggle dot command tooltip" });
            CSimpleCommandParser::registerCommand({ ".help", "show help" });
        }

        if (this->placeholderText().isEmpty())
        {
            this->setPlaceholderText(".dot commands");
        }

        const QPointer<CCommandInput> myself(this);
        QTimer::singleShot(5000, this, [=] {
            if (!myself) { return; }
            m_dsCommandTooltip.inputSignal();
        });

        if (sGui && sGui->supportsContexts())
        {
            if (sGui->getIContextSimulator())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this, &CCommandInput::onSimulatorPluginChanged, Qt::QueuedConnection);
            }
            if (sGui->getIContextNetwork())
            {
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectedServerChanged, this, &CCommandInput::onConnectedServerChanged, Qt::QueuedConnection);
            }
        }
        connect(this, &CCommandInput::returnPressedUnemptyLine, this, &CCommandInput::validateCommand, Qt::QueuedConnection);
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
        else
        {
            emit this->textEntered(c, this->identifier());
        }
    }

    void CCommandInput::setCommandToolTip()
    {
        const bool context = (sGui && sGui->getIContextApplication());
        if (m_showToolTip)
        {
            this->setToolTip(context ?
                                 sGui->getIContextApplication()->dotCommandsHtmlHelp() :
                                 CSimpleCommandParser::commandsHtmlHelp());
        }
        else
        {
            this->setToolTip("");
        }
    }

    void CCommandInput::onSimulatorPluginChanged(const CSimulatorPluginInfo &info)
    {
        Q_UNUSED(info)

        // different simulators have different commands
        m_dsCommandTooltip.inputSignal();
    }

    void CCommandInput::onConnectedServerChanged(const Network::CServer &server)
    {
        Q_UNUSED(server)

        // commands of network
        m_dsCommandTooltip.inputSignal();
    }
} // ns
