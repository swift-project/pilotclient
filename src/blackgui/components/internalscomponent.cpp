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
#include "blackmisc/aviation/aircraftenginelist.h"
#include "blackmisc/aviation/aircraftlights.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/textmessage.h"

#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "ui_internalscomponent.h"

#include <QCheckBox>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTextEdit>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;

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

            ui->le_TxtMsgFrom->setValidator(new CUpperCaseValidator(ui->le_TxtMsgFrom));
            ui->le_TxtMsgTo->setValidator(new CUpperCaseValidator(ui->le_TxtMsgFrom));

            connect(ui->pb_SendAircraftPartsGui, &QPushButton::pressed, this, &CInternalsComponent::ps_sendAircraftParts);
            connect(ui->pb_SendAircraftPartsJson, &QPushButton::pressed, this, &CInternalsComponent::ps_sendAircraftParts);
            connect(ui->pb_AircraftPartsLightsOn, &QPushButton::pressed, this, &CInternalsComponent::ps_setAllLights);
            connect(ui->pb_AircraftPartsLightsOff, &QPushButton::pressed, this, &CInternalsComponent::ps_setAllLights);
            connect(ui->pb_AircraftPartsEnginesOn, &QPushButton::pressed, this, &CInternalsComponent::ps_setAllEngines);
            connect(ui->pb_AircraftPartsEnginesOff, &QPushButton::pressed, this, &CInternalsComponent::ps_setAllEngines);
            connect(ui->pb_AircraftPartsUiToJson, &QPushButton::pressed, this, &CInternalsComponent::ps_guiToJson);

            connect(ui->cb_DebugContextAudio, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);
            connect(ui->cb_DebugContextApplication, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);
            connect(ui->cb_DebugContextNetwork, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);
            connect(ui->cb_DebugContextOwnAircraft, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);
            connect(ui->cb_DebugContextSimulator, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);

            connect(ui->cb_DebugDriver, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);
            connect(ui->cb_DebugInterpolator, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);
            connect(ui->cb_ForceFullInterpolation, &QCheckBox::stateChanged, this, &CInternalsComponent::ps_enableDebug);

            connect(ui->pb_SendTextMessage, &QPushButton::pressed, this, &CInternalsComponent::ps_sendTextMessage);
            connect(ui->tb_LogStatusMessage, &QPushButton::pressed, this, &CInternalsComponent::ps_logStatusMessage);
            connect(ui->le_StatusMessage, &QLineEdit::returnPressed, this, &CInternalsComponent::ps_logStatusMessage);

            contextFlagsToGui();
        }

        CInternalsComponent::~CInternalsComponent() { }

        void CInternalsComponent::showEvent(QShowEvent *event)
        {
            // force new data when visible
            this->contextFlagsToGui();
            QWidget::showEvent(event);
        }

        void CInternalsComponent::ps_sendAircraftParts()
        {
            Q_ASSERT(sGui->getIContextNetwork());
            if (!sGui->getIContextNetwork()->isConnected())
            {
                CLogMessage(this).validationError("Cannot send aircraft parts, network not connected");
                return;
            }
            CCallsign callsign(ui->comp_RemoteAircraftSelector->getSelectedCallsign());
            if (callsign.isEmpty())
            {
                CLogMessage(this).validationError("No valid callsign selected");
                return;
            }

            CAircraftParts parts;
            bool json = (QObject::sender() == ui->pb_SendAircraftPartsJson);

            if (json)
            {
                QString jsonParts = ui->te_AircraftPartsJson->toPlainText().trimmed();
                if (jsonParts.isEmpty())
                {
                    CLogMessage(this).validationError("No JSON content");
                    return;
                }
                QJsonParseError jsonError;
                QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonParts.toUtf8(), &jsonError));
                if (jsonError.error != QJsonParseError::NoError)
                {
                    CLogMessage(this).validationError("Parse error: %1") << jsonError.errorString();
                    return;
                }
                parts.convertFromJson(jsonDoc.object());
                partsToGui(parts);
            }
            else
            {
                parts = CAircraftParts(guiToAircraftParts());
                this->ps_guiToJson();
            }

            sGui->getIContextNetwork()->testAddAircraftParts(callsign, parts, ui->cb_AircraftPartsIncremental->isChecked());
            CLogMessage(this).info("Added parts for %1") << callsign.toQString();
        }

        void CInternalsComponent::ps_setAllLights()
        {
            bool on = QObject::sender() == ui->pb_AircraftPartsLightsOn ? true : false;
            ui->cb_AircraftPartsLightsStrobe->setChecked(on);
            ui->cb_AircraftPartsLightsLanding->setChecked(on);
            ui->cb_AircraftPartsLightsTaxi->setChecked(on);
            ui->cb_AircraftPartsLightsBeacon->setChecked(on);
            ui->cb_AircraftPartsLightsNav->setChecked(on);
            ui->cb_AircraftPartsLightsLogo->setChecked(on);
        }

        void CInternalsComponent::ps_setAllEngines()
        {
            bool on = QObject::sender() == ui->pb_AircraftPartsEnginesOn ? true : false;
            ui->cb_AircraftPartsEngine1->setChecked(on);
            ui->cb_AircraftPartsEngine2->setChecked(on);
            ui->cb_AircraftPartsEngine3->setChecked(on);
            ui->cb_AircraftPartsEngine4->setChecked(on);
            ui->cb_AircraftPartsEngine5->setChecked(on);
            ui->cb_AircraftPartsEngine6->setChecked(on);
        }

        void CInternalsComponent::ps_guiToJson()
        {
            QJsonDocument json(guiToAircraftParts().toJson());
            QString j(json.toJson(QJsonDocument::Indented));
            ui->te_AircraftPartsJson->setText(j);
        }

        void CInternalsComponent::ps_enableDebug(int state)
        {
            Q_ASSERT(sGui->getIContextApplication());
            Q_ASSERT(sGui->getIContextAudio());
            Q_ASSERT(sGui->getIContextNetwork());
            Q_ASSERT(sGui->getIContextOwnAircraft());
            Q_ASSERT(sGui->getIContextSimulator());

            Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
            bool debug = (checkState == Qt::Checked);
            QObject *sender = QObject::sender();

            if (sender == ui->cb_DebugContextApplication) { sGui->getIContextApplication()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextAudio)  { sGui->getIContextAudio()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextNetwork)  { sGui->getIContextNetwork()->setDebugEnabled(debug);}
            else if (sender == ui->cb_DebugContextOwnAircraft)  { sGui->getIContextOwnAircraft()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextSimulator)  { sGui->getIContextSimulator()->setDebugEnabled(debug);}
            else if (sender == ui->cb_DebugDriver || sender == ui->cb_DebugInterpolator || sender == ui->cb_ForceFullInterpolation)
            {
                CInterpolationAndRenderingSetup setup;
                setup.setForceFullInterpolation(ui->cb_ForceFullInterpolation->isChecked());
                setup.setDriverDebuggingMessages(ui->cb_DebugDriver->isChecked());
                setup.setInterpolatorDebuggingMessages(ui->cb_DebugInterpolator->isChecked());
                sGui->getIContextSimulator()->setInterpolationAndRenderingSetup(setup);
            }
        }

        void CInternalsComponent::ps_sendTextMessage()
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

        void CInternalsComponent::ps_logStatusMessage()
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

        CAircraftParts CInternalsComponent::guiToAircraftParts() const
        {
            CAircraftLights lights(
                ui->cb_AircraftPartsLightsStrobe->isChecked(),
                ui->cb_AircraftPartsLightsLanding->isChecked(),
                ui->cb_AircraftPartsLightsTaxi->isChecked(),
                ui->cb_AircraftPartsLightsBeacon->isChecked(),
                ui->cb_AircraftPartsLightsNav->isChecked(),
                ui->cb_AircraftPartsLightsLogo->isChecked()
            );
            CAircraftEngineList engines(
            {
                ui->cb_AircraftPartsEngine1->isChecked(),
                ui->cb_AircraftPartsEngine2->isChecked(),
                ui->cb_AircraftPartsEngine3->isChecked(),
                ui->cb_AircraftPartsEngine4->isChecked(),
                ui->cb_AircraftPartsEngine5->isChecked(),
                ui->cb_AircraftPartsEngine6->isChecked()
            }
            );
            CAircraftParts parts(lights,
                                 ui->cb_AircraftPartsGearDown->isChecked(),
                                 ui->sb_AircraftPartsFlapsPercentage->value(),
                                 ui->cb_AircraftPartsSpoilers->isChecked(),
                                 engines,
                                 ui->cb_AircraftPartsIsOnGround->isChecked()
                                );
            return parts;
        }

        void CInternalsComponent::partsToGui(const CAircraftParts &parts)
        {
            ui->cb_AircraftPartsGearDown->setChecked(parts.isGearDown());
            ui->cb_AircraftPartsIsOnGround->setChecked(parts.isOnGround());
            ui->cb_AircraftPartsSpoilers->setChecked(parts.isSpoilersOut());
            ui->sb_AircraftPartsFlapsPercentage->setValue(parts.getFlapsPercent());

            CAircraftLights lights = parts.getLights();
            ui->cb_AircraftPartsLightsBeacon->setChecked(lights.isBeaconOn());
            ui->cb_AircraftPartsLightsLanding->setChecked(lights.isLandingOn());
            ui->cb_AircraftPartsLightsLogo->setChecked(lights.isLogoOn());
            ui->cb_AircraftPartsLightsNav->setChecked(lights.isNavOn());
            ui->cb_AircraftPartsLightsStrobe->setChecked(lights.isStrobeOn());
            ui->cb_AircraftPartsLightsTaxi->setChecked(lights.isTaxiOn());

            CAircraftEngineList engines = parts.getEngines();
            ui->cb_AircraftPartsEngine1->setChecked(engines.isEngineOn(1));
            ui->cb_AircraftPartsEngine2->setChecked(engines.isEngineOn(2));
            ui->cb_AircraftPartsEngine3->setChecked(engines.isEngineOn(3));
            ui->cb_AircraftPartsEngine4->setChecked(engines.isEngineOn(4));
            ui->cb_AircraftPartsEngine5->setChecked(engines.isEngineOn(5));
            ui->cb_AircraftPartsEngine6->setChecked(engines.isEngineOn(6));
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
