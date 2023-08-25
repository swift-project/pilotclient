// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "flightplancomponent.h"
#include "blackgui/components/altitudedialog.h"
#include "blackgui/components/stringlistdialog.h"
#include "blackgui/components/simbriefdownloaddialog.h"
#include "blackgui/components/selcalcodeselector.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/eventfilter.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/user.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

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
#include <QPointer>
#include <QFile>
#include <QMessageBox>
#include <Qt>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackGui;
using namespace BlackCore;
using namespace BlackConfig;

namespace BlackGui::Components
{
    CFlightPlanComponent::CFlightPlanComponent(QWidget *parent) : COverlayMessagesTabWidget(parent),
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

        // rules
        ui->cb_FlightRule->clear();
        ui->cb_FlightRule->addItems(CFlightPlan::flightRules());

        // validators
        ui->le_Callsign->setValidator(new CUpperCaseValidator(ui->le_Callsign));
        ui->le_AircraftType->setValidator(new CUpperCaseValidator(ui->le_AircraftType));
        ui->le_DestinationAirport->setValidator(new CUpperCaseValidator(ui->le_DestinationAirport));
        ui->le_AlternateAirport->setValidator(new CUpperCaseValidator(ui->le_AlternateAirport));
        ui->le_OriginAirport->setValidator(new CUpperCaseValidator(ui->le_OriginAirport));
        ui->le_AirlineOperator->setValidator(new CUpperCaseValidator(ui->le_AirlineOperator));
        ui->le_AircraftRegistration->setValidator(new CUpperCaseValidator(ui->le_AircraftRegistration));

        CUpperCaseValidator *ucv = new CUpperCaseValidator(true, 0, 1, ui->le_EquipmentSuffix);
        ucv->setRestrictions(CFlightPlan::equipmentCodes());
        ui->le_EquipmentSuffix->setValidator(ucv);
        ui->le_EquipmentSuffix->setToolTip(ui->tbr_EquipmentCodes->toHtml());
        QCompleter *completer = new QCompleter(CFlightPlan::equipmentCodesInfo(), ui->le_EquipmentSuffix);
        completer->setMaxVisibleItems(10);
        completer->popup()->setMinimumWidth(225);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        ui->le_EquipmentSuffix->setCompleter(completer);

        CUpperCaseEventFilter *ef = new CUpperCaseEventFilter(ui->pte_Route);
        ef->setOnlyAscii();
        ui->pte_Route->installEventFilter(ef);
        ef = new CUpperCaseEventFilter(ui->pte_Remarks);
        ef->setOnlyAscii();
        ui->pte_Remarks->installEventFilter(ef);
        ef = new CUpperCaseEventFilter(ui->pte_AdditionalRemarks);
        ef->setOnlyAscii();
        ui->pte_AdditionalRemarks->installEventFilter(ef);
        // readonly
        // ef = new CUpperCaseEventFilter(ui->pte_RemarksGenerated);
        // ui->pte_RemarksGenerated->installEventFilter(ef);

        // connect
        connect(ui->pb_Send, &QPushButton::pressed, this, &CFlightPlanComponent::sendFlightPlan, Qt::QueuedConnection);
        connect(ui->pb_Download, &QPushButton::pressed, this, &CFlightPlanComponent::loadFlightPlanFromNetwork, Qt::QueuedConnection);
        connect(ui->pb_Reset, &QPushButton::pressed, this, &CFlightPlanComponent::resetFlightPlan, Qt::QueuedConnection);
        connect(ui->pb_ValidateFlightPlan, &QPushButton::pressed, this, &CFlightPlanComponent::validateFlightPlan, Qt::QueuedConnection);
        connect(ui->tb_SyncWithSimulator, &QPushButton::released, this, &CFlightPlanComponent::syncWithSimulator, Qt::QueuedConnection);
        connect(ui->pb_Prefill, &QPushButton::pressed, this, &CFlightPlanComponent::anticipateValues, Qt::QueuedConnection);
        connect(ui->pb_SimBrief, &QPushButton::pressed, this, &CFlightPlanComponent::loadFromSimBrief, Qt::QueuedConnection);

        connect(ui->pb_SaveTemplate, &QPushButton::released, this, &CFlightPlanComponent::saveTemplateToDisk, Qt::QueuedConnection);
        connect(ui->pb_LoadTemplate, &QPushButton::released, this, &CFlightPlanComponent::loadTemplateFromDisk, Qt::QueuedConnection);
        connect(ui->pb_ClearTemplate, &QPushButton::released, this, &CFlightPlanComponent::clearTemplate, Qt::QueuedConnection);

