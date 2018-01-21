/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/webdataservices.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/components/flightplancomponent.h"
#include "blackgui/components/selcalcodeselector.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/statusmessage.h"
#include "ui_flightplancomponent.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QTabBar>
#include <QCompleter>
#include <QStringBuilder>
#include <Qt>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackGui;
using namespace BlackCore;
using namespace BlackConfig;

namespace BlackGui
{
    namespace Components
    {
        CFlightPlanComponent::CFlightPlanComponent(QWidget *parent) :
            COverlayMessagesTabWidget(parent),
            ui(new Ui::CFlightPlanComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "missing sGui");
            Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "missing web services");

            // UI
            ui->setupUi(this);
            this->setCurrentIndex(0);

            // fix style
            this->tabBar()->setExpanding(false);
            this->tabBar()->setUsesScrollButtons(true);

            // overlay
            this->setOverlaySizeFactors(0.8, 0.9);
            this->setReducedInfo(true);
            this->setForceSmall(true);
            this->showKillButton(false);

            // validators
            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            ui->le_Callsign->setValidator(ucv);
            ui->le_AircraftType->setValidator(ucv);
            ui->le_DestinationAirport->setValidator(ucv);
            ui->le_AlternateAirport->setValidator(ucv);
            ui->le_OriginAirport->setValidator(ucv);

            ucv = new CUpperCaseValidator(true, 0, 1, ui->le_EquipmentSuffix);
            ucv->setRestrictions(CFlightPlan::equipmentCodes());
            ui->le_EquipmentSuffix->setValidator(ucv);
            ui->le_EquipmentSuffix->setToolTip(ui->tbr_EquipmentCodes->toHtml());
            QCompleter *completer = new QCompleter(CFlightPlan::equipmentCodesInfo(), ui->le_EquipmentSuffix);
            completer->setMaxVisibleItems(10);
            completer->popup()->setMinimumWidth(225);
            completer->setCompletionMode(QCompleter::PopupCompletion);
            ui->le_EquipmentSuffix->setCompleter(completer);

            // connect
            connect(ui->pb_Send, &QPushButton::pressed, this, &CFlightPlanComponent::sendFlightPlan);
            connect(ui->pb_Load, &QPushButton::pressed, this, &CFlightPlanComponent::loadFlightPlanFromNetwork);
            connect(ui->pb_Reset, &QPushButton::pressed, this, &CFlightPlanComponent::resetFlightPlan);
            connect(ui->pb_ValidateFlightPlan, &QPushButton::pressed, this, &CFlightPlanComponent::validateFlightPlan);
            connect(ui->pb_Prefill, &QPushButton::pressed, this, &CFlightPlanComponent::anticipateValues);

            connect(ui->cb_VoiceCapabilities, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks);
            connect(ui->cb_VoiceCapabilities, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::syncVoiceComboBoxes);
            connect(ui->cb_VoiceCapabilitiesFirstPage, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::syncVoiceComboBoxes);
            connect(ui->cb_NavigationEquipment, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks);
            connect(ui->cb_PerformanceCategory, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks);
            connect(ui->cb_PilotRating, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks);
            connect(ui->cb_RequiredNavigationPerformance, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks);

            connect(ui->pb_LoadDisk, &QPushButton::clicked, this, &CFlightPlanComponent::loadFromDisk);
            connect(ui->pb_SaveDisk, &QPushButton::clicked, this, &CFlightPlanComponent::saveToDisk);

            connect(ui->le_AircraftRegistration, &QLineEdit::textChanged, this, &CFlightPlanComponent::buildRemarksString);
            connect(ui->le_AirlineOperator, &QLineEdit::textChanged, this, &CFlightPlanComponent::buildRemarksString);
            connect(ui->cb_NoSidsStarts, &QCheckBox::released, this, &CFlightPlanComponent::buildRemarksString);

