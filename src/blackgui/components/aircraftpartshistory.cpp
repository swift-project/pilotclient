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
#include <QStringListModel>

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
            ui->le_Callsign->setValidator(new CUpperCaseValidator(this));
            ui->le_Callsign->setCompleter(new QCompleter(ui->le_Callsign));
            this->m_timerCallsignUpdate.setInterval(20 * 1000);
            this->m_timerUpdateHistory.setInterval(2 * 1000);
            this->initGui();
            this->m_text.setDefaultStyleSheet(CStatusMessageList::htmlStyleSheet());
            connect(ui->le_Callsign, &QLineEdit::textEdited, this, &CAircraftPartsHistory::callsignModified);
            connect(ui->le_Callsign, &QLineEdit::returnPressed, this, &CAircraftPartsHistory::callsignEntered);
            connect(ui->cb_PartsHistoryEnabled, &QCheckBox::toggled, this, &CAircraftPartsHistory::toggleHistoryEnabled);

            if (this->hasContexts())
            {
                connect(sGui->getIContextNetwork(), &IContextNetwork::changedLogOrDebugSettings, this, &CAircraftPartsHistory::valuesChanged);
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAircraftPartsHistory::connectionStatusChanged);
            }
            connect(&this->m_timerCallsignUpdate, &QTimer::timeout, this, &CAircraftPartsHistory::updateCallsignCompleter);
            connect(&this->m_timerUpdateHistory, &QTimer::timeout, this, &CAircraftPartsHistory::updatePartsHistory);
        }

        CAircraftPartsHistory::~CAircraftPartsHistory()
        { }

        void CAircraftPartsHistory::initGui()
        {
            const bool needCallsigns = this->partsHistoryEnabled();
            if (needCallsigns && !m_timerCallsignUpdate.isActive() && !m_timerUpdateHistory.isActive())
            {
                this->m_timerCallsignUpdate.start();
                this->m_timerUpdateHistory.start();
                this->updateCallsignCompleter();
            }
            else if (!needCallsigns)
            {
                this->m_timerCallsignUpdate.stop();
                this->m_timerUpdateHistory.stop();
            }

            // avoid signal roundtrip
            bool c = sGui->getIContextNetwork()->isAircraftPartsHistoryEnabled();
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

        void CAircraftPartsHistory::updateCallsignCompleter()
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

        void CAircraftPartsHistory::updatePartsHistory()
        {
            if (!this->hasContexts()) { return; }
            if (isBeingModified) { return; }
            static const CPropertyIndexList properties({ CStatusMessage::IndexUtcTimestampFormattedHms, CStatusMessage::IndexMessage });
            const CCallsign cs(ui->le_Callsign->text().trimmed().toUpper());
            if (cs.isEmpty()) { return; }
            const auto currentAircraftParts = sGui->getIContextNetwork()->getRemoteAircraftParts(cs, -1).frontOrDefault();
            const auto aircraftPartsHistory = sGui->getIContextNetwork()->getAircraftPartsHistory(cs);

            QString html;
            if (currentAircraftParts == CAircraftParts() && aircraftPartsHistory.isEmpty())
            {
                html = cs.toQString() + QString(" does not support aircraft parts or nothing received yet.");
            }
            else
            {
                QString s;
                s += "lights on:";
                s += "<br>";
                s += "&nbsp;&nbsp;&nbsp;&nbsp;";
                s += currentAircraftParts.getLights().toQString();
                s += "<br>";
                s += "gear down: ";
                s += BlackMisc::boolToYesNo(currentAircraftParts.isGearDown());
                s += "<br>";
                s += "flaps pct: ";
                s += QString::number(currentAircraftParts.getFlapsPercent());
                s += "<br>";
                s += "spoilers out: ";
                s += BlackMisc::boolToYesNo(currentAircraftParts.isSpoilersOut());
                s += "<br>";
                s += "engines on: ";
                s += "<br>";
                s += "&nbsp;&nbsp;&nbsp;&nbsp;";
                s += currentAircraftParts.getEngines().toQString();
                s += "<br>";
                s += " on ground: ";
                s += BlackMisc::boolToYesNo(currentAircraftParts.isOnGround());
                html += s;
                if (ui->cb_PartsHistoryEnabled->isChecked())
                {
                    html +="<hr>";
                    html += aircraftPartsHistory.toHtml(properties);
                }
            }

            this->m_text.setHtml(html);
            ui->te_Messages->setDocument(&this->m_text);

            if (ui->cb_AutoScrollEnabled->isChecked())
            {
                QTextCursor c =  ui->te_Messages->textCursor();
                c.movePosition(QTextCursor::End);
                ui->te_Messages->setTextCursor(c);
            }
        }

        void CAircraftPartsHistory::callsignEntered()
        {
            isBeingModified = false;
            updatePartsHistory();
            m_timerUpdateHistory.start();
        }

        void CAircraftPartsHistory::callsignModified()
        {
            isBeingModified = true;
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
