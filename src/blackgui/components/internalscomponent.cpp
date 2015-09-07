/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "internalscomponent.h"
#include "ui_internalscomponent.h"
#include "blackcore/context_all_interfaces.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CInternalsComponent::CInternalsComponent(QWidget *parent) :
            QWidget(parent),
            CEnableForRuntime(nullptr, false),
            ui(new Ui::CInternalsComponent)
        {
            ui->setupUi(this);
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
        }

        CInternalsComponent::~CInternalsComponent() { }

        void CInternalsComponent::runtimeHasBeenSet()
        {
            contextFlagsToGui();
        }

        void CInternalsComponent::showEvent(QShowEvent *event)
        {
            // force new data when visible
            this->contextFlagsToGui();
            QWidget::showEvent(event);
        }

        void CInternalsComponent::ps_sendAircraftParts()
        {
            Q_ASSERT(this->getIContextNetwork());
            if (!this->getIContextNetwork()->isConnected())
            {
                CLogMessage(this).validationError("Cannot send aircraft parts, network not connected");
                return;
            }
            CCallsign callsign(this->ui->comp_RemoteAircraftSelector->getSelectedCallsign());
            if (callsign.isEmpty())
            {
                CLogMessage(this).validationError("No valid callsign selected");
                return;
            }

            CAircraftParts parts;
            bool json = (QObject::sender() == ui->pb_SendAircraftPartsJson);

            if (json)
            {
                QString jsonParts = this->ui->te_AircraftPartsJson->toPlainText().trimmed();
                if (jsonParts.isEmpty())
                {
                    CLogMessage(this).validationError("No JSON content");
                    return;
                }
                QJsonParseError jsonError;
                QJsonDocument json(QJsonDocument::fromJson(jsonParts.toUtf8(), &jsonError));
                if (jsonError.error != QJsonParseError::NoError)
                {
                    CLogMessage(this).validationError("Parse error: %1") << jsonError.errorString();
                    return;
                }
                parts.convertFromJson(json.object());
                partsToGui(parts);
            }
            else
            {
                parts = CAircraftParts(guiToAircraftParts());
                this->ps_guiToJson();
            }

            this->getIContextNetwork()->testAddAircraftParts(callsign, parts, this->ui->cb_AircraftPartsIncremental->isChecked());
            CLogMessage(this).info("Added parts for %1") << callsign.toQString();
        }

        void CInternalsComponent::ps_setAllLights()
        {
            bool on = QObject::sender() == ui->pb_AircraftPartsLightsOn ? true : false;
            this->ui->cb_AircraftPartsLightsStrobe->setChecked(on);
            this->ui->cb_AircraftPartsLightsLanding->setChecked(on);
            this->ui->cb_AircraftPartsLightsTaxi->setChecked(on);
            this->ui->cb_AircraftPartsLightsBeacon->setChecked(on);
            this->ui->cb_AircraftPartsLightsNav->setChecked(on);
            this->ui->cb_AircraftPartsLightsLogo->setChecked(on);
        }

        void CInternalsComponent::ps_setAllEngines()
        {
            bool on = QObject::sender() == ui->pb_AircraftPartsEnginesOn ? true : false;
            this->ui->cb_AircraftPartsEngine1->setChecked(on);
            this->ui->cb_AircraftPartsEngine2->setChecked(on);
            this->ui->cb_AircraftPartsEngine3->setChecked(on);
            this->ui->cb_AircraftPartsEngine4->setChecked(on);
            this->ui->cb_AircraftPartsEngine5->setChecked(on);
            this->ui->cb_AircraftPartsEngine6->setChecked(on);
        }

        void CInternalsComponent::ps_guiToJson()
        {
            QJsonDocument json(guiToAircraftParts().toJson());
            QString j(json.toJson(QJsonDocument::Indented));
            this->ui->te_AircraftPartsJson->setText(j);
        }

        void CInternalsComponent::ps_enableDebug(int state)
        {
            Q_ASSERT(this->getIContextApplication());
            Q_ASSERT(this->getIContextAudio());
            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextOwnAircraft());
            Q_ASSERT(this->getIContextSimulator());

            Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
            bool debug = (checkState == Qt::Checked);
            QObject *sender = QObject::sender();

            if (sender == ui->cb_DebugContextApplication) { getIContextApplication()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextAudio)  { getIContextAudio()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextNetwork)  { getIContextNetwork()->setDebugEnabled(debug);}
            else if (sender == ui->cb_DebugContextOwnAircraft)  { getIContextOwnAircraft()->setDebugEnabled(debug); }
            else if (sender == ui->cb_DebugContextSimulator)  { getIContextSimulator()->setDebugEnabled(debug);}
            else if (sender == ui->cb_DebugDriver || sender == ui->cb_DebugInterpolator)
            {
                this->getIContextSimulator()->enableDebugMessages(
                    this->ui->cb_DebugDriver->isChecked(),
                    this->ui->cb_DebugInterpolator->isChecked()
                );
            }
        }

        CAircraftParts CInternalsComponent::guiToAircraftParts() const
        {
            CAircraftLights lights(
                this->ui->cb_AircraftPartsLightsStrobe->isChecked(),
                this->ui->cb_AircraftPartsLightsLanding->isChecked(),
                this->ui->cb_AircraftPartsLightsTaxi->isChecked(),
                this->ui->cb_AircraftPartsLightsBeacon->isChecked(),
                this->ui->cb_AircraftPartsLightsNav->isChecked(),
                this->ui->cb_AircraftPartsLightsLogo->isChecked()
            );
            CAircraftEngineList engines(
            {
                this->ui->cb_AircraftPartsEngine1->isChecked(),
                this->ui->cb_AircraftPartsEngine2->isChecked(),
                this->ui->cb_AircraftPartsEngine3->isChecked(),
                this->ui->cb_AircraftPartsEngine4->isChecked(),
                this->ui->cb_AircraftPartsEngine5->isChecked(),
                this->ui->cb_AircraftPartsEngine6->isChecked()
            }
            );
            CAircraftParts parts(lights,
                                 this->ui->cb_AircraftPartsGearDown->isChecked(),
                                 this->ui->sb_AircraftPartsFlapsPercentage->value(),
                                 this->ui->cb_AircraftPartsSpoilers->isChecked(),
                                 engines,
                                 this->ui->cb_AircraftPartsIsOnGround->isChecked()
                                );
            return parts;
        }

        void CInternalsComponent::partsToGui(const CAircraftParts &parts)
        {
            this->ui->cb_AircraftPartsGearDown->setChecked(parts.isGearDown());
            this->ui->cb_AircraftPartsIsOnGround->setChecked(parts.isOnGround());
            this->ui->cb_AircraftPartsSpoilers->setChecked(parts.isSpoilersOut());
            this->ui->sb_AircraftPartsFlapsPercentage->setValue(parts.getFlapsPercent());

            CAircraftLights lights = parts.getLights();
            this->ui->cb_AircraftPartsLightsBeacon->setChecked(lights.isBeaconOn());
            this->ui->cb_AircraftPartsLightsLanding->setChecked(lights.isLandingOn());
            this->ui->cb_AircraftPartsLightsLogo->setChecked(lights.isLogoOn());
            this->ui->cb_AircraftPartsLightsNav->setChecked(lights.isNavOn());
            this->ui->cb_AircraftPartsLightsStrobe->setChecked(lights.isStrobeOn());
            this->ui->cb_AircraftPartsLightsTaxi->setChecked(lights.isTaxiOn());

            CAircraftEngineList engines = parts.getEngines();
            this->ui->cb_AircraftPartsEngine1->setChecked(engines.isEngineOn(1));
            this->ui->cb_AircraftPartsEngine2->setChecked(engines.isEngineOn(2));
            this->ui->cb_AircraftPartsEngine3->setChecked(engines.isEngineOn(3));
            this->ui->cb_AircraftPartsEngine4->setChecked(engines.isEngineOn(4));
            this->ui->cb_AircraftPartsEngine5->setChecked(engines.isEngineOn(5));
            this->ui->cb_AircraftPartsEngine6->setChecked(engines.isEngineOn(6));

        }

        void CInternalsComponent::contextFlagsToGui()
        {
            ui->cb_DebugContextApplication->setChecked(getIContextApplication()->isDebugEnabled());
            ui->cb_DebugContextNetwork->setChecked(getIContextNetwork()->isDebugEnabled());
            ui->cb_DebugContextOwnAircraft->setChecked(getIContextOwnAircraft()->isDebugEnabled());
            ui->cb_DebugContextSimulator->setChecked(getIContextSimulator()->isDebugEnabled());
        }

    } // namespace
} // namespace