            connect(ui->pte_AdditionalRemarks, &QPlainTextEdit::textChanged, this, &CFlightPlanComponent::buildRemarksString);
            connect(ui->frp_SelcalCode, &CSelcalCodeSelector::valueChanged, this, &CFlightPlanComponent::buildRemarksString);
            connect(ui->frp_SelcalCode, &CSelcalCodeSelector::valueChanged, this, &CFlightPlanComponent::setSelcalInOwnAircraft);
            connect(ui->pb_CopyOver, &QPushButton::pressed, this, &CFlightPlanComponent::copyRemarksConfirmed);
            connect(ui->pb_GetFromGenerator, &QPushButton::pressed, this, &CFlightPlanComponent::copyRemarksConfirmed);
            connect(ui->pb_RemarksGenerator, &QPushButton::clicked, this, &CFlightPlanComponent::currentTabGenerator);
            connect(ui->tb_HelpEquipment, &QToolButton::clicked, this, &CFlightPlanComponent::showEquipmentCodesTab);

            connect(ui->le_AircraftType, &QLineEdit::editingFinished, this, &CFlightPlanComponent::aircraftTypeChanged);
            connect(ui->le_EquipmentSuffix, &QLineEdit::editingFinished, this, &CFlightPlanComponent::buildPrefixIcaoSuffix);
            connect(ui->cb_Heavy, &QCheckBox::released, this, &CFlightPlanComponent::prefixCheckBoxChanged);
            connect(ui->cb_Tcas, &QCheckBox::released, this, &CFlightPlanComponent::prefixCheckBoxChanged);

            // web services
            connect(sGui->getWebDataServices(), &CWebDataServices::allSwiftDbDataRead, this, &CFlightPlanComponent::swiftWebDataRead);

            // init GUI
            this->resetFlightPlan();
            this->buildRemarksString();

