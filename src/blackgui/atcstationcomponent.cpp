#include "blackmisc/avinformationmessage.h"
#include "atcstationcomponent.h"
#include "ui_atcstationcomponent.h"

using namespace BlackMisc::Aviation;
using namespace BlackCore;

namespace BlackGui
{

    CAtcStationComponent::CAtcStationComponent(QWidget *parent) :
        QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CAtcStationComponent), m_timerComponent(nullptr)
    {
        ui->setupUi(this);
        this->m_timerComponent = new CTimerBasedComponent(SLOT(update()), this);
        this->ui->tvp_AtcStationsBooked->setStationMode(CAtcStationListModel::StationsBooked);

        // Signal / Slots
        bool connected = this->connect(this->ui->le_AtcStationsOnlineMetar, SIGNAL(returnPressed()), this, SLOT(getMetar()));
        Q_ASSERT(connected);
        connected = this->connect(this->ui->pb_AtcStationsLoadMetar, SIGNAL(clicked()), this, SLOT(getMetar()));
        Q_ASSERT(connected);
        this->connect(this, &QTabWidget::currentChanged, this, &CAtcStationComponent::atcStationsTabChanged);
        this->connect(this->ui->pb_ReloadAtcStationsBooked, &QPushButton::clicked, this, &CAtcStationComponent::reloadAtcStationsBooked);
        this->connect(this->ui->tvp_AtcStationsOnline, &QTableView::clicked, this, &CAtcStationComponent::onlineAtcStationSelected);
        this->connect(this->ui->pb_AtcStationsAtisReload, &QPushButton::clicked, this, &CAtcStationComponent::requestAtis);
    }

    CAtcStationComponent::~CAtcStationComponent()
    {
        delete ui;
    }

    void CAtcStationComponent::runtimeHasBeenSet()
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getIContextNetwork());
        this->connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnline, this, &CAtcStationComponent::changedAtcStationsOnline);
        this->connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsBooked, this, &CAtcStationComponent::changedAtcStationsBooked);
    }

    void CAtcStationComponent::update()
    {
        Q_ASSERT(this->ui->tvp_AtcStationsBooked);
        Q_ASSERT(this->ui->tvp_AtcStationsOnline);
        Q_ASSERT(this->getIContextNetwork());

        if (this->getIContextNetwork()->isConnected())
        {
            // initial read for bookings
            if (this->ui->tvp_AtcStationsBooked->isEmpty()) this->reloadAtcStationsBooked();

            // update
            if (this->m_timestampOnlineStationsChanged.isNull() || this->m_timestampLastReadOnlineStations.isNull() ||
                    (this->m_timestampOnlineStationsChanged > this->m_timestampLastReadOnlineStations))
            {
                this->ui->tvp_AtcStationsOnline->update(this->getIContextNetwork()->getAtcStationsOnline());
                this->m_timestampLastReadOnlineStations = QDateTime::currentDateTimeUtc();
                this->m_timestampOnlineStationsChanged = this->m_timestampLastReadOnlineStations;
            }
        }
        else
        {
            this->ui->le_AtcStationsOnlineMetar->clear();
        }
    }

    void CAtcStationComponent::reloadAtcStationsBooked()
    {
        Q_ASSERT(this->ui->tvp_AtcStationsBooked);
        Q_ASSERT(this->getIContextNetwork());

        if (this->getIContextNetwork()->isConnected())
        {
            this->ui->tvp_AtcStationsBooked->update(this->getIContextNetwork()->getAtcStationsBooked());
            this->m_timestampLastReadBookedStations = QDateTime::currentDateTimeUtc();
        }
    }

    void CAtcStationComponent::changedAtcStationsOnline()
    {
        this->m_timestampOnlineStationsChanged = QDateTime::currentDateTimeUtc();
    }

    void CAtcStationComponent::changedAtcStationsBooked()
    {
        this->reloadAtcStationsBooked();
    }

    void CAtcStationComponent::getMetar(const QString &airportIcaoCode)
    {
        if (!this->getIContextNetwork()->isConnected()) return;
        QString icao = airportIcaoCode.isEmpty() ? this->ui->le_AtcStationsOnlineMetar->text().trimmed().toUpper() : airportIcaoCode.trimmed().toUpper();
        this->ui->le_AtcStationsOnlineMetar->setText(icao);
        if (icao.length() != 4) return;
        CInformationMessage metar = this->getIContextNetwork()->getMetar(icao);
        if (metar.getType() != CInformationMessage::METAR) return;
        if (metar.isEmpty()) return;
        this->ui->te_AtcStationsOnlineInfo->setText(metar.getMessage());
    }

    void CAtcStationComponent::onlineAtcStationSelected(QModelIndex index)
    {
        this->ui->te_AtcStationsOnlineInfo->setText(""); // reset
        const CAtcStation stationClicked = this->ui->tvp_AtcStationsOnline->derivedModel()->at(index);
        QString infoMessage;

        if (stationClicked.hasAtis())
        {
            infoMessage.append(stationClicked.getAtis().getMessage());
        }
        if (stationClicked.hasMetar())
        {
            if (!infoMessage.isEmpty()) infoMessage.append("\n\n");
            infoMessage.append(stationClicked.getMetar().getMessage());
        }
        this->ui->te_AtcStationsOnlineInfo->setText(infoMessage);
    }

    void CAtcStationComponent::atcStationsTabChanged()
    {
        if (this->currentWidget() == this->ui->tb_AtcStationsOnline)
        {
            if (this->m_timestampLastReadBookedStations.isNull())
                this->reloadAtcStationsBooked();
        }
    }

    void CAtcStationComponent::requestAtis()
    {
        if (!this->getIContextNetwork()->isConnected()) return;
        this->getIContextNetwork()->requestAtisUpdates();
    }
}
