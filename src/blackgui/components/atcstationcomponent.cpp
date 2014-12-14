/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "atcstationcomponent.h"
#include "../views/atcstationview.h"
#include "../guiutility.h"
#include "ui_atcstationcomponent.h"
#include "blackmisc/avinformationmessage.h"
#include "blackmisc/logmessage.h"
#include "blackcore/context_network.h"

using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackGui::Views;
using namespace BlackMisc::Aviation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CAtcStationComponent::CAtcStationComponent(QWidget *parent) :
            QTabWidget(parent),
            ui(new Ui::CAtcStationComponent)
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);
            this->m_updateTimer = new CUpdateTimer(SLOT(update()), this);

            // some icons
            this->ui->pb_AtcStationsAtisReload->setIcon(CIcons::atis());
            this->ui->pb_AtcStationsAtisReload->setText("");
            this->ui->pb_AtcStationsLoadMetar->setIcon(CIcons::metar());
            this->ui->pb_AtcStationsLoadMetar->setText("");

            // set station mode
            this->ui->tvp_AtcStationsOnline->setStationMode(CAtcStationListModel::StationsOnline);
            this->ui->tvp_AtcStationsBooked->setStationMode(CAtcStationListModel::StationsBooked);
            this->ui->tvp_AtcStationsBooked->setResizeMode(CAtcStationView::ResizingOnce);

            // Signal / Slots
            bool connected = this->connect(this->ui->le_AtcStationsOnlineMetar, SIGNAL(returnPressed()), this, SLOT(getMetar()));
            Q_ASSERT(connected);
            connected = this->connect(this->ui->pb_AtcStationsLoadMetar, SIGNAL(clicked()), this, SLOT(getMetar()));
            Q_ASSERT(connected);
            Q_UNUSED(connected);
            connect(this, &QTabWidget::currentChanged, this, &CAtcStationComponent::ps_atcStationsTabChanged); // "local" tab changed (booked, online)
            connect(this->ui->tvp_AtcStationsOnline, &QTableView::clicked, this, &CAtcStationComponent::ps_onlineAtcStationSelected);
            connect(this->ui->tvp_AtcStationsOnline, &CAtcStationView::testRequestDummyAtcOnlineStations, this, &CAtcStationComponent::ps_testCreateDummyOnlineAtcStations);
            connect(this->ui->tvp_AtcStationsOnline, &CAtcStationView::requestUpdate, this, &CAtcStationComponent::ps_requestOnlineStationsUpdate);
            connect(this->ui->tvp_AtcStationsOnline, &CAtcStationView::countChanged, this, &CAtcStationComponent::ps_countChanged);
            connect(this->ui->tvp_AtcStationsOnline, &CAtcStationView::countChanged, this, &CAtcStationComponent::ps_countChanged);

            connect(this->ui->tvp_AtcStationsBooked, &CAtcStationView::requestUpdate, this, &CAtcStationComponent::ps_reloadAtcStationsBooked);
            connect(this->ui->tvp_AtcStationsBooked, &CAtcStationView::countChanged, this, &CAtcStationComponent::ps_countChanged);

            connect(this->ui->pb_AtcStationsAtisReload, &QPushButton::clicked, this, &CAtcStationComponent::ps_requestAtis);
        }

        CAtcStationComponent::~CAtcStationComponent()
        { }

        int CAtcStationComponent::countBookedStations() const
        {
            return ui->tvp_AtcStationsBooked->rowCount();
        }

        int CAtcStationComponent::countOnlineStations() const
        {
            return ui->tvp_AtcStationsOnline->rowCount();
        }

        void CAtcStationComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getIContextNetwork());
            if (this->getIContextNetwork())
            {
                this->connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnlineDigest, this, &CAtcStationComponent::ps_changedAtcStationsOnline);
                this->connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsBookedDigest, this, &CAtcStationComponent::ps_changedAtcStationsBooked);
                this->connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationOnlineConnectionStatus, this, &CAtcStationComponent::changedAtcStationOnlineConnectionStatus);
                this->connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAtcStationComponent::ps_connectionStatusChanged);
            }
            connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CAtcStationComponent::ps_infoAreaTabBarChanged);
        }

        void CAtcStationComponent::update()
        {
            Q_ASSERT(this->ui->tvp_AtcStationsBooked);
            Q_ASSERT(this->ui->tvp_AtcStationsOnline);
            Q_ASSERT(this->getIContextNetwork());

            // check if component is visible, if we have already data then skip udpate
            bool hasData = this->countBookedStations() > 0 || this->countOnlineStations() > 0;
            if (hasData && !this->isVisibleWidget())
            {
                // KWB remove: qDebug() will be removed soon
                qDebug() << this->objectName() << "Skipping update, not visible";
                return;
            }

            // bookings
            if (this->m_timestampBookedStationsChanged > this->m_timestampLastReadBookedStations)
            {
                this->ps_reloadAtcStationsBooked();
            }

            // online stations, only when connected
            if (this->getIContextNetwork()->isConnected())
            {
                // update
                if (this->m_timestampOnlineStationsChanged > this->m_timestampLastReadOnlineStations)
                {
                    this->ui->tvp_AtcStationsOnline->updateContainerMaybeAsync(this->getIContextNetwork()->getAtcStationsOnline());
                    this->m_timestampLastReadOnlineStations = QDateTime::currentDateTimeUtc();
                    this->m_timestampOnlineStationsChanged = this->m_timestampLastReadOnlineStations;
                }
            }
            else
            {
                this->ui->tvp_AtcStationsOnline->clear();
                this->ui->le_AtcStationsOnlineMetar->clear();
            }
        }

        void CAtcStationComponent::changedAtcStationOnlineConnectionStatus(const CAtcStation &station, bool added)
        {
            this->ui->tvp_AtcStationsOnline->changedAtcStationConnectionStatus(station, added);
        }

        void CAtcStationComponent::getMetar(const QString &airportIcaoCode)
        {
            if (!this->getIContextNetwork()->isConnected())
            {
                this->ui->te_AtcStationsOnlineInfo->clear();
                return;
            }
            QString icao = airportIcaoCode.isEmpty() ? this->ui->le_AtcStationsOnlineMetar->text().trimmed().toUpper() : airportIcaoCode.trimmed().toUpper();
            this->ui->le_AtcStationsOnlineMetar->setText(icao);
            if (icao.length() != 4) return;
            CInformationMessage metar = this->getIContextNetwork()->getMetar(icao);
            if (metar.getType() != CInformationMessage::METAR) return;
            if (metar.isEmpty())
            {
                this->ui->te_AtcStationsOnlineInfo->clear();
            }
            else
            {
                this->ui->te_AtcStationsOnlineInfo->setText(metar.getMessage());
            }
        }

        void CAtcStationComponent::ps_reloadAtcStationsBooked()
        {
            Q_ASSERT(this->ui->tvp_AtcStationsBooked);
            Q_ASSERT(this->getIContextNetwork());

            QObject *sender = QObject::sender();
            if (sender == this->ui->tvp_AtcStationsBooked && this->getIContextNetwork())
            {
                // trigger new read, which takes some time. A signal will be received when this is done
                CLogMessage(this).info("Requested new bookings");
                this->getIContextNetwork()->readAtcBookingsFromSource();
            }
            else
            {
                this->ui->tvp_AtcStationsBooked->updateContainer(this->getIContextNetwork()->getAtcStationsBooked());
                this->m_timestampLastReadBookedStations = QDateTime::currentDateTimeUtc();
            }
        }

        void CAtcStationComponent::ps_changedAtcStationsOnline()
        {
            // just update timestamp, data will be pulled by timer
            // the timestamp will tell if there are any newer data
            this->m_timestampOnlineStationsChanged = QDateTime::currentDateTimeUtc();
        }

        void CAtcStationComponent::ps_changedAtcStationsBooked()
        {
            // a change can mean a complete change of the bookings, or
            // a single value is updated (e.g. online status)
            // just update timestamp, data will be pulled by timer
            // the timestamp will tell if there are any newer data
            this->m_timestampBookedStationsChanged = QDateTime::currentDateTimeUtc();
        }

        void CAtcStationComponent::ps_connectionStatusChanged(uint from, uint to)
        {
            INetwork::ConnectionStatus fromStatus = static_cast<INetwork::ConnectionStatus>(from);
            INetwork::ConnectionStatus toStatus = static_cast<INetwork::ConnectionStatus>(to);
            Q_UNUSED(fromStatus);
            if (INetwork::isDisconnectedStatus(toStatus))
            {
                this->ui->tvp_AtcStationsOnline->clear();
                this->ui->le_AtcStationsOnlineMetar->clear();
            }
        }

        void CAtcStationComponent::ps_testCreateDummyOnlineAtcStations(int number)
        {
            if (this->getIContextNetwork())
            {
                this->getIContextNetwork()->testCreateDummyOnlineAtcStations(number);
            }
        }

        void CAtcStationComponent::ps_requestOnlineStationsUpdate()
        {
            this->m_updateTimer->fireTimer();
            this->m_timestampLastReadOnlineStations = CUpdateTimer::epoch();
        }

        void CAtcStationComponent::ps_infoAreaTabBarChanged(int index)
        {
            // ignore in those cases
            if (!this->isVisibleWidget()) return;
            if (this->isParentDockWidgetFloating()) return;

            // here I know I am the selected widget, update, but keep GUI responsive (-> timer)
            QTimer::singleShot(1000, this, SLOT(update()));
            Q_UNUSED(index);
        }

        void CAtcStationComponent::ps_countChanged(int count)
        {
            Q_UNUSED(count);
            int io = this->indexOf(this->ui->tb_AtcStationsOnline);
            int ib = this->indexOf(this->ui->tb_AtcStationsBooked);
            QString o = this->tabBar()->tabText(io);
            QString b = this->tabBar()->tabText(ib);
            o = CGuiUtility::replaceTabCountValue(o, this->countOnlineStations());
            b = CGuiUtility::replaceTabCountValue(b, this->countBookedStations());
            this->tabBar()->setTabText(io, o);
            this->tabBar()->setTabText(ib, b);
        }

        void CAtcStationComponent::ps_onlineAtcStationSelected(QModelIndex index)
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

        void CAtcStationComponent::ps_atcStationsTabChanged()
        {
            if (this->currentWidget() == this->ui->tb_AtcStationsBooked)
            {
                if (this->ui->tvp_AtcStationsBooked->isEmpty())
                {
                    this->ps_reloadAtcStationsBooked();
                }
            }
        }

        void CAtcStationComponent::ps_requestAtis()
        {
            if (!this->getIContextNetwork()->isConnected()) return;
            this->getIContextNetwork()->requestAtisUpdates();
        }
    } // namespace
} // namespace
