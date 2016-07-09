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
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include <QCompleter>
#include <QStringListModel>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackCore;

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
            this->m_timer.setInterval(20 * 1000);
            this->initGui();
            this->m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
            connect(ui->le_Callsign, &QLineEdit::returnPressed, this, &CModelMatcherLogComponent::ps_callsignEntered);
            connect(ui->cb_LogReverseLookup, &QCheckBox::toggled, this, &CModelMatcherLogComponent::ps_reverseLookupEnabled);
            connect(&this->m_timer, &QTimer::timeout, this, &CModelMatcherLogComponent::ps_updateCallsignCompleter);
        }

        CModelMatcherLogComponent::~CModelMatcherLogComponent()
        { }

        void CModelMatcherLogComponent::initGui()
        {
            bool reverseLookup = false;
            if (sGui && sGui->getIContextNetwork())
            {
                reverseLookup = sGui->getIContextNetwork()->isReverseLookupMessagesEnabled();
            }
            if (reverseLookup && !this->m_timer.isActive())
            {
                this->m_timer.start();
                this->ps_updateCallsignCompleter();
            }
            else if (!reverseLookup)
            {
                this->m_timer.stop();
            }
            ui->cb_LogReverseLookup->setChecked(reverseLookup);
            ui->le_Callsign->setReadOnly(!reverseLookup);
        }

        void CModelMatcherLogComponent::ps_updateCallsignCompleter()
        {
            if (!sGui || !sGui->getIContextNetwork()->isConnected()) { return; }

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
            if (!sGui || !ui->cb_LogReverseLookup->isChecked()) { return; }
            static const CPropertyIndexList properties({ CStatusMessage::IndexMessage });
            const CCallsign cs(ui->le_Callsign->text().trimmed().toUpper());
            const CStatusMessageList msgs = sGui->getIContextNetwork()->getReverseLookupMessages(cs);
            const QString html = msgs.toHtml(properties);
            this->m_text.setHtml(html);
            ui->te_Messages->setDocument(&this->m_text);
        }

        void CModelMatcherLogComponent::ps_reverseLookupEnabled(bool enabled)
        {
            if (!sGui || !sGui->getIContextNetwork()) { return; }
            sGui->getIContextNetwork()->enableReverseLookupMessages(enabled);
            this->initGui();
        }
    } // ns
} // ns