            // prefill some data derived from what was used last
            if (sGui->getIContextSimulator()->isSimulatorSimulating())
            {
                this->prefillWithOwnAircraftData();
            }
            else
            {
                const CAircraftModel model = m_lastAircraftModel.get();
                const CServer server = m_lastServer.get();
                CSimulatedAircraft aircraft(model);
                aircraft.setPilot(server.getUser());
                this->prefillWithAircraftData(aircraft);
            }
        }

        CFlightPlanComponent::~CFlightPlanComponent()
        { }

        void CFlightPlanComponent::loginDataSet()
        {
            if (m_sentFlightPlan.wasSentOrLoaded())  { return; } // when loaded or sent do not override
            this->prefillWithOwnAircraftData();
        }

        void CFlightPlanComponent::prefillWithOwnAircraftData()
        {
            if (!sGui->getIContextOwnAircraft()) { return; }
            if (!sGui->getIContextSimulator())   { return; }
            if (!sGui->getIContextSimulator()->isSimulatorAvailable()) { return; }
            const CSimulatedAircraft ownAircraft(sGui->getIContextOwnAircraft()->getOwnAircraft());
            this->prefillWithAircraftData(ownAircraft);
        }

        void CFlightPlanComponent::prefillWithAircraftData(const CSimulatedAircraft &aircraft)
        {
            if (m_sentFlightPlan.wasSentOrLoaded()) { return; }

            // only override with valid values
            if (CCallsign::isValidAircraftCallsign(aircraft.getCallsignAsString()))
            {
                ui->le_Callsign->setText(aircraft.getCallsign().asString());
            }
            if (CAircraftIcaoCode::isValidDesignator(aircraft.getAircraftIcaoCodeDesignator()))
            {
                ui->le_AircraftType->setText(aircraft.getAircraftIcaoCodeDesignator());
            }
            m_model = aircraft.getModel();
            this->prefillWithUserData(aircraft.getPilot());
            this->buildPrefixIcaoSuffix();
        }

        void CFlightPlanComponent::prefillWithUserData(const CUser &user)
        {
            if (m_sentFlightPlan.wasSentOrLoaded()) { return; }

            // only override with valid values
            if (user.hasRealName())
            {
                ui->le_PilotsName->setText(user.getRealName());
            }
            if (user.hasHomeBase())
            {
                ui->le_PilotsHomeBase->setText(user.getHomeBase().getIcaoCode());
            }
            if (user.hasCallsign())
            {
                ui->le_Callsign->setText(user.getCallsign().asString());
            }
        }

        void CFlightPlanComponent::fillWithFlightPlanData(const BlackMisc::Aviation::CFlightPlan &flightPlan)
        {
            ui->le_AlternateAirport->setText(flightPlan.getAlternateAirportIcao().asString());
            ui->le_DestinationAirport->setText(flightPlan.getDestinationAirportIcao().asString());
            ui->le_OriginAirport->setText(flightPlan.getOriginAirportIcao().asString());
            ui->pte_Route->setPlainText(flightPlan.getRoute());
            ui->pte_Remarks->setPlainText(flightPlan.getRemarks());
            ui->le_TakeOffTimePlanned->setText(flightPlan.getTakeoffTimePlannedHourMin());
            ui->le_FuelOnBoard->setText(flightPlan.getFuelTimeHourMin());
            ui->le_EstimatedTimeEnroute->setText(flightPlan.getEnrouteTimeHourMin());
            ui->le_CruiseTrueAirspeed->setText(flightPlan.getCruiseTrueAirspeed().valueRoundedWithUnit(CSpeedUnit::kts(), 0));

            const CAltitude cruiseAlt = flightPlan.getCruiseAltitude();
            ui->lep_CrusingAltitude->setAltitude(cruiseAlt);

            switch (flightPlan.getFlightRulesAsVFRorIFR())
            {
            case CFlightPlan::VFR: ui->cb_FlightRule->setCurrentText("VFR"); break;
            default: ui->cb_FlightRule->setCurrentText("IFR"); break;
            }
        }

        const CLogCategoryList &CFlightPlanComponent::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::flightPlan(), CLogCategory::guiComponent() };
            return cats;
        }

        CStatusMessageList CFlightPlanComponent::validateAndInitializeFlightPlan(CFlightPlan &flightPlan)
        {
            CStatusMessageList messages;
            const bool strict = ui->cb_StrictCheck->isChecked();
            const bool vfr = this->isVfr();
            const CStatusMessage::StatusSeverity severity = strict ? CStatusMessage::SeverityError : CStatusMessage::SeverityWarning;
            messages.push_back(CStatusMessage(this).validationInfo(strict ? "Strict validation" : "Lenient validation"));

            const CFlightPlan::FlightRules rule = ui->cb_FlightRule->currentText().startsWith("I") ? CFlightPlan::IFR : CFlightPlan::VFR;
            flightPlan.setFlightRule(rule);

            // callsign
            QString v;
            v = ui->le_Callsign->text().trimmed().toUpper();
            if (v.isEmpty())
            {
                messages.push_back(CStatusMessage(this).validationError("Missing '%1'") << ui->lbl_Callsign->text());
            }
            else if (!CCallsign::isValidAircraftCallsign(v))
            {
                messages.push_back(CStatusMessage(this).validationError("Invalid callsign '%1'") << v);
            }
            flightPlan.setCallsign(CCallsign(v));

            // aircraft ICAO / aircraft type
            v = ui->le_AircraftType->text().trimmed().toUpper();
            if (v.isEmpty())
            {
                messages.push_back(CStatusMessage(this).validationError("Missing '%1'") << ui->lbl_AircraftType->text());
            }
            else if (!CAircraftIcaoCode::isValidDesignator(v))
            {
                messages.push_back(CStatusMessage(this).validationError("Invalid aircraft ICAO code '%1'") << v);
            }
            else if (sApp && sApp->hasWebDataServices() && sApp->getWebDataServices()->hasDbAircraftData() && !sApp->getWebDataServices()->containsAircraftIcaoDesignator(v))
            {
                messages.push_back(CStatusMessage(this).validationWarning("Are you sure '%1' is a valid type?") << v);
            }
            flightPlan.setAircraftIcao(this->getAircraftIcaoCode());

            // prefix / equipment code
            v = this->getPrefix();
            if (!v.isEmpty() && !CFlightPlan::prefixCodes().contains(v))
            {
                messages.push_back(CStatusMessage(this).validation(severity, "Invalid prefix"));
            }
            flightPlan.setPrefix(v);

            v = this->getEquipmentSuffix();
            if (!v.isEmpty() && !CFlightPlan::equipmentCodes().contains(v))
            {
                messages.push_back(CStatusMessage(this).validation(severity, "Invalid equipment code"));
            }
            flightPlan.setEquipmentSuffix(v);

            // route
            v = ui->pte_Route->toPlainText().trimmed();
            if (v.isEmpty())
            {
                messages.push_back(CStatusMessage(this).validation(
                                       vfr ?
                                       CStatusMessage::SeverityInfo :
                                       CStatusMessage::SeverityWarning, "Missing '%1'") << ui->lbl_Route->text());
            }
            else if (v.length() > CFlightPlan::MaxRouteLength)
            {
                messages.push_back(CStatusMessage(this).validationError("Flight plan route length exceeded (%1 chars max.)") << CFlightPlan::MaxRouteLength);
            }
            else
            {
                flightPlan.setRoute(v);
            }

            // remarks
            v = ui->pte_Remarks->toPlainText().trimmed();
            if (v.isEmpty())
            {
                messages.push_back(CStatusMessage(this).validationError("No '%1', voice capabilities are mandatory") << ui->lbl_Remarks->text());
            }
            else if (v.length() > CFlightPlan::MaxRemarksLength)
            {
                messages.push_back(CStatusMessage(this).validationError("Flight plan remarks length exceeded (%1 chars max.)") << CFlightPlan::MaxRemarksLength);
            }
            else
            {
                flightPlan.setRemarks(v);
            }

            // time enroute
            v = ui->le_EstimatedTimeEnroute->text();
            if (v.isEmpty() || v == defaultTime())
            {
                messages.push_back(CStatusMessage(this).validation(severity, "Missing '%1'") << ui->lbl_EstimatedTimeEnroute->text());
            }
            flightPlan.setEnrouteTime(v);

            // fuel
            v = ui->le_FuelOnBoard->text();
            if (v.isEmpty() || v == defaultTime())
            {
                messages.push_back(CStatusMessage(this).validation(severity, "Missing '%1'") << ui->lbl_FuelOnBoard->text());
            }
            flightPlan.setFuelTime(v);

            // take off time
            v = ui->le_TakeOffTimePlanned->text();
            if (v.isEmpty() || v == defaultTime())
            {
                messages.push_back(CStatusMessage(this).validation(severity, "Missing '%1'") << ui->lbl_TakeOffTimePlanned->text());
            }
            flightPlan.setTakeoffTimePlanned(v);

            // cruising alt
            if (ui->lep_CrusingAltitude->isValid(&messages))
            {
                const CAltitude cruisingAltitude = ui->lep_CrusingAltitude->getAltitude();
                flightPlan.setCruiseAltitude(cruisingAltitude);
            }

            // destination airport
            v = ui->le_DestinationAirport->text();
            if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
            {
                messages.push_back(CStatusMessage(this).validationError("Missing '%1'") << ui->lbl_DestinationAirport->text());
                flightPlan.setDestinationAirportIcao(QString(""));
            }
            else
            {
                flightPlan.setDestinationAirportIcao(v);
            }

            // origin airport
            v = ui->le_OriginAirport->text();
            if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
            {
                messages.push_back(CStatusMessage(this).validationError("Missing '%1'") << ui->lbl_OriginAirport->text());
                flightPlan.setOriginAirportIcao(defaultIcao());
            }
            else
            {
                flightPlan.setOriginAirportIcao(v);
            }

            // TAS
            v = ui->le_CruiseTrueAirspeed->text();
            CSpeed cruiseTAS;
            cruiseTAS.parseFromString(v, CPqString::SeparatorsLocale);
            if (cruiseTAS.isNull())
            {
                messages.push_back(CStatusMessage(this).validationError("Wrong TAS, %1") << ui->lbl_CruiseTrueAirspeed->text());
                flightPlan.setDestinationAirportIcao(defaultIcao());
            }
            else
            {
                flightPlan.setCruiseTrueAirspeed(cruiseTAS);
            }

            // Optional fields
            v = ui->le_AlternateAirport->text();
            if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
            {
                if (!messages.hasWarningOrErrorMessages())
                {
                    messages.push_back(CStatusMessage(this).validationInfo("Missing %1") << ui->lbl_AlternateAirport->text());
                }
                flightPlan.setAlternateAirportIcao(QString(""));
            }
            else
            {
                flightPlan.setAlternateAirportIcao(v);
            }

            // OK
            if (!messages.isFailure())
            {
                messages.push_back(CStatusMessage(this).validationInfo("Flight plan validation passed"));
            }
            return messages;
        }

        void CFlightPlanComponent::sendFlightPlan()
        {
            CFlightPlan flightPlan;
            const CStatusMessageList messages = this->validateAndInitializeFlightPlan(flightPlan);
            if (messages.isSuccess())
            {
                // no error, send if possible
                CStatusMessage m;
                QString lastSent;
                if (sGui && sGui->getIContextNetwork()->isConnected())
                {
                    flightPlan.setWhenLastSentOrLoaded(QDateTime::currentDateTimeUtc());
                    sGui->getIContextNetwork()->sendFlightPlan(flightPlan);
                    lastSent = flightPlan.whenLastSentOrLoaded().toString();
                    m = CStatusMessage(this).validationInfo("Sent flight plan");
                }
                else
                {
                    flightPlan.setWhenLastSentOrLoaded(QDateTime()); // empty
                    m = CStatusMessage(this).validationError("No errors, but not connected, cannot send flight plan");
                }
                ui->le_LastSent->setText(lastSent);
                this->showOverlayMessage(m, OverlayMessageMs);
                m_sentFlightPlan = flightPlan; // last valid FP
            }
            else
            {
                this->showOverlayMessages(messages);
            }
        }

        void CFlightPlanComponent::validateFlightPlan()
        {
            CFlightPlan flightPlan;
            const CStatusMessageList messages = this->validateAndInitializeFlightPlan(flightPlan);
            this->showOverlayMessages(messages);
        }

        void CFlightPlanComponent::resetFlightPlan()
        {
            this->prefillWithOwnAircraftData();
            ui->le_AircraftRegistration->clear();
            ui->le_AirlineOperator->clear();
            ui->lep_CrusingAltitude->setText("FL70");
            ui->le_CruiseTrueAirspeed->setText("100 kts");
            ui->pte_Remarks->clear();
            ui->pte_Route->clear();
            ui->le_AlternateAirport->clear();
            ui->le_DestinationAirport->clear();
            ui->le_OriginAirport->clear();
            ui->le_FuelOnBoard->setText(defaultTime());
            ui->le_EstimatedTimeEnroute->setText(defaultTime());
            ui->le_TakeOffTimePlanned->setText(QDateTime::currentDateTimeUtc().addSecs(30 * 60).toString("hh:mm"));
            this->syncVoiceComboBoxes(ui->cb_VoiceCapabilities->itemText(0));
        }

        void CFlightPlanComponent::loadFromDisk()
        {
            CStatusMessage m;
            const QString fileName = QFileDialog::getOpenFileName(nullptr,
                                     tr("Load flight plan"), getDefaultFilename(true),
                                     tr("swift (*.json *.txt)"));
            do
            {
                if (fileName.isEmpty())
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityDebug, "Load canceled", true);
                    break;
                }

                const QString json(CFileUtils::readFileToString(fileName));
                if (!Json::looksLikeSwiftJson(json))
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityWarning, "Reading '%1' yields no data", true) << fileName;
                    break;
                }

                try
                {
                    CVariant variant;
                    variant.convertFromJson(Json::jsonObjectFromString(json));
                    if (variant.canConvert<CFlightPlan>())
                    {
                        const CFlightPlan fp = variant.value<CFlightPlan>();
                        this->fillWithFlightPlanData(fp);
                    }
                    else
                    {
                        m = CStatusMessage(this, CStatusMessage::SeverityWarning, "Wrong format for flight plan in '%1'") << fileName;
                    }
                }
                catch (const CJsonException &ex)
                {
                    m = ex.toStatusMessage(this, "Parse error in " + fileName);
                    break;
                }
            }
            while (false);
            if (m.isFailure()) { CLogMessage::preformatted(m); }
        }

        void CFlightPlanComponent::saveToDisk()
        {
            CStatusMessage m;
            const QString fileName = QFileDialog::getSaveFileName(nullptr,
                                     tr("Save flight plan"), getDefaultFilename(false),
                                     tr("swift (*.json *.txt)"));
            do
            {
                if (fileName.isEmpty())
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityDebug, "Save canceled", true);
                    break;
                }
                CFlightPlan fp;
                this->validateAndInitializeFlightPlan(fp); // get data
                const CVariant variantFp = CVariant::fromValue(fp);
                const QString json(variantFp.toJsonString());
                const bool ok = CFileUtils::writeStringToFile(json, fileName);
                if (ok)
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityInfo, "Written " + fileName, true);
                }
                else
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityError, "Writing " + fileName + " failed", true);
                }
            }
            while (false);
            if (m.isFailure()) { CLogMessage::preformatted(m); }
        }

        void CFlightPlanComponent::setSelcalInOwnAircraft()
        {
            if (!sGui->getIContextOwnAircraft()) return;
            if (!ui->frp_SelcalCode->hasValidCode()) return;
            sGui->getIContextOwnAircraft()->updateSelcal(ui->frp_SelcalCode->getSelcal(), flightPlanIdentifier());
        }

        void CFlightPlanComponent::loadFlightPlanFromNetwork()
        {
            if (!sGui->getIContextNetwork() || !sGui->getIContextNetwork()->isConnected())
            {
                CLogMessage(this).warning("Cannot load flight plan, network not connected");
                return;
            }

            const CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
            const CFlightPlan loadedPlan = sGui->getIContextNetwork()->loadFlightPlanFromNetwork(ownAircraft.getCallsign());
            if (loadedPlan.wasSentOrLoaded())
            {
                const int r = QMessageBox::warning(this, "Override current data?", "Loaded FP", QMessageBox::Yes | QMessageBox::No);
                if (r != QMessageBox::Yes) { return; }
                this->fillWithFlightPlanData(loadedPlan);
                CLogMessage(this).info("Updated with loaded flight plan");
            }
            else
            {
                CLogMessage(this).warning("No flight plan data");
            }
        }

        void CFlightPlanComponent::buildRemarksString()
        {
            QString v = ui->cb_VoiceCapabilities->currentText().toUpper();
            QString rem = CFlightPlanRemarks::textToVoiceCapabilities(v);

            v = ui->le_AirlineOperator->text().trimmed();
            if (!v.isEmpty()) rem.append("OPR/").append(v).append(" ");

            v = ui->le_AircraftRegistration->text().trimmed();
            if (!v.isEmpty()) rem.append("REG/").append(v).append(" ");

            v = ui->cb_PilotRating->currentText().toUpper();
            if (v.contains("P1"))      { rem.append("PR/P1 "); }
            else if (v.contains("P2")) { rem.append("PR/P2 "); }
            else if (v.contains("P3")) { rem.append("PR/P3 "); }
            else if (v.contains("P4")) { rem.append("PR/P4 "); }
            else if (v.contains("P5")) { rem.append("PR/P5 "); }

            v = ui->cb_RequiredNavigationPerformance->currentText().toUpper();
            if (v.contains("10"))     { rem.append("RNP10 "); }
            else if (v.contains("4")) { rem.append("RNP4 "); }

            v = ui->cb_NavigationEquipment->currentText().toUpper();
            if (v.contains("VORS"))         { rem.append("NAV/VORNDB "); }
            else if (v.contains("SIDS"))    { rem.append("NAV/GPSRNAV "); }
            if (v.contains("DEFAULT"))      { rem.append("NAV/GPS "); }
            else if (v.contains("OCEANIC")) { rem.append("NAV/GPSOCEANIC "); }

            v = ui->cb_PerformanceCategory->currentText().toUpper();
            if (v.startsWith("A"))      { rem.append("PER/A "); }
            else if (v.startsWith("B")) { rem.append("PER/B "); }
            else if (v.startsWith("C")) { rem.append("PER/C "); }
            else if (v.startsWith("D")) { rem.append("PER/D "); }
            else if (v.startsWith("E")) { rem.append("PER/E "); }

            if (ui->frp_SelcalCode->hasValidCode())
            {
                rem.append("SEL/").append(ui->frp_SelcalCode->getSelcalCode());
                rem.append(" ");
            }

            if (ui->cb_NoSidsStarts->isChecked()) { rem.append("NO SID/STAR "); }

            v = ui->pte_AdditionalRemarks->toPlainText().trimmed();
            if (!v.isEmpty()) { rem.append(v); }

            rem = rem.simplified().trimmed();
            ui->pte_RemarksGenerated->setPlainText(rem);
        }

        void CFlightPlanComponent::copyRemarks(bool confirm)
        {
            const QString generated = ui->pte_RemarksGenerated->toPlainText().trimmed();
            if (confirm && !this->overrideRemarks()) { return; }
            ui->pte_Remarks->setPlainText(generated);
            CLogMessage(this).info("Copied remarks");
        }

        void CFlightPlanComponent::currentTabGenerator()
        {
            this->setCurrentWidget(ui->tb_RemarksGenerator);
        }

        void CFlightPlanComponent::swiftWebDataRead()
        {
            this->initCompleters();
        }

        void CFlightPlanComponent::buildPrefixIcaoSuffix()
        {
            ui->le_PrefixIcaoSuffix->setText(this->getCombinedPrefixIcaoSuffix());
        }

        void CFlightPlanComponent::prefixCheckBoxChanged()
        {
            QObject *sender = QObject::sender();
            if (sender == ui->cb_Heavy)
            {
                if (ui->cb_Heavy->isChecked())
                {
                    QTimer::singleShot(10, this, [ = ]
                    {
                        ui->cb_Tcas->setChecked(false);
                        this->buildPrefixIcaoSuffix();
                    });
                    return;
                }
            }
            else if (sender == ui->cb_Tcas)
            {
                if (ui->cb_Tcas->isChecked())
                {
                    QTimer::singleShot(10, this, [ = ]
                    {
                        ui->cb_Heavy->setChecked(false);
                        this->buildPrefixIcaoSuffix();
                    });
                    return;
                }
            }
            this->buildPrefixIcaoSuffix();
        }

        void CFlightPlanComponent::aircraftTypeChanged()
        {
            const CAircraftIcaoCode icao = this->getAircraftIcaoCode();
            if (!icao.isLoadedFromDb()) { return; }
            QTimer::singleShot(25, this, [ = ]
            {
                const bool heavy = icao.getWtc().startsWith("H");
                ui->cb_Heavy->setChecked(heavy);
                if (heavy) { ui->cb_Tcas->setChecked(false); }
                this->buildPrefixIcaoSuffix();
            });
        }

        QString CFlightPlanComponent::getPrefix() const
        {
            if (ui->cb_Heavy->isChecked()) { return QStringLiteral("H"); }
            else if (ui->cb_Tcas->isChecked()) { return QStringLiteral("T"); }
            return QStringLiteral("");
        }

        CAircraftIcaoCode CFlightPlanComponent::getAircraftIcaoCode() const
        {
            const QString designator(ui->le_AircraftType->text());
            if (!sApp || !sApp->hasWebDataServices() || !CAircraftIcaoCode::isValidDesignator(designator)) { return CAircraftIcaoCode(); }
            return sApp->getWebDataServices()->getAircraftIcaoCodeForDesignator(designator);
        }

        QString CFlightPlanComponent::getEquipmentSuffix() const
        {
            return ui->le_EquipmentSuffix->text().trimmed().toUpper();
        }

        QString CFlightPlanComponent::getCombinedPrefixIcaoSuffix() const
        {
            return CFlightPlan::concatPrefixIcaoSuffix(this->getPrefix(), this->getAircraftIcaoCode().getDesignator(), this->getEquipmentSuffix());
        }

        void CFlightPlanComponent::showEquipmentCodesTab()
        {
            this->setCurrentWidget(ui->tb_EquipmentCodes);
        }

        bool CFlightPlanComponent::isVfr() const
        {
            const bool vfr = ui->cb_FlightRule->currentText().startsWith("V", Qt::CaseInsensitive);
            return vfr;
        }

        bool CFlightPlanComponent::overrideRemarks()
        {
            if (!ui->pte_Remarks->toPlainText().trimmed().isEmpty())
            {
                const int reply = QMessageBox::question(this, "Remarks", "Override existing remarks?", QMessageBox::Yes | QMessageBox::No);
                if (reply != QMessageBox::Yes) { return false; }
            }
            return true;
        }

        void CFlightPlanComponent::anticipateValues()
        {
            if (!this->overrideRemarks()) { return; }
            CStatusMessageList msgs;
            const bool vfr = this->isVfr();
            const bool airline = m_model.hasAirlineDesignator();

            if (vfr)
            {
                ui->cb_NoSidsStarts->setChecked(true);
                msgs.push_back(CStatusMessage(this).validationInfo("No SID/STARs"));
                ui->cb_RequiredNavigationPerformance->setCurrentIndex(0);
                ui->cb_PerformanceCategory->setCurrentIndex(0);
                ui->cb_NavigationEquipment->setCurrentIndex(0);
                msgs.push_back(CStatusMessage(this).validationInfo("Set navigation and performance to VFR"));
            }
            else
            {
                // IFR
                const CAircraftIcaoCode icao = this->getAircraftIcaoCode();
                if (icao.isLoadedFromDb())
                {
                    if (icao.getEngineCount() >= 2 && icao.getEngineType() == "J")
                    {
                        // jet with >=2 engines
                        msgs.push_back(CStatusMessage(this).validationInfo("Jet >=2 engines"));
                        msgs.push_back(CStatusMessage(this).validationInfo("SID/STARs"));
                        ui->cb_NoSidsStarts->setChecked(false);
                        ui->cb_NavigationEquipment->setCurrentText("GPS or FMC capable of SIDs/STARs");
                        msgs.push_back(CStatusMessage(this).validationInfo("GPS or FMC capable of SIDs/STARs"));

                        // reset those values
                        ui->cb_RequiredNavigationPerformance->setCurrentIndex(0);
                        ui->cb_PerformanceCategory->setCurrentIndex(0);
                    }
                } // ICAO
            }

            // further info if having model from DB
            if (m_model.isLoadedFromDb())
            {
                if (airline)
                {
                    ui->le_AirlineOperator->setText(m_model.getAirlineIcaoCode().getName());
                }
            }

            // messages
            this->showOverlayMessages(msgs, false, OverlayMessageMs);

            // copy over
            if (msgs.isSuccess())
            {
                this->buildRemarksString();
                this->copyRemarks(false);
            }
        }

        void CFlightPlanComponent::initCompleters()
        {
            if (!sGui || !sGui->hasWebDataServices()) { return; }
            const QStringList aircraft(sGui->getWebDataServices()->getAircraftIcaoCodes().allIcaoCodes().toList());
            ui->le_AircraftType->setCompleter(new QCompleter(aircraft, this));

            const QStringList airports = sGui->getWebDataServices()->getAirports().allIcaoCodes(true);
            QCompleter *airportCompleter = new QCompleter(airports, this);
            ui->le_AlternateAirport->setCompleter(airportCompleter);
            ui->le_DestinationAirport->setCompleter(airportCompleter);
            ui->le_OriginAirport->setCompleter(airportCompleter);
        }

        QString CFlightPlanComponent::getDefaultFilename(bool load)
        {
            // some logic to find a useful default name
            const QString dir = CDirectoryUtils::documentationDirectory();
            if (load) { return CFileUtils::appendFilePaths(dir, CFileUtils::jsonWildcardAppendix()); }

            // Save file path
            QString name("Flight plan");
            if (!ui->le_DestinationAirport->text().isEmpty() && !ui->le_OriginAirport->text().isEmpty())
            {
                name += QStringLiteral(" ") % ui->le_OriginAirport->text() %
                        QStringLiteral("-") % ui->le_DestinationAirport->text();
            }

            if (!name.endsWith(CFileUtils::jsonAppendix(), Qt::CaseInsensitive))
            {
                name += CFileUtils::jsonAppendix();
            }
            return CFileUtils::appendFilePaths(dir, name);
        }

        void CFlightPlanComponent::syncVoiceComboBoxes(const QString &text)
        {
            const QObject *sender = QObject::sender();
            if (sender == ui->cb_VoiceCapabilities)
            {
                ui->cb_VoiceCapabilitiesFirstPage->setCurrentText(text);
            }
            else
            {
                ui->cb_VoiceCapabilities->setCurrentText(text);
                const QString r = CFlightPlanRemarks::replaceVoiceCapabilities(CFlightPlanRemarks::textToVoiceCapabilities(text), ui->pte_Remarks->toPlainText());
                ui->pte_Remarks->setPlainText(r);
            }
        }
    } // namespace
} // namespace
