/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelmatcherlogcomponent.h"
#include "ui_modelmatcherlogcomponent.h"

#include "blackmisc/propertyindexlist.h"
#include "blackmisc/htmlutils.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include <QCompleter>
#include <QStringListModel>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CModelMatcherLogComponent::CModelMatcherLogComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CModelMatcherLogComponent)
        {
            ui->setupUi(this);
            this->initGui();
            m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
            connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEntered, this, &CModelMatcherLogComponent::callsignEntered);
            connect(ui->cb_LogReverseLookup, &QCheckBox::toggled, this, &CModelMatcherLogComponent::enabledCheckboxChanged);
            connect(ui->cb_LogMatchingMessages, &QCheckBox::toggled, this, &CModelMatcherLogComponent::enabledCheckboxChanged);

            if (this->hasContexts())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::changedLogOrDebugSettings, this, &CModelMatcherLogComponent::valuesChanged, Qt::QueuedConnection);
                connect(sGui->getIContextNetwork(), &IContextNetwork::changedLogOrDebugSettings, this, &CModelMatcherLogComponent::valuesChanged, Qt::QueuedConnection);
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CModelMatcherLogComponent::connectionStatusChanged, Qt::QueuedConnection);
            }
        }

        CModelMatcherLogComponent::~CModelMatcherLogComponent()
        { }

        void CModelMatcherLogComponent::initGui()
        {
            if (sGui)
            {
                // avoid signal roundtrips
                bool c = sGui->getIContextNetwork()->isReverseLookupMessagesEnabled();
                if (c != ui->cb_LogReverseLookup->isChecked())
                {
                    ui->cb_LogReverseLookup->setChecked(c);
                }

                c = sGui->getIContextSimulator()->isMatchingMessagesEnabled();
                if (c != ui->cb_LogMatchingMessages->isChecked())
                {
                    ui->cb_LogMatchingMessages->setChecked(c);
                }
            }
        }

        bool CModelMatcherLogComponent::hasContexts() const
        {
            return sGui && sGui->getIContextSimulator() && sGui->getIContextNetwork();
        }

        bool CModelMatcherLogComponent::enabledMessages() const
        {
            return this->hasContexts() && (ui->cb_LogMatchingMessages->isChecked() || ui->cb_LogReverseLookup->isChecked());
        }

        void CModelMatcherLogComponent::callsignEntered()
        {
            if (!this->hasContexts()) { return; }
            const CCallsign cs(ui->comp_CallsignCompleter->getCallsign());
            if (cs.isEmpty()) { return; }
            const CStatusMessageList reverseLookupMessages = sGui->getIContextNetwork()->getReverseLookupMessages(cs);
            const CStatusMessageList matchingMessages = sGui->getIContextSimulator()->getMatchingMessages(cs);

            CStatusMessageList allMessages(reverseLookupMessages);
            allMessages.push_back(matchingMessages);

            const QString html = allMessages.toHtml();
            m_text.setHtml(html);
            ui->te_Messages->setDocument(&m_text);
        }

        void CModelMatcherLogComponent::valuesChanged()
        {
            this->initGui();
        }

        void CModelMatcherLogComponent::enabledCheckboxChanged(bool enabled)
        {
            if (!sGui || !sGui->getIContextNetwork() || !sGui->getIContextSimulator()) { return; }
            const QObject *sender = QObject::sender();
            if (sender == ui->cb_LogReverseLookup)
            {
                sGui->getIContextNetwork()->enableReverseLookupMessages(enabled);
            }
            else if (sender == ui->cb_LogMatchingMessages)
            {
                sGui->getIContextSimulator()->enableMatchingMessages(enabled);
            }
        }

        void CModelMatcherLogComponent::connectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (to == INetwork::Connected || to == INetwork::Disconnected)
            {
                this->initGui();
            }
        }
    } // ns
} // ns
