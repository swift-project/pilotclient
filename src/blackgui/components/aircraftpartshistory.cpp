/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftpartshistory.h"
#include "ui_aircraftpartshistory.h"

#include "blackmisc/propertyindexlist.h"
#include "blackmisc/htmlutils.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include <QCompleter>
#include <QStringBuilder>
#include <QStringListModel>
#include <QHash>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CAircraftPartsHistory::CAircraftPartsHistory(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAircraftPartsHistory)
        {
            ui->setupUi(this);
            ui->cb_PartsHistoryEnabled->setChecked(sApp && sApp->isDeveloperFlagSet()); // default

            m_timerUpdateHistory.setInterval(2 * 1000);
            this->initGui();
            m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
            connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEntered, this, &CAircraftPartsHistory::callsignEntered);
            connect(ui->cb_PartsHistoryEnabled, &QCheckBox::toggled, this, &CAircraftPartsHistory::toggleHistoryEnabled);

            if (this->hasContexts())
            {
                connect(sGui->getIContextNetwork(), &IContextNetwork::changedLogOrDebugSettings, this, &CAircraftPartsHistory::valuesChanged);
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAircraftPartsHistory::connectionStatusChanged);
            }
            connect(&m_timerUpdateHistory, &QTimer::timeout, this, &CAircraftPartsHistory::updatePartsHistory);
        }

        CAircraftPartsHistory::~CAircraftPartsHistory()
        { }

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
                html = cs.toQString() % QLatin1String(" does not support aircraft parts or nothing received yet.");
            }
            else
            {
                const QString s =
                    QLatin1String("lights on:") %
                    QLatin1String("<br>") %
                    QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;") %
                    currentAircraftParts.getLights().toQString() %
                    QLatin1String("<br>") %
                    QLatin1String("gear down: ") %
                    BlackMisc::boolToYesNo(currentAircraftParts.isGearDown()) %
                    QLatin1String("<br>") %
                    QLatin1String("flaps pct: ") %
                    QString::number(currentAircraftParts.getFlapsPercent()) %
                    QLatin1String("<br>") %
                    QLatin1String("spoilers out: ") %
                    BlackMisc::boolToYesNo(currentAircraftParts.isSpoilersOut()) %
                    QLatin1String("<br>") %
                    QLatin1String("engines on: ") %
                    QLatin1String("<br>") %
                    QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;") %
                    currentAircraftParts.getEngines().toQString() %
                    QLatin1String("<br>") %
                    QLatin1String(" on ground: ") %
                    BlackMisc::boolToYesNo(currentAircraftParts.isOnGround());
                html += s;
                if (ui->cb_PartsHistoryEnabled->isChecked())
                {
                    html += QLatin1String("<hr>") %
                            aircraftPartsHistory.toHtml(CStatusMessageList::timestampHtmlOutput());
                }
            }

            const uint hash = qHash(html);
            if (hash == m_htmlHash) { return; } // avoid to always scroll to the end when there is no update
            m_htmlHash = hash;
            m_text.setHtml(html);
            ui->te_Messages->setDocument(&m_text);

            if (ui->cb_AutoScrollEnabled->isChecked())
            {
                QTextCursor c =  ui->te_Messages->textCursor();
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

        void CAircraftPartsHistory::connectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (to == INetwork::Connected || to == INetwork::Disconnected)
            {
                this->initGui();
            }
        }
    } // ns
} // ns
