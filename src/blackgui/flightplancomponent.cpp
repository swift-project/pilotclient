#include "flightplancomponent.h"
#include "ui_flightplancomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    CFlightPlanComponent::CFlightPlanComponent(QWidget *parent) :
        QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CFlightPlanComponent)
    {
        ui->setupUi(this);
        connect(this->ui->pb_Send, &QPushButton::pressed, this, &CFlightPlanComponent::sendFlightPlan);
        connect(this->ui->pb_Load, &QPushButton::pressed, this, &CFlightPlanComponent::loadFlightPlan);
        connect(this->ui->pb_Reset, &QPushButton::pressed, this, &CFlightPlanComponent::resetFlightPlan);
        connect(this->ui->pb_ValidateFlightPlan, &QPushButton::pressed, this, &CFlightPlanComponent::validateFlightPlan);

        bool c;
        c = connect(this->ui->cb_VoiceCapabilities, SIGNAL(currentIndexChanged(int)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        c = connect(this->ui->cb_NavigationEquipment, SIGNAL(currentIndexChanged(int)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        c = connect(this->ui->cb_PerformanceCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        c = connect(this->ui->cb_PilotRating, SIGNAL(currentIndexChanged(int)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        c = connect(this->ui->cb_RequiredNavigationPerformance, SIGNAL(currentIndexChanged(int)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        c = connect(this->ui->cb_NoSidsStarts, SIGNAL(toggled(bool)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        c = connect(this->ui->le_AircraftRegistration, SIGNAL(textChanged(QString)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        c = connect(this->ui->le_AirlineOperator, SIGNAL(textChanged(QString)), this, SLOT(buildRemarkString()));
        Q_ASSERT(c);
        Q_UNUSED(c);

        connect(this->ui->pte_AdditionalRemarks, &QPlainTextEdit::textChanged, this, &CFlightPlanComponent::buildRemarkString);
        connect(this->ui->frp_SelcalCode, &CSelcalCodeSelector::valueChanged, this, &CFlightPlanComponent::buildRemarkString);
        connect(this->ui->pb_CopyOver, &QPushButton::pressed, this, &CFlightPlanComponent::copyRemarks);
        connect(this->ui->pb_RemarksGenerator, &QPushButton::clicked, this, &CFlightPlanComponent::currentTabGenerator);

        this->ui->frp_SelcalCode->resetSelcalCodes(true);
        this->resetFlightPlan();
        this->buildRemarkString();
    }

    CFlightPlanComponent::~CFlightPlanComponent()
    {
        delete ui;
    }

    void CFlightPlanComponent::prefillWithAircraftData(const BlackMisc::Aviation::CAircraft &aircraftData)
    {
        this->ui->le_Callsign->setText(aircraftData.getCallsign().asString());
        this->ui->le_AircraftType->setText(aircraftData.getIcaoInfo().getAircraftDesignator());
        this->ui->le_PilotsName->setText(aircraftData.getPilot().getRealName());
    }

    void CFlightPlanComponent::prefillWithFlightPlanData(const BlackMisc::Aviation::CFlightPlan &flightPlan)
    {
        this->ui->le_AlternateAirport->setText(flightPlan.getAlternateAirportIcao().asString());
        this->ui->le_DestinationAirport->setText(flightPlan.getAlternateAirportIcao().asString());
        this->ui->le_OriginAirport->setText(flightPlan.getAlternateAirportIcao().asString());
        this->ui->pte_Route->setPlainText(flightPlan.getRoute());
        this->ui->pte_Remarks->setPlainText(flightPlan.getRemarks());
        this->ui->le_TakeOffTimePlanned->setText(flightPlan.getTakeoffTimePlannedHourMin());
        this->ui->le_FuelOnBoard->setText(flightPlan.getFuelTimeHourMin());
        this->ui->le_EstimatedTimeEnroute->setText(flightPlan.getEnrouteTimeHourMin());
        this->ui->le_CrusingAltitude->setText(flightPlan.getCruiseAltitude().toQString());
        this->ui->le_CruiseTrueAirspeed->setText(flightPlan.getCruiseTrueAirspeed().valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CSpeedUnit::kts(), 0));
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
            rule = CFlightPlan::IFR;
        else if (this->ui->rb_TypeVfr->isChecked())
            rule = CFlightPlan::VFR;
        flightPlan.setFlightRule(rule);

        v = ui->le_Callsign->text().trimmed();
        if (v.isEmpty())
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_Callsign->text());
            messages.push_back(CStatusMessage::getValidationError(m));
        }

        v = ui->pte_Route->toPlainText().trimmed();
        if (v.isEmpty())
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_Route->text());
            messages.push_back(CStatusMessage::getValidationError(m));
        }
        else
            flightPlan.setRoute(v);

        v = ui->pte_Remarks->toPlainText().trimmed();
        if (v.length() > 100)
        {
            QString m = QString("Length exceeded (100 chars max.) %1").arg(this->ui->lbl_Remarks->text());
            messages.push_back(CStatusMessage::getValidationError(m));
        }
        else
            flightPlan.setRemarks(v);

        v = ui->le_EstimatedTimeEnroute->text();
        if (v.isEmpty() || v == defaultTime())
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_EstimatedTimeEnroute->text());
            messages.push_back(CStatusMessage::getValidationError(m));
        }
        else
            flightPlan.setEnrouteTime(v);

        v = ui->le_FuelOnBoard->text();
        if (v.isEmpty() || v == defaultTime())
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_FuelOnBorad->text());
            messages.push_back(CStatusMessage::getValidationError(m));
        }
        else
            flightPlan.setFuelTime(v);

        v = ui->le_TakeOffTimePlanned->text();
        if (v.isEmpty() || v == defaultTime())
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_TakeOffTimePlanned->text());
            messages.push_back(CStatusMessage::getValidationError(m));
        }
        else
            flightPlan.setTakeoffTimePlanned(v);

        v = ui->le_CrusingAltitude->text().trimmed();
        CAltitude cruisingAltitude(v);
        if (v.isEmpty() || cruisingAltitude.isNull())
        {
            QString m = QString("Wrong %1").arg(this->ui->lbl_CrusingAltitude->text());
            messages.push_back(CStatusMessage::getValidationError(m));
        }
        else
            flightPlan.setCruiseAltitude(cruisingAltitude);

        v = this->ui->le_AlternateAirport->text();
        if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_AlternateAirport->text());
            messages.push_back(CStatusMessage::getValidationError(m));
            flightPlan.setAlternateAirportIcao(defaultIcao());
        }
        else
            flightPlan.setAlternateAirportIcao(v);

        v = this->ui->le_DestinationAirport->text();
        if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_DestinationAirport->text());
            messages.push_back(CStatusMessage::getValidationError(m));
            flightPlan.setDestinationAirportIcao(defaultIcao());
        }
        else
            flightPlan.setDestinationAirportIcao(v);

        v = this->ui->le_CruiseTrueAirspeed->text();
        BlackMisc::PhysicalQuantities::CSpeed cruiseTAS;
        cruiseTAS.parseFromString(v);
        if (cruiseTAS.isNull())
        {
            QString m = QString("Wrong TAS, %1").arg(this->ui->lbl_CruiseTrueAirspeed->text());
            messages.push_back(CStatusMessage::getValidationError(m));
            flightPlan.setDestinationAirportIcao(defaultIcao());
        }
        else
            flightPlan.setCruiseTrueAirspeed(cruiseTAS);

        v = this->ui->le_OriginAirport->text();
        if (v.isEmpty() || v.endsWith(defaultIcao(), Qt::CaseInsensitive))
        {
            QString m = QString("Missing %1").arg(this->ui->lbl_OriginAirport->text());
            messages.push_back(CStatusMessage::getValidationError(m));
            flightPlan.setOriginAirportIcao(defaultIcao());
        }
        else
            flightPlan.setOriginAirportIcao(v);

        return messages;
    }

    void CFlightPlanComponent::sendFlightPlan()
    {
        CFlightPlan flightPlan;
        CStatusMessageList messages = this->validateAndInitializeFlightPlan(flightPlan);
        if (messages.isEmpty())
        {
            // no error, send if possible
            CStatusMessage m;
            if (this->getIContextNetwork()->isConnected())
            {
                flightPlan.setWhenLastSent(QDateTime::currentDateTimeUtc());
                this->getIContextNetwork()->sendFlightPlan(flightPlan);
                this->ui->le_LastSent->setText(flightPlan.whenLastSent().toString());
                m = CStatusMessage::getInfoMessage("Sent flight plan", CStatusMessage::TypeTrafficNetwork);
            }
            else
            {
                flightPlan.setWhenLastSent(QDateTime());
                this->ui->le_LastSent->clear();
                m = CStatusMessage::getErrorMessage("No errors, but not connected, cannot send flight plan", CStatusMessage::TypeTrafficNetwork);
            }
            this->sendStatusMessage(m);
            this->m_flightPlan = flightPlan; // last valid FP
        }
        else
        {
            this->sendStatusMessages(messages);
        }
    }

    void CFlightPlanComponent::validateFlightPlan()
    {
        CFlightPlan flightPlan;
        CStatusMessageList messages = this->validateAndInitializeFlightPlan(flightPlan);
        if (messages.isEmpty())
        {
            this->sendStatusMessage(CStatusMessage::getInfoMessage("No errors", CStatusMessage::TypeTrafficNetwork));
        }
        else
        {
            this->sendStatusMessages(messages);
        }
    }

    void CFlightPlanComponent::resetFlightPlan()
    {
        if (this->getIContextNetwork())
        {
            this->prefillWithAircraftData(this->getIContextOwnAircraft()->getOwnAircraft());
        }
        this->ui->le_AircraftRegistration->clear();
        this->ui->le_AirlineOperator->clear();
        this->ui->le_CrusingAltitude->setText("FL70");
        this->ui->le_CruiseTrueAirspeed->setText("100 kts");
        this->ui->pte_Remarks->clear();
        this->ui->pte_Route->clear();
        this->ui->le_AlternateAirport->setText(defaultIcao());
        this->ui->le_DestinationAirport->setText(defaultIcao());
        this->ui->le_OriginAirport->setText(defaultIcao());
        this->ui->le_FuelOnBoard->setText(defaultTime());
        this->ui->le_EstimatedTimeEnroute->setText(defaultTime());
        this->ui->le_TakeOffTimePlanned->setText(QDateTime::currentDateTimeUtc().addSecs(30 * 60).toString("hh:mm"));
    }

    void CFlightPlanComponent::loadFlightPlan()
    {

    }

    void CFlightPlanComponent::buildRemarkString()
    {
        QString rem;
        QString v = this->ui->cb_VoiceCapabilities->currentText().toUpper();
        if (v.contains("TEXT"))
            rem.append("/T/ ");
        else if (v.contains("VOICE"))
            rem.append("/V/ ");
        else if (v.contains("RECEIVE"))
            rem.append("/R/ ");

        v = this->ui->le_AirlineOperator->text().trimmed();
        if (!v.isEmpty()) rem.append("OPR/").append(v).append(" ");

        v = this->ui->le_AircraftRegistration->text().trimmed();
        if (!v.isEmpty()) rem.append("REG/").append(v).append(" ");

        v = this->ui->cb_PilotRating->currentText().toUpper();
        if (v.contains("P1"))
            rem.append("PR/P1 ");
        else if (v.contains("P2"))
            rem.append("PR/P2 ");
        else if (v.contains("P3"))
            rem.append("PR/P3 ");
        else if (v.contains("P4"))
            rem.append("PR/P4 ");
        else if (v.contains("P5"))
            rem.append("PR/P5 ");

        v = this->ui->cb_RequiredNavigationPerformance->currentText().toUpper();
        if (v.contains("10"))
            rem.append("RNP10 ");
        else if (v.contains("4"))
            rem.append("RNP4 ");

        v = this->ui->cb_NavigationEquipment->currentText().toUpper();
        if (v.contains("VORS"))
            rem.append("NAV/VORNDB ");
        else if (v.contains("SIDS"))
            rem.append("NAV/GPSRNAV ");
        if (v.contains("DEFAULT"))
            rem.append("NAV/GPS ");
        else if (v.contains("OCEANIC"))
            rem.append("NAV/GPSOCEANIC ");

        v = this->ui->cb_PerformanceCategory->currentText().toUpper();
        if (v.startsWith("A"))
            rem.append("PER/A ");
        else if (v.startsWith("B"))
            rem.append("PER/B ");
        else if (v.startsWith("C"))
            rem.append("PER/C ");
        else if (v.startsWith("D"))
            rem.append("PER/D ");
        else if (v.startsWith("E"))
            rem.append("PER/E ");

        if (this->ui->frp_SelcalCode->hasValidCode())
        {
            rem.append("SEL/").append(this->ui->frp_SelcalCode->getSelcalCode());
            rem.append(" ");
        }

        if (this->ui->cb_NoSidsStarts->isChecked())
            rem.append("NO SID/STAR ");

        v = this->ui->pte_AdditionalRemarks->toPlainText().trimmed();
        if (!v.isEmpty()) rem.append(v);

        rem = rem.simplified().trimmed();
        this->ui->pte_RemarksGenerated->setPlainText(rem);
    }

    void CFlightPlanComponent::copyRemarks()
    {
        this->ui->pte_Remarks->setPlainText(this->ui->pte_RemarksGenerated->toPlainText());
        this->sendStatusMessage(CStatusMessage::getInfoMessage("Copied remarks", CStatusMessage::TypeTrafficNetwork));
    }

    void CFlightPlanComponent::currentTabGenerator()
    {
        this->setCurrentWidget(this->ui->tb_RemarksGenerator);
    }
}
