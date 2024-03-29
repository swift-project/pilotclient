// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
    CModelMatcherLogComponent::CModelMatcherLogComponent(QWidget *parent) : QFrame(parent),
                                                                            ui(new Ui::CModelMatcherLogComponent)
    {
        ui->setupUi(this);
        m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEnteredDigest, this, &CModelMatcherLogComponent::callsignEntered);
    }

    CModelMatcherLogComponent::~CModelMatcherLogComponent()
    {}

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
