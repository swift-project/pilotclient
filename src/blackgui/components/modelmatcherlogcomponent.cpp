/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "modelmatcherlogcomponent.h"
#include "ui_modelmatcherlogcomponent.h"

#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/htmlutils.h"

#include <QCompleter>
#include <QStringListModel>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CModelMatcherLogComponent::CModelMatcherLogComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CModelMatcherLogComponent)
    {
        ui->setupUi(this);
        m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEnteredDigest, this, &CModelMatcherLogComponent::callsignEntered);
    }

    CModelMatcherLogComponent::~CModelMatcherLogComponent()
    { }

    bool CModelMatcherLogComponent::hasContexts() const
    {
        return sGui && sGui->getIContextSimulator() && sGui->getIContextNetwork();
    }

    bool CModelMatcherLogComponent::enabledMessages() const
    {
        return this->hasContexts() && (ui->comp_EnableLog->isMatchingLogEnabled() || ui->comp_EnableLog->isReverseLookupLogEnabled());
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
} // ns
