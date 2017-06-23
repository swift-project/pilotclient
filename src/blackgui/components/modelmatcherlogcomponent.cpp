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
            ui->le_Callsign->setValidator(new CUpperCaseValidator(this));
            ui->le_Callsign->setCompleter(new QCompleter(ui->le_Callsign));
            this->m_updateCompleterTimer.setInterval(20 * 1000);
            this->initGui();
            this->m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
            connect(ui->le_Callsign, &QLineEdit::returnPressed, this, &CModelMatcherLogComponent::ps_callsignEntered);
            connect(ui->cb_LogReverseLookup, &QCheckBox::toggled, this, &CModelMatcherLogComponent::ps_enabledCheckboxChanged);
            connect(ui->cb_LogMatchingMessages, &QCheckBox::toggled, this, &CModelMatcherLogComponent::ps_enabledCheckboxChanged);

            if (this->hasContexts())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::changedLogOrDebugSettings, this, &CModelMatcherLogComponent::ps_valuesChanged);
                connect(sGui->getIContextNetwork(), &IContextNetwork::changedLogOrDebugSettings, this, &CModelMatcherLogComponent::ps_valuesChanged);
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CModelMatcherLogComponent::ps_connectionStatusChanged);
            }
            connect(&this->m_updateCompleterTimer, &QTimer::timeout, this, &CModelMatcherLogComponent::ps_updateCallsignCompleter);
        }

        CModelMatcherLogComponent::~CModelMatcherLogComponent()
        { }

        void CModelMatcherLogComponent::initGui()
        {
            const bool needCallsigns = this->enabledMessages();
            if (needCallsigns && !this->m_updateCompleterTimer.isActive())
            {
                this->m_updateCompleterTimer.start();
                this->ps_updateCallsignCompleter();
            }
            else if (!needCallsigns)
            {
                this->m_updateCompleterTimer.stop();
            }

            // avoid signal roundtrip
            bool c = sGui->getIContextNetwork()->isReverseLookupMessagesEnabled();
            ui->cb_LogReverseLookup->setChecked(c);

            c = sGui->getIContextSimulator()->isMatchingMessagesEnabled();
            ui->cb_LogMatchingMessages->setChecked(c);
        }

        bool CModelMatcherLogComponent::hasContexts() const
        {
            return sGui && sGui->getIContextSimulator() && sGui->getIContextNetwork();
        }

        bool CModelMatcherLogComponent::enabledMessages() const
        {
            return this->hasContexts() && (ui->cb_LogMatchingMessages->isChecked() || ui->cb_LogReverseLookup->isChecked());
        }

        void CModelMatcherLogComponent::ps_updateCallsignCompleter()
        {
            if (!this->hasContexts() || !sGui->getIContextNetwork()->isConnected()) { return; }

            const QStringList callsigns = sGui->getIContextNetwork()->getAircraftInRangeCallsigns().toStringList(false);
            QCompleter *completer = ui->le_Callsign->completer();
            Q_ASSERT_X(completer, Q_FUNC_INFO, "missing completer");
            if (!completer->model())
            {
                completer->setModel(new QStringListModel(callsigns, completer));
            }
            else
            {
                qobject_cast<QStringListModel *>(completer->model())->setStringList(callsigns);
            }
        }

        void CModelMatcherLogComponent::ps_callsignEntered()
        {
            if (!this->hasContexts()) { return; }
            const CCallsign cs(ui->le_Callsign->text().trimmed().toUpper());
            const CStatusMessageList reverseLookupMessages = sGui->getIContextNetwork()->getReverseLookupMessages(cs);
            const CStatusMessageList matchingMessages = sGui->getIContextSimulator()->getMatchingMessages(cs);

            CStatusMessageList allMessages(reverseLookupMessages);
            allMessages.push_back(matchingMessages);

            const QString html = allMessages.toHtml();
            this->m_text.setHtml(html);
            ui->te_Messages->setDocument(&this->m_text);
        }

        void CModelMatcherLogComponent::ps_valuesChanged()
        {
            this->initGui();
        }

        void CModelMatcherLogComponent::ps_enabledCheckboxChanged(bool enabled)
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

        void CModelMatcherLogComponent::ps_connectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (to == INetwork::Connected || to == INetwork::Disconnected)
            {
                this->initGui();
            }
        }
    } // ns
} // ns