        connect(ui->cb_VoiceCapabilities, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks, Qt::QueuedConnection);
        connect(ui->cb_VoiceCapabilities, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::syncVoiceComboBoxes, Qt::QueuedConnection);
        connect(ui->cb_VoiceCapabilitiesFirstPage, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::syncVoiceComboBoxes, Qt::QueuedConnection);
        connect(ui->cb_NavigationEquipment, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks, Qt::QueuedConnection);
        connect(ui->cb_PerformanceCategory, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks, Qt::QueuedConnection);
        connect(ui->cb_PilotRating, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks, Qt::QueuedConnection);
        connect(ui->cb_RequiredNavigationPerformance, &QComboBox::currentTextChanged, this, &CFlightPlanComponent::currentTextChangedToBuildRemarks, Qt::QueuedConnection);

        connect(ui->pb_LoadDisk, &QPushButton::clicked, this, &CFlightPlanComponent::loadFromDisk, Qt::QueuedConnection);
        connect(ui->pb_SaveDisk, &QPushButton::clicked, this, &CFlightPlanComponent::saveToDisk, Qt::QueuedConnection);

        connect(ui->le_AircraftRegistration, &QLineEdit::textChanged, this, &CFlightPlanComponent::buildRemarksString, Qt::QueuedConnection);
        connect(ui->le_AirlineOperator, &QLineEdit::textChanged, this, &CFlightPlanComponent::buildRemarksString, Qt::QueuedConnection);
        connect(ui->cb_NoSidsStarts, &QCheckBox::released, this, &CFlightPlanComponent::buildRemarksString, Qt::QueuedConnection);

        connect(ui->pte_AdditionalRemarks, &QPlainTextEdit::textChanged, this, &CFlightPlanComponent::buildRemarksString, Qt::QueuedConnection);
        connect(ui->frp_SelcalCode, &CSelcalCodeSelector::valueChanged, this, &CFlightPlanComponent::buildRemarksString, Qt::QueuedConnection);
        connect(ui->frp_SelcalCode, &CSelcalCodeSelector::valueChanged, this, &CFlightPlanComponent::setSelcalInOwnAircraft, Qt::QueuedConnection);
        connect(ui->pb_CopyOver, &QPushButton::pressed, this, &CFlightPlanComponent::copyRemarksConfirmed, Qt::QueuedConnection);
        connect(ui->pb_GetFromGenerator, &QPushButton::pressed, this, &CFlightPlanComponent::copyRemarksConfirmed, Qt::QueuedConnection);
        connect(ui->pb_RemarksGenerator, &QPushButton::clicked, this, &CFlightPlanComponent::currentTabGenerator, Qt::QueuedConnection);

        connect(ui->tb_HelpEquipment, &QToolButton::clicked, this, &CFlightPlanComponent::showEquipmentCodesTab, Qt::QueuedConnection);
        connect(ui->tb_AltitudeDialog, &QToolButton::clicked, this, &CFlightPlanComponent::altitudeDialog, Qt::QueuedConnection);

        connect(ui->le_AircraftType, &QLineEdit::editingFinished, this, &CFlightPlanComponent::aircraftTypeChanged, Qt::QueuedConnection);
        connect(ui->le_EquipmentSuffix, &QLineEdit::editingFinished, this, &CFlightPlanComponent::buildPrefixIcaoSuffix, Qt::QueuedConnection);
        connect(ui->cb_Heavy, &QCheckBox::released, this, &CFlightPlanComponent::prefixCheckBoxChanged, Qt::QueuedConnection);
        connect(ui->cb_Tcas, &QCheckBox::released, this, &CFlightPlanComponent::prefixCheckBoxChanged, Qt::QueuedConnection);

        connect(ui->pb_Remarks, &QPushButton::pressed, this, &CFlightPlanComponent::remarksHistory, Qt::QueuedConnection);
        connect(ui->pb_AddRemarks, &QPushButton::pressed, this, &CFlightPlanComponent::remarksHistory, Qt::QueuedConnection);

