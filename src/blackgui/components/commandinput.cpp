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
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/simplecommandparser.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;

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

            const QPointer<CCommandInput> myself(this);
            QTimer::singleShot(5000, this, [ = ]
            {
                if (!myself) { return; }
                m_dsCommandTooltip.inputSignal();
            });

            if (sGui && sGui->supportsContexts())
            {
                if (sGui->getIContextSimulator())
                {
                    connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this, &CCommandInput::onSimulatorPluginChanged);
                }
                if (sGui->getIContextNetwork())
                {
                    connect(sGui->getIContextNetwork(), &IContextNetwork::connectedServerChanged, this, &CCommandInput::onConnectedServerChanged);
                }
            }
            connect(this, &CCommandInput::returnPressed, this, &CCommandInput::validateCommand);
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

        void CCommandInput::onSimulatorPluginChanged(const CSimulatorPluginInfo &info)
        {
            Q_UNUSED(info);

            // different simulators have different commands
            m_dsCommandTooltip.inputSignal();
        }

        void CCommandInput::onConnectedServerChanged(const Network::CServer &server)
        {
            Q_UNUSED(server);

            // commands of network
            m_dsCommandTooltip.inputSignal();
        }
    } // ns
} // ns
