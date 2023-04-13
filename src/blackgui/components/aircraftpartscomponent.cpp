/* Copyright (C) 2020
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftpartscomponent.h"
#include "ui_aircraftpartscomponent.h"

#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackmisc/aviation/aircraftenginelist.h"
#include "blackmisc/aviation/aircraftlights.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CAircraftPartsComponent::CAircraftPartsComponent(QWidget *parent) : QFrame(parent),
                                                                        ui(new Ui::CAircraftPartsComponent)
    {
        ui->setupUi(this);

        ui->editor_AircraftParts->showSetButton(false);
        connect(ui->pb_SendAircraftPartsGui, &QPushButton::released, this, &CAircraftPartsComponent::sendAircraftParts);
        connect(ui->pb_SendAircraftPartsJson, &QPushButton::released, this, &CAircraftPartsComponent::sendAircraftParts);
        connect(ui->pb_CurrentParts, &QPushButton::released, this, &CAircraftPartsComponent::setCurrentParts);
        connect(ui->pb_OwnParts, &QPushButton::released, this, &CAircraftPartsComponent::displayOwnParts);
        connect(ui->pb_RequestFromNetwork, &QPushButton::released, this, &CAircraftPartsComponent::requestPartsFromNetwork);
        connect(ui->pb_DisplayLog, &QPushButton::released, this, &CAircraftPartsComponent::displayLogInSimulator);
        connect(ui->comp_RemoteAircraftCompleter, &CCallsignCompleter::validCallsignEnteredDigest, this, &CAircraftPartsComponent::onCallsignChanged);

        ui->comp_RemoteAircraftCompleter->addOwnCallsign(true);
    }

    CAircraftPartsComponent::~CAircraftPartsComponent()
    {
        // void
    }

    void CAircraftPartsComponent::setCallsign(const CCallsign &callsign)
    {
        ui->comp_RemoteAircraftCompleter->setCallsign(callsign);
        this->onCallsignChanged();
    }

    void CAircraftPartsComponent::sendAircraftParts()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        Q_ASSERT(sGui->getIContextNetwork());
        if (!sGui->getIContextNetwork()->isConnected())
        {
            CLogMessage(this).validationError(u"Cannot send aircraft parts, network not connected");
            return;
        }
        const CCallsign callsign(ui->comp_RemoteAircraftCompleter->getCallsign(true));
        if (callsign.isEmpty())
        {
            CLogMessage(this).validationError(u"No valid callsign selected");
            return;
        }

        CClient client = sGui->getIContextNetwork()->getClientsForCallsigns(callsign).frontOrDefault();
        if (client.getCallsign().isEmpty() || client.getCallsign() != callsign)
        {
            CLogMessage(this).validationError(u"No valid client for '%1'") << callsign.asString();
            return;
        }

        if (!client.hasAircraftPartsCapability())
        {
            static const QString question("'%1' does not support parts, enable parts for it?");
            const QMessageBox::StandardButton reply = QMessageBox::question(this, "No parts supported", question.arg(callsign.asString()), QMessageBox::Yes | QMessageBox::No);
            if (reply != QMessageBox::Yes) { return; }
            client.addCapability(CClient::FsdWithAircraftConfig);
            const bool enabled = sGui->getIContextNetwork()->setOtherClient(client);
            Q_UNUSED(enabled)
        }

        const bool json = (QObject::sender() == ui->pb_SendAircraftPartsJson);
        const CAircraftParts parts = json ? ui->editor_AircraftParts->getAircraftPartsFromJson() : ui->editor_AircraftParts->getAircraftPartsFromGui();
        ui->editor_AircraftParts->setAircraftParts(parts); // display in UI as GUI and JSON

        ui->tb_History->setToolTip("");
        const bool incremental = ui->cb_AircraftPartsIncremental->isChecked();
        sGui->getIContextNetwork()->testAddAircraftParts(callsign, parts, incremental);
        CLogMessage(this).info(u"Added parts for %1") << callsign.toQString();
    }

    void CAircraftPartsComponent::setCurrentParts()
    {
        if (!sGui->getIContextNetwork()->isConnected()) { return; }
        const CCallsign callsign(ui->comp_RemoteAircraftCompleter->getCallsign());
        if (callsign.isEmpty()) { return; }

        const CAircraftPartsList partsList = sGui->getIContextNetwork()->getRemoteAircraftParts(callsign);
        if (partsList.isEmpty())
        {
            CStatusMessage(this).info(u"No parts for '%1'") << callsign.asString();
            return;
        }
        const CAircraftParts parts = partsList.latestObject();
        const CStatusMessageList history = sGui->getIContextNetwork()->getAircraftPartsHistory(callsign);
        ui->editor_AircraftParts->setAircraftParts(parts);
        ui->tb_History->setToolTip(history.toHtml());
    }

    void CAircraftPartsComponent::requestPartsFromNetwork()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextNetwork()) { return; }

        const CCallsign callsign(ui->comp_RemoteAircraftCompleter->getCallsign(true));
        if (callsign.isEmpty())
        {
            CLogMessage(this).validationError(u"No valid callsign selected");
            return;
        }
        ui->pb_RequestFromNetwork->setEnabled(false);
        sGui->getIContextNetwork()->testRequestAircraftConfig(callsign);
        CLogMessage(this).info(u"Request aircraft config for '%1'") << callsign.asString();

        // simple approach to update UI when parts are received
        const QPointer<CAircraftPartsComponent> myself(this);
        QTimer::singleShot(3000, this, [=] {
            if (!myself) { return; }
            ui->pb_CurrentParts->click();
            ui->pb_RequestFromNetwork->setEnabled(true);
        });
    }

    void CAircraftPartsComponent::onCallsignChanged()
    {
        this->setCurrentParts();
        emit this->callsignChanged(ui->comp_RemoteAircraftCompleter->getCallsign());
    }

    void CAircraftPartsComponent::displayOwnParts()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextOwnAircraft()) { return; }

        const CSimulatedAircraft myAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
        const CCallsign cs = myAircraft.getCallsign();
        const CAircraftParts parts = myAircraft.getParts();
        ui->comp_RemoteAircraftCompleter->setCallsign(cs);
        ui->editor_AircraftParts->setAircraftParts(parts);
    }

    void CAircraftPartsComponent::displayLogInSimulator()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextSimulator()) { return; }
        const CCallsign callsign(ui->comp_RemoteAircraftCompleter->getCallsign(true));
        if (callsign.isEmpty())
        {
            CLogMessage(this).validationError(u"No valid callsign selected");
            return;
        }

        const CIdentifier i(this->objectName());
        const QString dotCmd(".drv pos " + callsign.asString());
        sGui->getIContextSimulator()->parseCommandLine(dotCmd, i);
    }

} // ns
