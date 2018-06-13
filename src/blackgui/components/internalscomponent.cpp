/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackgui/components/internalscomponent.h"
#include "blackgui/components/remoteaircraftselector.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/aviation/aircraftenginelist.h"
#include "blackmisc/aviation/aircraftlights.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "ui_internalscomponent.h"

#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTextEdit>
#include <Qt>
#include <QtGlobal>
#include <QDesktopServices>
#include <QDateTime>
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CInternalsComponent::CInternalsComponent(QWidget *parent) :
            QWidget(parent),
            ui(new Ui::CInternalsComponent)
        {
            ui->setupUi(this);
            ui->tw_Internals->setCurrentIndex(0);
            ui->comp_RemoteAircraftSelector->indicatePartsEnabled(true);
            ui->editor_AircraftParts->showSetButton(false);

            ui->le_TxtMsgFrom->setValidator(new CUpperCaseValidator(ui->le_TxtMsgFrom));
            ui->le_TxtMsgTo->setValidator(new CUpperCaseValidator(ui->le_TxtMsgFrom));

            connect(ui->pb_SendAircraftPartsGui, &QPushButton::pressed, this, &CInternalsComponent::sendAircraftParts);
            connect(ui->pb_SendAircraftPartsJson, &QPushButton::pressed, this, &CInternalsComponent::sendAircraftParts);
            connect(ui->pb_CurrentParts, &QPushButton::pressed, this, &CInternalsComponent::setCurrentParts);

            connect(ui->cb_DebugContextAudio, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
            connect(ui->cb_DebugContextApplication, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
            connect(ui->cb_DebugContextNetwork, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
            connect(ui->cb_DebugContextOwnAircraft, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
            connect(ui->cb_DebugContextSimulator, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);

            connect(ui->pb_SendTextMessage, &QPushButton::pressed, this, &CInternalsComponent::sendTextMessage);
            connect(ui->tb_LogStatusMessage, &QPushButton::pressed, this, &CInternalsComponent::logStatusMessage);
            connect(ui->le_StatusMessage, &QLineEdit::returnPressed, this, &CInternalsComponent::logStatusMessage);

            connect(ui->pb_LatestInterpolationLog, &QPushButton::pressed, this, &CInternalsComponent::showLogFiles);
            connect(ui->pb_LatestPartsLog, &QPushButton::pressed, this, &CInternalsComponent::showLogFiles);
            connect(ui->pb_RequestFromNetwork, &QPushButton::pressed, this, &CInternalsComponent::requestPartsFromNetwork);
            connect(ui->pb_DisplayLog, &QPushButton::pressed, this, &CInternalsComponent::displayLogInSimulator);

            connect(ui->comp_RemoteAircraftSelector, &CRemoteAircraftSelector::changedCallsign, this, &CInternalsComponent::selectorChanged);
            this->contextFlagsToGui();
        }

        CInternalsComponent::~CInternalsComponent() { }

        void CInternalsComponent::showEvent(QShowEvent *event)
        {
            // force new data when visible
            this->contextFlagsToGui();
            QWidget::showEvent(event);
        }

        void CInternalsComponent::sendAircraftParts()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }

            Q_ASSERT(sGui->getIContextNetwork());
            if (!sGui->getIContextNetwork()->isConnected())
            {
                CLogMessage(this).validationError("Cannot send aircraft parts, network not connected");
                return;
            }
            const CCallsign callsign(ui->comp_RemoteAircraftSelector->getSelectedCallsign());
            if (callsign.isEmpty())
            {
                CLogMessage(this).validationError("No valid callsign selected");
                return;
            }

            CClient client = sGui->getIContextNetwork()->getClientsForCallsigns(callsign).frontOrDefault();
            if (client.getCallsign().isEmpty() || client.getCallsign() != callsign)
            {
                CLogMessage(this).validationError("No valid client for '%1'") << callsign.asString();
                return;
            }

            if (!client.hasAircraftPartsCapability())
            {
                static const QString question("'%1' does not support parts, enable parts for it?");
                const QMessageBox::StandardButton reply = QMessageBox::question(this, "No parts supported", question.arg(callsign.asString()), QMessageBox::Yes | QMessageBox::No);
                if (reply != QMessageBox::Yes) { return; }
                client.addCapability(CClient::FsdWithAircraftConfig);
                const bool enabled = sGui->getIContextNetwork()->setOtherClient(client);
                Q_UNUSED(enabled);
            }

            const bool json = (QObject::sender() == ui->pb_SendAircraftPartsJson);
            const CAircraftParts parts = json ? ui->editor_AircraftParts->getAircraftPartsFromJson() : ui->editor_AircraftParts->getAircraftPartsFromGui();
            ui->editor_AircraftParts->setAircraftParts(parts); // display in UI as GUI and JSON

            ui->tb_History->setToolTip("");
            const bool incremental = ui->cb_AircraftPartsIncremental->isChecked();
            sGui->getIContextNetwork()->testAddAircraftParts(callsign, parts, incremental);
            CLogMessage(this).info("Added parts for %1") << callsign.toQString();
        }

        void CInternalsComponent::setCurrentParts()
        {
            if (!sGui->getIContextNetwork()->isConnected()) { return; }
            const CCallsign callsign(ui->comp_RemoteAircraftSelector->getSelectedCallsign());
            if (callsign.isEmpty()) { return; }

            const CAircraftPartsList partsList = sGui->getIContextNetwork()->getRemoteAircraftParts(callsign);
            if (partsList.isEmpty())
            {
                CStatusMessage(this).info("No parts for '%1'") << callsign.asString();
                return;
            }
            const CAircraftParts parts = partsList.latestObject();
            const CStatusMessageList history = sGui->getIContextNetwork()->getAircraftPartsHistory(callsign);
            ui->editor_AircraftParts->setAircraftParts(parts);
            ui->tb_History->setToolTip(history.toHtml());
        }

        void CInternalsComponent::enableDebug(int state)
        {
            Q_ASSERT(sGui->getIContextApplication());
            Q_ASSERT(sGui->getIContextAudio());
            Q_ASSERT(sGui->getIContextNetwork());
            Q_ASSERT(sGui->getIContextOwnAircraft());
            Q_ASSERT(sGui->getIContextSimulator());

            const Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
            const bool debug = (checkState == Qt::Checked);
            const QObject *sender = QObject::sender();

            if (sender == ui->cb_DebugContextApplication)  { sGui->getIContextApplication()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextAudio)   { sGui->getIContextAudio()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextNetwork) { sGui->getIContextNetwork()->setDebugEnabled(debug);}
            else if (sender == ui->cb_DebugContextOwnAircraft) { sGui->getIContextOwnAircraft()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextSimulator)   { sGui->getIContextSimulator()->setDebugEnabled(debug);}
        }

        void CInternalsComponent::sendTextMessage()
        {
            if (ui->le_TxtMsgTo->text().isEmpty()) { return; }
            if (ui->le_TxtMsgFrom->text().isEmpty()) { return; }
            if (ui->pte_TxtMsg->toPlainText().isEmpty()) { return; }
            if (!sGui->getIContextNetwork()) { return; }

            const CCallsign sender(ui->le_TxtMsgFrom->text().trimmed());
            const CCallsign recipient(ui->le_TxtMsgTo->text().trimmed());
            const QString msgTxt(ui->pte_TxtMsg->toPlainText().trimmed());
            const double freqMHz = ui->dsb_TxtMsgFrequency->value();
            CTextMessage tm;
            CFrequency f;
            if (freqMHz >= 118.0)
            {
                f = CFrequency(freqMHz, CFrequencyUnit::MHz());
                tm = CTextMessage(msgTxt, f, sender);
            }
            else
            {
                tm = CTextMessage(msgTxt, sender, recipient);
            }
            tm.setCurrentUtcTime();
            sGui->getIContextNetwork()->testReceivedTextMessages(CTextMessageList({ tm }));
        }

        void CInternalsComponent::logStatusMessage()
        {
            if (ui->le_StatusMessage->text().isEmpty()) { return; }
            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityDebug;
            if (ui->rb_StatusMessageError->isChecked())
            {
                s = CStatusMessage::SeverityError;
            }
            else if (ui->rb_StatusMessageWarning->isChecked())
            {
                s = CStatusMessage::SeverityWarning;
            }
            else if (ui->rb_StatusMessageInfo->isChecked())
            {
                s = CStatusMessage::SeverityInfo;
            }
            const CStatusMessage sm = CStatusMessage(this, s, ui->le_StatusMessage->text().trimmed());
            CLogMessage::preformatted(sm);
        }

        void CInternalsComponent::showLogFiles()
        {
            QString file;
            const QObject *sender = QObject::sender();
            if (sender == ui->pb_LatestInterpolationLog)
            {
                file = CInterpolationLogger::getLatestLogFiles().first();
            }
            else if (sender == ui->pb_LatestPartsLog)
            {
                file = CInterpolationLogger::getLatestLogFiles().last();
            }

            if (file.isEmpty()) { return; }
            QDesktopServices::openUrl(QUrl::fromLocalFile(file));
        }

        void CInternalsComponent::requestPartsFromNetwork()
        {
            const CCallsign callsign(ui->comp_RemoteAircraftSelector->getSelectedCallsign());
            if (callsign.isEmpty())
            {
                CLogMessage(this).validationError("No valid callsign selected");
                return;
            }
            ui->pb_RequestFromNetwork->setEnabled(false);
            sGui->getIContextNetwork()->testRequestAircraftConfig(callsign);
            CLogMessage(this).info("Request aircraft config for '%1'") << callsign.asString();

            // simple approach to update UI when parts are received
            const QPointer<CInternalsComponent> myself(this);
            QTimer::singleShot(3000, this, [ = ]
            {
                if (!myself) { return; }
                ui->pb_CurrentParts->click();
                ui->pb_RequestFromNetwork->setEnabled(true);
            });
        }

        void CInternalsComponent::selectorChanged()
        {
            this->setCurrentParts();
        }

        void CInternalsComponent::displayLogInSimulator()
        {
            if (!sGui || sGui->isShuttingDown())  { return; }
            if (!sGui->getIContextSimulator())  { return; }
            const CCallsign callsign(ui->comp_RemoteAircraftSelector->getSelectedCallsign());
            if (callsign.isEmpty())
            {
                CLogMessage(this).validationError("No valid callsign selected");
                return;
            }

            const CIdentifier i(this->objectName());
            const QString dotCmd(".drv pos " + callsign.asString());
            sGui->getIContextSimulator()->parseCommandLine(dotCmd, i);
        }

        void CInternalsComponent::contextFlagsToGui()
        {
            ui->cb_DebugContextApplication->setChecked(sGui->getIContextApplication()->isDebugEnabled());
            ui->cb_DebugContextNetwork->setChecked(sGui->getIContextNetwork()->isDebugEnabled());
            ui->cb_DebugContextOwnAircraft->setChecked(sGui->getIContextOwnAircraft()->isDebugEnabled());
            ui->cb_DebugContextSimulator->setChecked(sGui->getIContextSimulator()->isDebugEnabled());
        }
    } // namespace
} // namespace