        // web services
        connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this, &CFlightPlanComponent::swiftWebDataRead, Qt::QueuedConnection);

        // init GUI
        this->resetFlightPlan();
        this->buildRemarksString();

        // prefill some data derived from what was used last
        const QPointer<CFlightPlanComponent> myself(this);
        QTimer::singleShot(2500, this, [=] {
            if (!sGui || sGui->isShuttingDown() || !myself) { return; }

            this->loadTemplateFromDisk();
            if (sGui->getIContextSimulator()->isSimulatorAvailable())
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
        });
    }

    CFlightPlanComponent::~CFlightPlanComponent()
    {}

    void CFlightPlanComponent::loginDataSet()
    {
        if (m_sentFlightPlan.wasSentOrLoaded()) { return; } // when loaded or sent do not override
        this->prefillWithOwnAircraftData();
    }

    void CFlightPlanComponent::prefillWithOwnAircraftData()
    {
        if (!sGui->getIContextOwnAircraft()) { return; }
        if (!sGui->getIContextSimulator()) { return; }
        if (!sGui->getIContextSimulator()->isSimulatorAvailable()) { return; }
        const CSimulatedAircraft ownAircraft(sGui->getIContextOwnAircraft()->getOwnAircraft());
        this->prefillWithAircraftData(ownAircraft);
    }

    void CFlightPlanComponent::prefillWithAircraftData(const CSimulatedAircraft &aircraft, bool force)
    {
        if (!force && m_sentFlightPlan.wasSentOrLoaded()) { return; }

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

        if (aircraft.getAircraftIcaoCode().isLoadedFromDb() && aircraft.getAircraftIcaoCode().hasValidWtc())
        {
            const QString wtc = aircraft.getAircraftIcaoCode().getWtc().toUpper();
            const bool heavyFlag = (wtc.startsWith("H", Qt::CaseInsensitive) || wtc.startsWith("S", Qt::CaseInsensitive));
            ui->cb_Heavy->setChecked(heavyFlag);
        }

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

    void CFlightPlanComponent::fillWithFlightPlanData(const CFlightPlan &flightPlan)
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

        const QString r = flightPlan.getFlightRulesAsString();
        if (CFlightPlan::flightRules().contains(r, Qt::CaseInsensitive))
        {
            ui->cb_FlightRule->setCurrentText(r);
        }
        else if (flightPlan.getFlightRules() == CFlightPlan::UNKNOWN)
        {
            ui->cb_FlightRule->setCurrentText(CFlightPlan::flightRulesToString(CFlightPlan::IFR));
            const CStatusMessage m = CStatusMessage(this).validationWarning(u"Unknown fligh rule, setting to default");
            this->showOverlayMessage(m);
        }

        if (!flightPlan.getRemarks().isEmpty())
        {
            const QString rem = flightPlan.getRemarks();
            this->setRemarksUIValues(rem);
        }
    }

    const QStringList &CFlightPlanComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::flightPlan(), CLogCategories::guiComponent() };
        return cats;
    }

    CStatusMessageList CFlightPlanComponent::validateAndInitializeFlightPlan(CFlightPlan &flightPlan)
    {
        CStatusMessageList messages;
        const bool strict = ui->cb_StrictCheck->isChecked();
        const bool vfr = this->isVfr();
        const CStatusMessage::StatusSeverity severity = strict ? CStatusMessage::SeverityError : CStatusMessage::SeverityWarning;
        messages.push_back(CStatusMessage(this).validationInfo(strict ? QStringLiteral("Strict validation") : QStringLiteral("Lenient validation")));

        const CFlightPlan::FlightRules rules = this->getFlightRules();
        flightPlan.setFlightRule(rules);

        // callsign
        QString v;
        v = ui->le_Callsign->text().trimmed().toUpper();
        if (v.isEmpty())
        {
            // messages.push_back(CStatusMessage(this).validationError(u"Missing '%1'") << ui->lbl_Callsign->text());
        }
        else if (!CCallsign::isValidAircraftCallsign(v))
        {
            messages.push_back(CStatusMessage(this).validationError(u"Invalid callsign '%1'") << v);
        }
        flightPlan.setCallsign(CCallsign(v));

        // aircraft ICAO / aircraft type
        v = ui->le_AircraftType->text().trimmed().toUpper();
        if (v.isEmpty())
        {
            messages.push_back(CStatusMessage(this).validationError(u"Missing '%1'") << ui->lbl_AircraftType->text());
        }
        else if (!CAircraftIcaoCode::isValidDesignator(v))
        {
            messages.push_back(CStatusMessage(this).validationError(u"Invalid aircraft ICAO code '%1'") << v);
        }
        else if (sApp && sApp->hasWebDataServices() && sApp->getWebDataServices()->hasDbAircraftData() && !sApp->getWebDataServices()->containsAircraftIcaoDesignator(v))
        {
            messages.push_back(CStatusMessage(this).validationWarning(u"Are you sure '%1' is a valid type?") << v);
        }
        flightPlan.setAircraftIcao(this->getAircraftIcaoCode());

        // prefix / equipment code
        v = this->getPrefix();
        if (!v.isEmpty() && !CFlightPlan::prefixCodes().contains(v))
        {
            messages.push_back(CStatusMessage(this).validation(severity, u"Invalid prefix"));
        }
        flightPlan.setPrefix(v);

        v = this->getEquipmentSuffix();
        if (!v.isEmpty() && !CFlightPlan::equipmentCodes().contains(v))
        {
            messages.push_back(CStatusMessage(this).validation(severity, u"Invalid equipment code"));
        }
        flightPlan.setEquipmentSuffix(v);

        // route
        v = ui->pte_Route->toPlainText().trimmed();
        const int routeLength = v.length();
        if (v.isEmpty())
        {
            messages.push_back(CStatusMessage(this).validation(
                                   vfr ?
                                       CStatusMessage::SeverityInfo :
                                       CStatusMessage::SeverityWarning,
                                   u"Missing '%1'")
                               << ui->lbl_Route->text());
        }
        else if (routeLength > CFlightPlan::MaxRouteLength)
        {
            messages.push_back(CStatusMessage(this).validationError(u"Flight plan route length exceeded (%1 chars max.)") << CFlightPlan::MaxRouteLength);
        }
        else
        {
            flightPlan.setRoute(v);
        }

        // remarks
        v = ui->pte_Remarks->toPlainText().trimmed();
        const int remarksLength = v.length();
        if (v.isEmpty())
        {
            messages.push_back(CStatusMessage(this).validationError(u"No '%1', voice capabilities are mandatory") << ui->pb_Remarks->text());
        }
        else if (remarksLength > CFlightPlan::MaxRemarksLength)
        {
            messages.push_back(CStatusMessage(this).validationError(u"Flight plan remarks length exceeded (%1 chars max.)") << CFlightPlan::MaxRemarksLength);
        }
        else
        {
            flightPlan.setRemarks(v);
        }

        // Total length
        if ((remarksLength + routeLength) > CFlightPlan::MaxRouteAndRemarksLength)
        {
            messages.push_back(CStatusMessage(this).validationError(u"Flight plan route (%1) and remarks (%2) length exceeded (%3 chars max.)") << routeLength << remarksLength << CFlightPlan::MaxRemarksLength);
        }

        // time enroute
        v = ui->le_EstimatedTimeEnroute->text();
        if (v.isEmpty() || v == defaultTime())
        {
            messages.push_back(CStatusMessage(this).validation(severity, u"Missing '%1'") << ui->lbl_EstimatedTimeEnroute->text());
        }
        flightPlan.setEnrouteTime(v);

        // fuel
        v = ui->le_FuelOnBoard->text();
        if (v.isEmpty() || v == defaultTime())
        {
            messages.push_back(CStatusMessage(this).validation(severity, u"Missing '%1'") << ui->lbl_FuelOnBoard->text());
        }
        flightPlan.setFuelTime(v);

        // take off time
        v = ui->le_TakeOffTimePlanned->text();
        if (v.isEmpty() || v == defaultTime())
        {
            messages.push_back(CStatusMessage(this).validation(severity, u"Missing '%1'") << ui->lbl_TakeOffTimePlanned->text());
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
            messages.push_back(CStatusMessage(this).validationError(u"Missing '%1'") << ui->lbl_DestinationAirport->text());
            flightPlan.setDestinationAirportIcao(QString());
        }
        else
        {
            flightPlan.setDestinationAirportIcao(v);
            if (strict && !flightPlan.getDestinationAirportIcao().hasValidIcaoCode(true))
            {
                messages.push_back(CStatusMessage(this).validationError(u"Invalid destination ICAO code '%1'") << v);
            }
            else if (!flightPlan.getDestinationAirportIcao().hasValidIcaoCode(false))
            {
                messages.push_back(CStatusMessage(this).validationWarning(u"Wrong or missing '%1'") << ui->lbl_DestinationAirport->text());
            }
        }

        // origin airport
        v = ui->le_OriginAirport->text();
        if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
        {
            messages.push_back(CStatusMessage(this).validationError(u"Missing '%1'") << ui->lbl_OriginAirport->text());
            flightPlan.setOriginAirportIcao(defaultIcao());
        }
        else
        {
            flightPlan.setOriginAirportIcao(v);
            if (strict && !flightPlan.getOriginAirportIcao().hasValidIcaoCode(true))
            {
                messages.push_back(CStatusMessage(this).validationError(u"Invalid origin ICAO code '%1'") << v);
            }
            else if (!flightPlan.getOriginAirportIcao().hasValidIcaoCode(false))
            {
                messages.push_back(CStatusMessage(this).validationWarning(u"Wrong or missing '%1'") << ui->lbl_DestinationAirport->text());
            }
        }

        // TAS
        v = ui->le_CruiseTrueAirspeed->text();
        CSpeed cruiseTAS;
        cruiseTAS.parseFromString(v, CPqString::SeparatorBestGuess);
        if (cruiseTAS.isNull())
        {
            messages.push_back(CStatusMessage(this).validationError(u"Wrong TAS, %1. Try adding a unit like '100kts' or '150km/h'") << ui->lbl_CruiseTrueAirspeed->text());
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
                messages.push_back(CStatusMessage(this).validationInfo(u"Missing %1") << ui->lbl_AlternateAirport->text());
            }
            flightPlan.setAlternateAirportIcao(QString());
        }
        else
        {
            flightPlan.setAlternateAirportIcao(v);
        }

        // OK
        if (!messages.isFailure())
        {
            messages.push_back(CStatusMessage(this).validationInfo(u"Flight plan validation passed"));
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
                m = CStatusMessage(this).validationInfo(u"Sent flight plan");
            }
            else
            {
                flightPlan.setWhenLastSentOrLoaded(QDateTime()); // empty
                m = CStatusMessage(this).validationError(u"No errors, but not connected, cannot send flight plan");
            }
            ui->le_LastSent->setText(lastSent);
            if (m.isSeverityInfoOrLess())
            {
                this->showOverlayHTMLMessage(m, OverlayTimeoutMs);
                this->updateRemarksHistories(); // all OK, we keep that in history
            }
            else
            {
                this->showOverlayMessage(m, OverlayTimeoutMs);
            }
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
        CStatusMessageList msgs;
        const QString fileName = QFileDialog::getOpenFileName(this, tr("Load flight plan"), this->getDefaultFilename(true), "Flight plans (*.json *.sfp *.vfp *.xml);;swift (*.json *.txt);;SimBrief (*.xml);;vPilot (*.vfp);;SB4 (*.sfp)");
        if (fileName.isEmpty()) { return; }
        CFlightPlan fp = CFlightPlan::loadFromMultipleFormats(fileName, &msgs);
        if (!fp.hasCallsign()) { fp.setCallsign(ui->le_Callsign->text()); } // set callsign if it wasn't set

        if (msgs.isSuccess())
        {
            this->fillWithFlightPlanData(fp);
            this->updateDirectorySettings(fileName);
        }
        else
        {
            this->showOverlayMessages(msgs, OverlayTimeoutMs);
        }
    }

    void CFlightPlanComponent::loadTemplateFromDisk()
    {
        const QFile f(this->getTemplateName());
        if (!f.exists()) { return; }

        CStatusMessageList msgs;
        CFlightPlan fp = CFlightPlan::loadFromMultipleFormats(f.fileName(), &msgs);
        if (!fp.hasCallsign()) { fp.setCallsign(ui->le_Callsign->text()); } // set callsign if it wasn't set
        if (msgs.isSuccess())
        {
            this->fillWithFlightPlanData(fp);
        }
    }

    void CFlightPlanComponent::saveToDisk()
    {
        CStatusMessage m;
        const QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save flight plan"), this->getDefaultFilename(false), tr("swift (*.json;*.txt)"));
        do
        {
            if (fileName.isEmpty())
            {
                m = CStatusMessage(this, CStatusMessage::SeverityDebug, u"Save canceled", true);
                break;
            }

            QFileInfo fi(fileName);
            QDir fpDir = fi.absoluteDir();
            if (CDirectoryUtils::isInApplicationDirectory(fpDir.absolutePath()))
            {
                const int ret = QMessageBox::warning(this, "swift flight plan",
                                                     "You try to save inside the swift directory '" + fpDir.absolutePath() +
                                                         "'\n\nThis is not recommended!"
                                                         "\n\nDo you want to really do this?",
                                                     QMessageBox::Save | QMessageBox::Cancel);
                if (ret != QMessageBox::Save) { return; }
            }

            const bool ok = this->saveFPToDisk(fileName);
            if (ok)
            {
                m = CStatusMessage(this, CStatusMessage::SeverityInfo, u"Written " % fileName, true);
                this->updateDirectorySettings(fileName);
            }
            else
            {
                m = CStatusMessage(this, CStatusMessage::SeverityError, u"Writing " % fileName % u" failed", true);
            }
        }
        while (false);
        if (m.isFailure()) { CLogMessage::preformatted(m); }
    }

    bool CFlightPlanComponent::saveFPToDisk(const QString &fileName)
    {
        CFlightPlan fp;
        const CStatusMessageList msgs = this->validateAndInitializeFlightPlan(fp); // get data
        // if (msgs.hasErrorMessages()) { return false; }
        Q_UNUSED(msgs)

        // save as CVariant format
        const CVariant variantFp = CVariant::fromValue(fp);
        const QString json(variantFp.toJsonString());
        const bool ok = CFileUtils::writeStringToFile(json, fileName);

        return ok;
    }

    void CFlightPlanComponent::saveTemplateToDisk()
    {
        const QString fn = this->getTemplateName();
        const bool ok = this->saveFPToDisk(fn);
        if (ok)
        {
            CLogMessage(this).info(u"Saved FP template '%1'") << fn;
        }
        else
        {
            CLogMessage(this).warning(u"Saving FP template '%1' failed") << fn;
        }
    }

    void CFlightPlanComponent::clearTemplate()
    {
        QFile f(this->getTemplateName());
        if (!f.exists()) { return; }
        const bool r = f.remove();
        if (r) { CLogMessage(this).info(u"Deleted FP template '%1'") << f.fileName(); }
    }

    QString CFlightPlanComponent::getTemplateName() const
    {
        const QString fn =
            CFileUtils::appendFilePathsAndFixUnc(
                CSwiftDirectories::normalizedApplicationDataDirectory(),
                QStringLiteral("swiftFlightPlanTemplate.json"));
        return fn;
    }

    void CFlightPlanComponent::setSelcalInOwnAircraft()
    {
        if (!sGui || !sGui->getIContextOwnAircraft()) { return; }
        if (!ui->frp_SelcalCode->hasValidCode()) { return; }
        sGui->getIContextOwnAircraft()->updateSelcal(ui->frp_SelcalCode->getSelcal(), flightPlanIdentifier());
    }

    void CFlightPlanComponent::loadFlightPlanFromNetwork()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork() || !sGui->getIContextNetwork()->isConnected())
        {
            const CStatusMessage m = CLogMessage(this).validationWarning(u"Cannot load network flight plan, network not connected");
            this->showOverlayHTMLMessage(m, OverlayTimeoutMs);
            return;
        }

        const CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
        const CFlightPlan loadedPlan = sGui->getIContextNetwork()->loadFlightPlanFromNetwork(ownAircraft.getCallsign());
        if (loadedPlan.wasSentOrLoaded())
        {
            const QMessageBox::StandardButton r = QMessageBox::warning(this, "Loaded FP", "Override current flight plan data?", QMessageBox::Yes | QMessageBox::No);
            if (r != QMessageBox::Yes) { return; }
            this->fillWithFlightPlanData(loadedPlan);
            CLogMessage(this).info(u"Updated with loaded flight plan");
        }
        else
        {
            const CStatusMessage m = CLogMessage(this).warning(u"No flight plan data in loaded plan");
            this->showOverlayHTMLMessage(m, OverlayTimeoutMs);
        }
    }

    void CFlightPlanComponent::buildRemarksString()
    {
        QString v = ui->cb_VoiceCapabilities->currentText().toUpper();
        QString rem = CFlightPlanRemarks::textToVoiceCapabilitiesRemarks(v).append(" ");

        v = ui->le_AirlineOperator->text().trimmed();
        if (!v.isEmpty()) rem.append("OPR/").append(v).append(" ");

        v = ui->le_AircraftRegistration->text().trimmed();
        if (!v.isEmpty()) rem.append("REG/").append(v).append(" ");

        v = ui->cb_PilotRating->currentText().toUpper();
        if (v.contains("P1")) { rem.append("PR/P1 "); }
        else if (v.contains("P2")) { rem.append("PR/P2 "); }
        else if (v.contains("P3")) { rem.append("PR/P3 "); }
        else if (v.contains("P4")) { rem.append("PR/P4 "); }
        else if (v.contains("P5")) { rem.append("PR/P5 "); }

        v = ui->cb_RequiredNavigationPerformance->currentText().toUpper();
        if (v.contains("10")) { rem.append("RNP10 "); }
        else if (v.contains("4")) { rem.append("RNP4 "); }

        v = ui->cb_NavigationEquipment->currentText().toUpper();
        if (v.contains("VORS")) { rem.append("NAV/VORNDB "); }
        else if (v.contains("SIDS")) { rem.append("NAV/GPSRNAV "); }
        if (v.contains("DEFAULT")) { rem.append("NAV/GPS "); }
        else if (v.contains("OCEANIC")) { rem.append("NAV/GPSOCEANIC "); }

        v = ui->cb_PerformanceCategory->currentText().toUpper();
        if (v.startsWith("A")) { rem.append("PER/A "); }
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
        CLogMessage(this).info(u"Copied remarks");
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
                const QPointer<CFlightPlanComponent> myself(this);
                QTimer::singleShot(10, this, [=] {
                    if (!myself) { return; }
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
                const QPointer<CFlightPlanComponent> myself(this);
                QTimer::singleShot(10, this, [=] {
                    if (!myself) { return; }
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
        QPointer<CFlightPlanComponent> myself(this);
        QTimer::singleShot(25, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            const bool heavy = icao.getWtc().startsWith("H");
            ui->cb_Heavy->setChecked(heavy);
            if (heavy) { ui->cb_Tcas->setChecked(false); }
            this->buildPrefixIcaoSuffix();
        });
    }

    void CFlightPlanComponent::syncWithSimulator()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        const QMessageBox::StandardButton reply = QMessageBox::question(this, QStringLiteral("Override aircraft data"), QStringLiteral("Override aircraft ICAO data from simulator"), QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) { return; }

        const CSimulatedAircraft aircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
        this->prefillWithAircraftData(aircraft, true);
    }

    QString CFlightPlanComponent::getPrefix() const
    {
        if (ui->cb_Heavy->isChecked()) { return QStringLiteral("H"); }
        else if (ui->cb_Tcas->isChecked()) { return QStringLiteral("T"); }
        return {};
    }

    CAircraftIcaoCode CFlightPlanComponent::getAircraftIcaoCode() const
    {
        const QString designator(ui->le_AircraftType->text());
        if (!CAircraftIcaoCode::isValidDesignator(designator)) { return CAircraftIcaoCode(); }
        if (sApp && sApp->hasWebDataServices())
        {
            const CAircraftIcaoCode designatorFromDb = sApp->getWebDataServices()->getAircraftIcaoCodeForDesignator(designator);
            if (designatorFromDb.isLoadedFromDb()) { return designatorFromDb; }
        }
        return designator;
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
        const bool vfr = CFlightPlan::isVFRRules(ui->cb_FlightRule->currentText());
        return vfr;
    }

    CFlightPlan::FlightRules CFlightPlanComponent::getFlightRules() const
    {
        const CFlightPlan::FlightRules r = CFlightPlan::stringToFlightRules(ui->cb_FlightRule->currentText());
        return r;
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
            msgs.push_back(CStatusMessage(this).validationInfo(u"No SID/STARs"));
            ui->cb_RequiredNavigationPerformance->setCurrentIndex(0);
            ui->cb_PerformanceCategory->setCurrentIndex(0);
            ui->cb_NavigationEquipment->setCurrentIndex(0);
            msgs.push_back(CStatusMessage(this).validationInfo(u"Set navigation and performance to VFR"));
        }
        else
        {
            // IFR
            const CAircraftIcaoCode icao = this->getAircraftIcaoCode();
            if (icao.isLoadedFromDb())
            {
                if (icao.getEnginesCount() >= 2 && icao.getEngineType() == "J")
                {
                    // jet with >=2 engines
                    msgs.push_back(CStatusMessage(this).validationInfo(u"Jet >=2 engines"));
                    msgs.push_back(CStatusMessage(this).validationInfo(u"SID/STARs"));
                    ui->cb_NoSidsStarts->setChecked(false);
                    ui->cb_NavigationEquipment->setCurrentText("GPS or FMC capable of SIDs/STARs");
                    msgs.push_back(CStatusMessage(this).validationInfo(u"GPS or FMC capable of SIDs/STARs"));

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
        this->showOverlayMessages(msgs, false, OverlayTimeoutMs);

        // copy over
        if (msgs.isSuccess())
        {
            this->buildRemarksString();
            this->copyRemarks(false);
        }
    }

    void CFlightPlanComponent::updateDirectorySettings(const QString &fileOrDirectory)
    {
        if (fileOrDirectory.isEmpty()) { return; }

        CDirectories swiftDirs = m_directories.get();
        swiftDirs.setFlightPlanDirectory(CDirectories::fileNameToDirectory(fileOrDirectory));
        CStatusMessage saveMsg = m_directories.setAndSave(swiftDirs);
        CLogMessage::preformatted(saveMsg);
    }

    void CFlightPlanComponent::altitudeDialog()
    {
        if (!m_altitudeDialog)
        {
            m_altitudeDialog = new CAltitudeDialog(this);
        }

        const QDialog::DialogCode ret = static_cast<QDialog::DialogCode>(m_altitudeDialog->exec());
        if (ret != QDialog::Accepted) { return; }

        if (!m_altitudeDialog->getAltitudeString().isEmpty())
        {
            ui->lep_CrusingAltitude->setText(m_altitudeDialog->getAltitudeString());
        }
    }

    void CFlightPlanComponent::updateRemarksHistories()
    {
        QString r = ui->pte_Remarks->toPlainText();
        if (!r.isEmpty())
        {
            QStringList h = m_remarksHistory.get();
            if (consolidateRemarks(h, r))
            {
                CStatusMessage m = m_remarksHistory.setAndSave(h);
                CLogMessage::preformatted(m);
            }
        }

        r = ui->pte_AdditionalRemarks->toPlainText();
        if (!r.isEmpty())
        {
            QStringList h = m_remarksHistoryAdditional.get();
            if (consolidateRemarks(h, r))
            {
                CStatusMessage m = m_remarksHistoryAdditional.setAndSave(h);
                CLogMessage::preformatted(m);
            }
        }
    }

    void CFlightPlanComponent::setRemarksUIValues(const QString &remarks)
    {
        if (remarks.isEmpty()) { return; }

        if (remarks.contains("/V"))
        {
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_VoiceCapabilitiesFirstPage, "FULL");
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_VoiceCapabilities, "FULL");
        }
        else if (remarks.contains("/T"))
        {
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_VoiceCapabilitiesFirstPage, "TEXT ONLY");
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_VoiceCapabilities, "TEXT ONLY");
        }
        else if (remarks.contains("/R"))
        {
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_VoiceCapabilitiesFirstPage, "RECEIVE");
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_VoiceCapabilities, "RECEIVE");
        }

        if (remarks.contains("NAV/GPSRNAV"))
        {
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_NavigationEquipment, "GPS OR FMC");
        }
        else if (remarks.contains("NAV/VORNDB"))
        {
            CGuiUtility::setComboBoxValueByContainingString(ui->cb_NavigationEquipment, "DIRECT VOR");
        }

        const int selcal = remarks.indexOf("SEL/");
        if (selcal >= 0 && remarks.length() > selcal + 7)
        {
            const QString code = remarks.mid(selcal + 4, 4);
            if (code.length() == 4)
            {
                ui->frp_SelcalCode->setSelcal(code);
            }
        }
    }

    void CFlightPlanComponent::loadFromSimBrief()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!m_simBriefDialog)
        {
            m_simBriefDialog = new CSimBriefDownloadDialog(this);
        }
        const int rv = m_simBriefDialog->exec();
        if (rv != QDialog::Accepted) { return; }

        const CUrl url = m_simBriefDialog->getSimBriefData().getUrlAndUsername();
        sApp->getFromNetwork(url.toNetworkRequest(), { this, &CFlightPlanComponent::handleSimBriefResponse });
    }

    void CFlightPlanComponent::handleSimBriefResponse(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (!sGui || sGui->isShuttingDown()) { return; }

        const QUrl url(nwReply->url());
        const QString urlString(url.toString());

        if (nwReply->error() == QNetworkReply::NoError)
        {
            // const qint64 lastModified = CNetworkUtils::lastModifiedMsSinceEpoch(nwReply.data());
            const QString simBriefFP(nwReplyPtr->readAll());
            nwReplyPtr->close();
            if (simBriefFP.isEmpty())
            {
                this->showOverlayHTMLMessage("No SimBrief data from " % urlString);
            }
            else
            {
                CFlightPlan fp = CFlightPlan::fromSimBriefFormat(simBriefFP);
                this->fillWithFlightPlanData(fp);
            }
        } // no error
        else
        {
            // network error, try next URL
            nwReply->abort();
        }
    }

    bool CFlightPlanComponent::consolidateRemarks(QStringList &remarks, const QString &newRemarks)
    {
        if (newRemarks.isEmpty()) { return false; }
        remarks.removeAll(newRemarks);
        remarks.push_front(newRemarks);
        return true;
    }

    void CFlightPlanComponent::remarksHistory()
    {
        const QObject *sender = QObject::sender();
        if (!m_fpRemarksDialog)
        {
            m_fpRemarksDialog = new CStringListDialog(this);
            m_fpRemarksDialog->setModal(true);
        }
        if (sender == ui->pb_Remarks) { m_fpRemarksDialog->setStrings(m_remarksHistory.getThreadLocal()); }
        else if (sender == ui->pb_AddRemarks) { m_fpRemarksDialog->setStrings(m_remarksHistoryAdditional.getThreadLocal()); }

        const int rv = m_fpRemarksDialog->exec();
        if (rv != QDialog::Accepted) { return; }
        const QString remarks = m_fpRemarksDialog->getSelectedValue();
        if (remarks.isEmpty()) { return; }

        if (sender == ui->pb_Remarks) { ui->pte_Remarks->setPlainText(remarks); }
        else if (sender == ui->pb_AddRemarks) { ui->pte_AdditionalRemarks->setPlainText(remarks); }
    }

    void CFlightPlanComponent::initCompleters()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        const QStringList aircraft(sGui->getWebDataServices()->getAircraftIcaoCodes().allDesignators().values());
        QCompleter *aircraftCompleter = new QCompleter(aircraft, this);
        aircraftCompleter->setMaxVisibleItems(10);
        const int w5chars1 = aircraftCompleter->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
        aircraftCompleter->popup()->setMinimumWidth(w5chars1 * 5);
        aircraftCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        aircraftCompleter->setCompletionMode(QCompleter::PopupCompletion);
        ui->le_AircraftType->setCompleter(aircraftCompleter);

        const QStringList airports = sGui->getWebDataServices()->getAirports().allIcaoCodes(true);
        QCompleter *airportCompleter = new QCompleter(airports, this);
        airportCompleter->setMaxVisibleItems(10);
        const int w5chars2 = airportCompleter->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
        airportCompleter->popup()->setMinimumWidth(w5chars2 * 5);
        airportCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        airportCompleter->setCompletionMode(QCompleter::PopupCompletion);
        ui->le_AlternateAirport->setCompleter(airportCompleter);
        ui->le_DestinationAirport->setCompleter(airportCompleter);
        ui->le_OriginAirport->setCompleter(airportCompleter);
    }

    QString CFlightPlanComponent::getDefaultFilename(bool load)
    {
        // some logic to find a useful default name
        const QString dir = m_directories.get().getFlightPlanDirectoryOrDefault();
        if (load) { return dir; }

        // Save file path
        QString name("Flight plan");
        if (!ui->le_DestinationAirport->text().isEmpty() && !ui->le_OriginAirport->text().isEmpty())
        {
            name += u' ' % ui->le_OriginAirport->text() %
                    u'-' % ui->le_DestinationAirport->text();
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
            const QString ct = ui->cb_VoiceCapabilitiesFirstPage->currentText();
            if (stringCompare(ct, text, Qt::CaseInsensitive)) { return; }
            ui->cb_VoiceCapabilitiesFirstPage->setCurrentText(text);
        }
        else
        {
            const QString ct = ui->cb_VoiceCapabilities->currentText();
            if (!stringCompare(ct, text, Qt::CaseInsensitive))
            {
                // avoid unnecessary roundtrips
                ui->cb_VoiceCapabilities->setCurrentText(text);
            }
            const QString r = CFlightPlanRemarks::replaceVoiceCapabilities(CFlightPlanRemarks::textToVoiceCapabilitiesRemarks(text), ui->pte_Remarks->toPlainText());
            if (ui->pte_Remarks->toPlainText() != r)
            {
                ui->pte_Remarks->setPlainText(r);
            }
        }
    }
} // namespace
