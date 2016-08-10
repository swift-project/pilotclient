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
#include "blackcore/webdataservices.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/components/flightplancomponent.h"
#include "blackgui/components/selcalcodeselector.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airporticaocode.h"
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
#include "blackmisc/statusmessage.h"
#include "ui_flightplancomponent.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExp>
#include <QTabBar>
#include <QCompleter>
#include <Qt>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
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
            QTabWidget(parent),
            ui(new Ui::CFlightPlanComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "missing sGui");

            // UI
            ui->setupUi(this);

            // fix style
            this->tabBar()->setExpanding(false);

            // validators
            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            ui->le_Callsign->setValidator(ucv);
            ui->le_AircraftType->setValidator(ucv);
            ui->le_DestinationAirport->setValidator(ucv);
            ui->le_AlternateAirport->setValidator(ucv);
            ui->le_OriginAirport->setValidator(ucv);

            // connect
            connect(this->ui->pb_Send, &QPushButton::pressed, this, &CFlightPlanComponent::ps_sendFlightPlan);
            connect(this->ui->pb_Load, &QPushButton::pressed, this, &CFlightPlanComponent::ps_loadFlightPlanFromNetwork);
            connect(this->ui->pb_Reset, &QPushButton::pressed, this, &CFlightPlanComponent::ps_resetFlightPlan);
            connect(this->ui->pb_ValidateFlightPlan, &QPushButton::pressed, this, &CFlightPlanComponent::ps_validateFlightPlan);

            connect(this->ui->cb_VoiceCapabilities, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::ps_currentTextChangedToBuildRemarks);
            connect(this->ui->cb_NavigationEquipment, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::ps_currentTextChangedToBuildRemarks);
            connect(this->ui->cb_PerformanceCategory, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::ps_currentTextChangedToBuildRemarks);
            connect(this->ui->cb_PilotRating, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::ps_currentTextChangedToBuildRemarks);
            connect(this->ui->cb_RequiredNavigationPerformance, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::ps_currentTextChangedToBuildRemarks);

            connect(this->ui->pb_LoadDisk, &QPushButton::clicked, this, &CFlightPlanComponent::ps_loadFromDisk);
            connect(this->ui->pb_SaveDisk, &QPushButton::clicked, this, &CFlightPlanComponent::ps_saveToDisk);

            bool c = connect(this->ui->le_AircraftRegistration, SIGNAL(textChanged(QString)), this, SLOT(ps_buildRemarksString()));
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            c = connect(this->ui->cb_NoSidsStarts, SIGNAL(toggled(bool)), this, SLOT(ps_buildRemarksString()));
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            c = connect(this->ui->le_AirlineOperator, SIGNAL(textChanged(QString)), this, SLOT(ps_buildRemarksString()));
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            Q_UNUSED(c);

            connect(this->ui->pte_AdditionalRemarks, &QPlainTextEdit::textChanged, this, &CFlightPlanComponent::ps_buildRemarksString);
            connect(this->ui->frp_SelcalCode, &CSelcalCodeSelector::valueChanged, this, &CFlightPlanComponent::ps_buildRemarksString);
            connect(this->ui->frp_SelcalCode, &CSelcalCodeSelector::valueChanged, this, &CFlightPlanComponent::ps_setSelcalInOwnAircraft);
            connect(this->ui->pb_CopyOver, &QPushButton::pressed, this, &CFlightPlanComponent::ps_copyRemarks);
            connect(this->ui->pb_RemarksGenerator, &QPushButton::clicked, this, &CFlightPlanComponent::ps_currentTabGenerator);

            // web services
            connect(sGui->getWebDataServices(), &CWebDataServices::allSwiftDbDataRead, this, &CFlightPlanComponent::ps_swiftDataRead);

            // init GUI
            this->ps_resetFlightPlan();
            this->ps_buildRemarksString();
        }

        CFlightPlanComponent::~CFlightPlanComponent()
        { }

        void CFlightPlanComponent::loginDataSet()
        {
            if (this->m_flightPlan.wasSentOrLoaded()) { return; } // when loaded or sent do not override
            if (!sGui->getIContextOwnAircraft()) { return; }
            this->prefillWithAircraftData(sGui->getIContextOwnAircraft()->getOwnAircraft());
        }

        void CFlightPlanComponent::prefillWithAircraftData(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft)
        {
            // only override with valid values
            if (CCallsign::isValidAircraftCallsign(ownAircraft.getCallsignAsString()))
            {
                this->ui->le_Callsign->setText(ownAircraft.getCallsign().asString());
            }
            if (CAircraftIcaoCode::isValidDesignator(ownAircraft.getAircraftIcaoCodeDesignator()))
            {
                this->ui->le_AircraftType->setText(ownAircraft.getAircraftIcaoCodeDesignator());
            }
            if (ownAircraft.hasValidRealName())
            {
                this->ui->le_PilotsName->setText(ownAircraft.getPilot().getRealName());
            }
        }

        void CFlightPlanComponent::fillWithFlightPlanData(const BlackMisc::Aviation::CFlightPlan &flightPlan)
        {
            this->ui->le_AlternateAirport->setText(flightPlan.getAlternateAirportIcao().asString());
            this->ui->le_DestinationAirport->setText(flightPlan.getDestinationAirportIcao().asString());
            this->ui->le_OriginAirport->setText(flightPlan.getOriginAirportIcao().asString());
            this->ui->pte_Route->setPlainText(flightPlan.getRoute());
            this->ui->pte_Remarks->setPlainText(flightPlan.getRemarks());
            this->ui->le_TakeOffTimePlanned->setText(flightPlan.getTakeoffTimePlannedHourMin());
            this->ui->le_FuelOnBoard->setText(flightPlan.getFuelTimeHourMin());
            this->ui->le_EstimatedTimeEnroute->setText(flightPlan.getEnrouteTimeHourMin());
            this->ui->le_CruiseTrueAirspeed->setText(flightPlan.getCruiseTrueAirspeed().valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CSpeedUnit::kts(), 0));

            const CAltitude cruiseAlt = flightPlan.getCruiseAltitude();
            if (cruiseAlt.isFlightLevel())
            {
                this->ui->le_CrusingAltitude->setText(cruiseAlt.toQString());
            }
            else
            {
                this->ui->le_CrusingAltitude->setText(cruiseAlt.valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CLengthUnit::ft(), 0));
            }
        }

        CFlightPlan CFlightPlanComponent::getFlightPlan() const
        {
            return this->m_flightPlan;
        }

        BlackMisc::CStatusMessageList CFlightPlanComponent::validateAndInitializeFlightPlan(BlackMisc::Aviation::CFlightPlan &flightPlan)
        {
            BlackMisc::CStatusMessageList messages;
            QString v;

            CFlightPlan::FlightRules rule = CFlightPlan::IFR;
            if (this->ui->rb_TypeIfr->isChecked())
            {
                rule = CFlightPlan::IFR;
            }
            else if (this->ui->rb_TypeVfr->isChecked())
            {
                rule = CFlightPlan::VFR;
            }
            flightPlan.setFlightRule(rule);

            v = ui->le_Callsign->text().trimmed();
            if (v.isEmpty())
            {
                messages.push_back(CLogMessage().validationWarning("Missing %1") << this->ui->lbl_Callsign->text());
            }

            v = ui->pte_Route->toPlainText().trimmed();
            if (v.isEmpty())
            {
                messages.push_back(CLogMessage().validationWarning("Missing flight plan route"));
            }
            else if (v.length() > CFlightPlan::MaxRouteLength)
            {
                messages.push_back(CLogMessage().validationWarning("Flight plan route length exceeded (%1 chars max.)") << CFlightPlan::MaxRouteLength);
            }
            else
            {
                flightPlan.setRoute(v);
            }

            v = ui->pte_Remarks->toPlainText().trimmed();
            if (v.isEmpty())
            {
                messages.push_back(CLogMessage().validationWarning("No remarks, voice capabilities are mandatory"));
            }
            else if (v.length() > CFlightPlan::MaxRemarksLength)
            {
                messages.push_back(CLogMessage().validationWarning("Flight plan remarks length exceeded (%1 chars max.)") << CFlightPlan::MaxRemarksLength);
            }
            else
            {
                flightPlan.setRemarks(v);
            }

            v = ui->le_EstimatedTimeEnroute->text();
            if (v.isEmpty() || v == defaultTime())
            {
                messages.push_back(CLogMessage().validationWarning("Missing %1") << this->ui->lbl_EstimatedTimeEnroute->text());
            }
            else
            {
                flightPlan.setEnrouteTime(v);
            }

            v = ui->le_FuelOnBoard->text();
            if (v.isEmpty() || v == defaultTime())
            {
                messages.push_back(CLogMessage().validationWarning("Missing %1") << this->ui->lbl_FuelOnBorad->text());
            }
            else
            {
                flightPlan.setFuelTime(v);
            }

            v = ui->le_TakeOffTimePlanned->text();
            if (v.isEmpty() || v == defaultTime())
            {
                messages.push_back(CLogMessage().validationWarning("Missing %1") << this->ui->lbl_TakeOffTimePlanned->text());
            }
            else
            {
                flightPlan.setTakeoffTimePlanned(v);
            }

            static const QRegExp withUnit("\\D+");
            v = ui->le_CrusingAltitude->text().trimmed();
            if (!v.isEmpty() && withUnit.indexIn(v) < 0)
            {
                v += "ft";
                this->ui->le_CrusingAltitude->setText(v);
            }

            CAltitude cruisingAltitude(v, CPqString::SeparatorsLocale);
            if (v.isEmpty() || cruisingAltitude.isNull())
            {
                messages.push_back(CLogMessage().validationWarning("Wrong %1") << this->ui->lbl_CrusingAltitude->text());
            }
            else
            {
                flightPlan.setCruiseAltitude(cruisingAltitude);
            }

            v = this->ui->le_DestinationAirport->text();
            if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
            {
                messages.push_back(CLogMessage().validationWarning("Missing %1") << this->ui->lbl_DestinationAirport->text());
                flightPlan.setDestinationAirportIcao(QString(""));
            }
            else
            {
                flightPlan.setDestinationAirportIcao(v);
            }

            v = this->ui->le_CruiseTrueAirspeed->text();
            BlackMisc::PhysicalQuantities::CSpeed cruiseTAS;
            cruiseTAS.parseFromString(v, CPqString::SeparatorsLocale);
            if (cruiseTAS.isNull())
            {
                messages.push_back(CLogMessage().validationWarning("Wrong TAS, %1") << this->ui->lbl_CruiseTrueAirspeed->text());
                flightPlan.setDestinationAirportIcao(defaultIcao());
            }
            else
            {
                flightPlan.setCruiseTrueAirspeed(cruiseTAS);
            }

            v = this->ui->le_OriginAirport->text();
            if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
            {
                messages.push_back(CLogMessage().validationWarning("Missing %1") << this->ui->lbl_OriginAirport->text());
                flightPlan.setOriginAirportIcao(defaultIcao());
            }
            else
            {
                flightPlan.setOriginAirportIcao(v);
            }

            // Optional fields
            v = this->ui->le_AlternateAirport->text();
            if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
            {
                if (!messages.hasWarningOrErrorMessages())
                {
                    messages.push_back(CLogMessage().validationInfo("Missing %1") << this->ui->lbl_AlternateAirport->text());
                }
                flightPlan.setAlternateAirportIcao(QString(""));
            }
            else
            {
                flightPlan.setAlternateAirportIcao(v);
            }

            // OK
            if (!messages.hasWarningOrErrorMessages())
            {
                messages.push_back(CLogMessage().validationInfo("Flight plan validation passed"));
            }
            return messages;
        }

        void CFlightPlanComponent::ps_sendFlightPlan()
        {
            CFlightPlan flightPlan;
            CStatusMessageList messages = this->validateAndInitializeFlightPlan(flightPlan);
            if (messages.isEmpty())
            {
                // no error, send if possible
                if (sGui->getIContextNetwork()->isConnected())
                {
                    flightPlan.setWhenLastSentOrLoaded(QDateTime::currentDateTimeUtc());
                    sGui->getIContextNetwork()->sendFlightPlan(flightPlan);
                    this->ui->le_LastSent->setText(flightPlan.whenLastSentOrLoaded().toString());
                    CLogMessage(this).info("Sent flight plan");
                }
                else
                {
                    flightPlan.setWhenLastSentOrLoaded(QDateTime()); // empty
                    this->ui->le_LastSent->clear();
                    CLogMessage(this).error("No errors, but not connected, cannot send flight plan");
                }
                this->m_flightPlan = flightPlan; // last valid FP
            }
        }

        void CFlightPlanComponent::ps_validateFlightPlan()
        {
            CFlightPlan flightPlan;
            CStatusMessageList messages = this->validateAndInitializeFlightPlan(flightPlan);
            messages.addCategories(this);
            messages.addCategory(CLogCategory::validation());
            CLogMessage::preformatted(messages);
        }

        void CFlightPlanComponent::ps_resetFlightPlan()
        {
            Q_ASSERT(sGui->getIContextNetwork());
            Q_ASSERT(sGui->getIContextOwnAircraft());
            if (sGui->getIContextOwnAircraft()) { this->prefillWithAircraftData(sGui->getIContextOwnAircraft()->getOwnAircraft()); }
            this->ui->le_AircraftRegistration->clear();
            this->ui->le_AirlineOperator->clear();
            this->ui->le_CrusingAltitude->setText("FL70");
            this->ui->le_CruiseTrueAirspeed->setText("100 kts");
            this->ui->pte_Remarks->clear();
            this->ui->pte_Route->clear();
            this->ui->le_AlternateAirport->clear();
            this->ui->le_DestinationAirport->clear();
            this->ui->le_OriginAirport->clear();
            this->ui->le_FuelOnBoard->setText(defaultTime());
            this->ui->le_EstimatedTimeEnroute->setText(defaultTime());
            this->ui->le_TakeOffTimePlanned->setText(QDateTime::currentDateTimeUtc().addSecs(30 * 60).toString("hh:mm"));
        }

        void CFlightPlanComponent::ps_loadFromDisk()
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
                if (json.isEmpty())
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityWarning, "Reading " + fileName + " yields no data", true);
                    break;
                }

                CFlightPlan fp;
                fp.convertFromJson(json);
                this->fillWithFlightPlanData(fp);
            }
            while (false);
            if (m.isFailure())
            {
                CLogMessage::preformatted(m);
            }
        }

        void CFlightPlanComponent::ps_saveToDisk()
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
                this->validateAndInitializeFlightPlan(fp);
                const QString json(fp.toJsonString());
                bool ok = CFileUtils::writeStringToFile(json, fileName);
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
            if (m.isFailure())
            {
                CLogMessage::preformatted(m);
            }
        }

        void CFlightPlanComponent::ps_setSelcalInOwnAircraft()
        {
            if (!sGui->getIContextOwnAircraft()) return;
            if (!this->ui->frp_SelcalCode->hasValidCode()) return;
            sGui->getIContextOwnAircraft()->updateSelcal(this->ui->frp_SelcalCode->getSelcal(), flightPlanIdentifier());
        }

        void CFlightPlanComponent::ps_loadFlightPlanFromNetwork()
        {
            if (!sGui->getIContextNetwork())
            {
                CLogMessage(this).info("Cannot load flight plan, network not available");
                return;
            }
            if (!sGui->getIContextNetwork()->isConnected())
            {
                CLogMessage(this).warning("Cannot load flight plan, network not connected");
                return;
            }

            CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
            CFlightPlan loadedPlan = sGui->getIContextNetwork()->loadFlightPlanFromNetwork(ownAircraft.getCallsign());
            if (loadedPlan.wasSentOrLoaded())
            {
                this->fillWithFlightPlanData(loadedPlan);
                CLogMessage(this).info("Updated with loaded flight plan");
            }
            else
            {
                CLogMessage(this).warning("No flight plan data");
            }
        }

        void CFlightPlanComponent::ps_buildRemarksString()
        {
            QString rem;
            QString v = this->ui->cb_VoiceCapabilities->currentText().toUpper();
            if (v.contains("TEXT"))         { rem.append("/T/ "); }
            else if (v.contains("RECEIVE")) { rem.append("/R/ "); }
            else if (v.contains("VOICE"))   { rem.append("/V/ "); }

            v = this->ui->le_AirlineOperator->text().trimmed();
            if (!v.isEmpty()) rem.append("OPR/").append(v).append(" ");

            v = this->ui->le_AircraftRegistration->text().trimmed();
            if (!v.isEmpty()) rem.append("REG/").append(v).append(" ");

            v = this->ui->cb_PilotRating->currentText().toUpper();
            if (v.contains("P1"))      { rem.append("PR/P1 "); }
            else if (v.contains("P2")) { rem.append("PR/P2 "); }
            else if (v.contains("P3")) { rem.append("PR/P3 "); }
            else if (v.contains("P4")) { rem.append("PR/P4 "); }
            else if (v.contains("P5")) { rem.append("PR/P5 "); }

            v = this->ui->cb_RequiredNavigationPerformance->currentText().toUpper();
            if (v.contains("10"))     { rem.append("RNP10 "); }
            else if (v.contains("4")) { rem.append("RNP4 "); }

            v = this->ui->cb_NavigationEquipment->currentText().toUpper();
            if (v.contains("VORS"))         { rem.append("NAV/VORNDB "); }
            else if (v.contains("SIDS"))    { rem.append("NAV/GPSRNAV "); }
            if (v.contains("DEFAULT"))      { rem.append("NAV/GPS "); }
            else if (v.contains("OCEANIC")) { rem.append("NAV/GPSOCEANIC "); }

            v = this->ui->cb_PerformanceCategory->currentText().toUpper();
            if (v.startsWith("A"))      { rem.append("PER/A "); }
            else if (v.startsWith("B")) { rem.append("PER/B "); }
            else if (v.startsWith("C")) { rem.append("PER/C "); }
            else if (v.startsWith("D")) { rem.append("PER/D "); }
            else if (v.startsWith("E")) { rem.append("PER/E "); }

            if (this->ui->frp_SelcalCode->hasValidCode())
            {
                rem.append("SEL/").append(this->ui->frp_SelcalCode->getSelcalCode());
                rem.append(" ");
            }

            if (this->ui->cb_NoSidsStarts->isChecked()) { rem.append("NO SID/STAR "); }

            v = this->ui->pte_AdditionalRemarks->toPlainText().trimmed();
            if (!v.isEmpty()) { rem.append(v); }

            rem = rem.simplified().trimmed();
            this->ui->pte_RemarksGenerated->setPlainText(rem);
        }

        void CFlightPlanComponent::ps_copyRemarks()
        {
            this->ui->pte_Remarks->setPlainText(this->ui->pte_RemarksGenerated->toPlainText());
            CLogMessage(this).info("Copied remarks");
        }

        void CFlightPlanComponent::ps_currentTabGenerator()
        {
            this->setCurrentWidget(this->ui->tb_RemarksGenerator);
        }

        void CFlightPlanComponent::ps_swiftDataRead()
        {
            this->initCompleters();
        }

        CIdentifier CFlightPlanComponent::flightPlanIdentifier()
        {
            if (m_identifier.getName().isEmpty()) { m_identifier = CIdentifier(QStringLiteral("FLIGHTPLANCOMPONENT")); }
            return m_identifier;
        }

        void CFlightPlanComponent::initCompleters()
        {
            if (!sGui || !sGui->getWebDataServices()) { return; }
            QStringList aircraft = sGui->getWebDataServices()->getAircraftIcaoCodes().allIcaoCodes();
            ui->le_AircraftType->setCompleter(new QCompleter(aircraft, this));
        }

        QString CFlightPlanComponent::getDefaultFilename(bool load)
        {
            // some logic to find a useful default name
            QString dir = CBuildConfig::getDocumentationDirectory();

            if (load)
            {
                return CFileUtils::appendFilePaths(dir, CFileUtils::jsonWildcardAppendix());
            }

            // Save file path
            QString name("Flight plan");
            if (!ui->le_DestinationAirport->text().isEmpty() && !ui->le_OriginAirport->text().isEmpty())
            {
                name.append(" ").append(ui->le_OriginAirport->text()).append("-").append(ui->le_DestinationAirport->text());
            }

            if (!name.endsWith(CFileUtils::jsonAppendix(), Qt::CaseInsensitive))
            {
                name += CFileUtils::jsonAppendix();
            }
            return CFileUtils::appendFilePaths(dir, name);
        }
    } // namespace
} // namespace
