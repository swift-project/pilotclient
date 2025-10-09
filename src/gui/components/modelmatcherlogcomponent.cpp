// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "modelmatcherlogcomponent.h"

#include <QCompleter>
#include <QStringListModel>

#include "ui_modelmatcherlogcomponent.h"

#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "gui/uppercasevalidator.h"
#include "misc/htmlutils.h"
#include "misc/propertyindexlist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::core;
using namespace swift::core::context;

namespace swift::gui::components
{
    CModelMatcherLogComponent::CModelMatcherLogComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CModelMatcherLogComponent)
    {
        ui->setupUi(this);
        m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEnteredDigest, this,
                &CModelMatcherLogComponent::callsignEntered);
    }

    CModelMatcherLogComponent::~CModelMatcherLogComponent() = default;

    bool CModelMatcherLogComponent::hasContexts() const
    {
        return sGui && sGui->getIContextSimulator() && sGui->getIContextNetwork();
    }

    bool CModelMatcherLogComponent::enabledMessages() const
    {
        return this->hasContexts() &&
               (ui->comp_EnableLog->isMatchingLogEnabled() || ui->comp_EnableLog->isReverseLookupLogEnabled());
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
} // namespace swift::gui::components
