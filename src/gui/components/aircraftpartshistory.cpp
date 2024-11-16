// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aircraftpartshistory.h"

#include <QCompleter>
#include <QHash>
#include <QStringBuilder>
#include <QStringListModel>

#include "ui_aircraftpartshistory.h"

#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "gui/uppercasevalidator.h"
#include "misc/htmlutils.h"
#include "misc/propertyindexlist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::core;
using namespace swift::core::context;

namespace swift::gui::components
{
    CAircraftPartsHistory::CAircraftPartsHistory(QWidget *parent) : QFrame(parent),
                                                                    ui(new Ui::CAircraftPartsHistory)
    {
        ui->setupUi(this);
        ui->cb_PartsHistoryEnabled->setChecked(sApp && sApp->isDeveloperFlagSet()); // default

        m_timerUpdateHistory.setInterval(2 * 1000);
        this->initGui();
        m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validChangedCallsignEntered, this, &CAircraftPartsHistory::callsignEntered);
        connect(ui->cb_PartsHistoryEnabled, &QCheckBox::toggled, this, &CAircraftPartsHistory::toggleHistoryEnabled);

        if (this->hasContexts())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedLogOrDebugSettings, this, &CAircraftPartsHistory::valuesChanged);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAircraftPartsHistory::connectionStatusChanged);
        }
        connect(&m_timerUpdateHistory, &QTimer::timeout, this, &CAircraftPartsHistory::updatePartsHistory);
    }

    CAircraftPartsHistory::~CAircraftPartsHistory()
    {}

    void CAircraftPartsHistory::initGui()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return; }
        const bool needCallsigns = this->partsHistoryEnabled();
        if (needCallsigns && !m_timerUpdateHistory.isActive())
        {
            m_timerUpdateHistory.start();
        }
        else if (!needCallsigns)
        {
            m_timerUpdateHistory.stop();
        }

        // avoid signal roundtrip
        const bool c = sGui->getIContextNetwork()->isAircraftPartsHistoryEnabled();
        ui->cb_PartsHistoryEnabled->setChecked(c);
    }

    bool CAircraftPartsHistory::hasContexts() const
    {
        return sGui && sGui->getIContextSimulator() && sGui->getIContextNetwork();
    }

    bool CAircraftPartsHistory::partsHistoryEnabled() const
    {
        return this->hasContexts();
    }

    void CAircraftPartsHistory::updatePartsHistory()
    {
        if (!this->hasContexts()) { return; }
        if (!this->isVisible()) { return; }
        const CCallsign cs(ui->comp_CallsignCompleter->getCallsign());
        if (cs.isEmpty()) { return; } // no or invalid callsign
        const auto currentAircraftParts = sGui->getIContextNetwork()->getRemoteAircraftParts(cs).frontOrDefault();
        const auto aircraftPartsHistory = sGui->getIContextNetwork()->getAircraftPartsHistory(cs);

        QString html;
        if (currentAircraftParts == CAircraftParts() && aircraftPartsHistory.isEmpty())
        {
            html = cs.toQString() % u" does not support aircraft parts or nothing received yet.";
        }
        else
        {
            const QString s =
                u"lights on:"
                u"<br>"
                u"&nbsp;&nbsp;&nbsp;&nbsp;" %
                currentAircraftParts.getLights().toQString() %
                u"<br>"
                u"gear down: " %
                swift::misc::boolToYesNo(currentAircraftParts.isGearDown()) %
                u"<br>"
                u"flaps pct: " %
                QString::number(currentAircraftParts.getFlapsPercent()) %
                u"<br>"
                u"spoilers out: " %
                swift::misc::boolToYesNo(currentAircraftParts.isSpoilersOut()) %
                u"<br>"
                u"engines on: "
                u"<br>"
                u"&nbsp;&nbsp;&nbsp;&nbsp;" %
                currentAircraftParts.getEngines().toQString() %
                u"<br>"
                u" on ground: " %
                swift::misc::boolToYesNo(currentAircraftParts.isOnGround());
            html += s;
            if (ui->cb_PartsHistoryEnabled->isChecked())
            {
                html += u"<hr>" %
                        aircraftPartsHistory.toHtml(CStatusMessageList::timestampHtmlOutput());
            }
        }

        const size_t hash = qHash(html);
        if (hash == m_htmlHash) { return; } // avoid to always scroll to the end when there is no update
        m_htmlHash = hash;
        m_text.setHtml(html);
        ui->te_Messages->setDocument(&m_text);

        if (ui->cb_AutoScrollEnabled->isChecked())
        {
            QTextCursor c = ui->te_Messages->textCursor();
            c.movePosition(QTextCursor::End);
            ui->te_Messages->setTextCursor(c);
        }
    }

    void CAircraftPartsHistory::callsignEntered()
    {
        this->updatePartsHistory();
        m_timerUpdateHistory.start();
    }

    void CAircraftPartsHistory::valuesChanged()
    {
        this->initGui();
    }

    void CAircraftPartsHistory::toggleHistoryEnabled(bool enabled)
    {
        if (!sGui || !sGui->getIContextNetwork() || !sGui->getIContextSimulator()) { return; }
        const QObject *sender = QObject::sender();
        if (sender == ui->cb_PartsHistoryEnabled)
        {
            sGui->getIContextNetwork()->enableAircraftPartsHistory(enabled);
        }
    }

    void CAircraftPartsHistory::connectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from);
        if (to.isConnected() || to.isDisconnected())
        {
            this->initGui();
        }
    }
} // namespace swift::gui::components
