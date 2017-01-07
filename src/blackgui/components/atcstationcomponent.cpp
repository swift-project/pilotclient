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
#include "blackgui/components/atcstationcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/infoarea.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/views/atcstationview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/compare.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/weather/metar.h"
#include "ui_atcstationcomponent.h"

#include <QAbstractItemModel>
#include <QLineEdit>
#include <QModelIndex>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTabBar>
#include <QTableView>
#include <QTextEdit>
#include <QTimer>
#include <QTreeView>
#include <QCompleter>

using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackGui::Views;
using namespace BlackGui::Settings;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CAtcStationComponent::CAtcStationComponent(QWidget *parent) :
            QTabWidget(parent),
            CIdentifiable(this),
            ui(new Ui::CAtcStationComponent)
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);
            this->tabBar()->setUsesScrollButtons(true);
            CUpperCaseValidator *ucv = new CUpperCaseValidator(ui->le_AtcStationsOnlineMetar);
            ui->le_AtcStationsOnlineMetar->setValidator(ucv);

            // some icons
            ui->pb_AtcStationsAtisReload->setIcon(CIcons::atis());
            ui->pb_AtcStationsAtisReload->setText("");
            ui->pb_AtcStationsLoadMetar->setIcon(CIcons::metar());
            ui->pb_AtcStationsLoadMetar->setText("");

            // set station mode
            ui->tvp_AtcStationsOnline->setStationMode(CAtcStationListModel::StationsOnline);
            ui->tvp_AtcStationsBooked->setStationMode(CAtcStationListModel::StationsBooked);

            // header
            ui->tvp_AtcStationsOnlineTree->setHeaderHidden(true);

            // Signal / Slots
            connect(ui->le_AtcStationsOnlineMetar, &QLineEdit::returnPressed, this, &CAtcStationComponent::ps_getMetarAsEntered);
            connect(ui->pb_AtcStationsLoadMetar, &QPushButton::clicked, this, &CAtcStationComponent::ps_getMetarAsEntered);
            connect(this, &QTabWidget::currentChanged, this, &CAtcStationComponent::ps_atcStationsTabChanged); // "local" tab changed (booked, online)
            connect(ui->tvp_AtcStationsOnline, &QTableView::clicked, this, &CAtcStationComponent::ps_onlineAtcStationSelected);
            connect(ui->tvp_AtcStationsOnline, &CAtcStationView::testRequestDummyAtcOnlineStations, this, &CAtcStationComponent::ps_testCreateDummyOnlineAtcStations);
            connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestUpdate, this, &CAtcStationComponent::ps_requestOnlineStationsUpdate);
            connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestNewBackendData, this, &CAtcStationComponent::ps_requestOnlineStationsUpdate);
            connect(ui->tvp_AtcStationsOnline, &CAtcStationView::modelDataChangedDigest, this, &CAtcStationComponent::ps_onCountChanged);
            connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestComFrequency, this, &CAtcStationComponent::ps_setComFrequency);
            connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestTextMessageWidget, this, &CAtcStationComponent::requestTextMessageWidget);

            connect(ui->tvp_AtcStationsBooked, &CAtcStationView::requestUpdate, this, &CAtcStationComponent::ps_reloadAtcStationsBooked);
            connect(ui->tvp_AtcStationsBooked, &CAtcStationView::requestNewBackendData, this, &CAtcStationComponent::ps_reloadAtcStationsBooked);
            connect(ui->tvp_AtcStationsBooked, &CAtcStationView::modelDataChangedDigest, this, &CAtcStationComponent::ps_onCountChanged);

            connect(ui->pb_AtcStationsAtisReload, &QPushButton::clicked, this, &CAtcStationComponent::ps_requestAtis);
            connect(&m_updateTimer, &QTimer::timeout, this, &CAtcStationComponent::update);

            // runtime based connects
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnlineDigest, this, &CAtcStationComponent::ps_changedAtcStationsOnline);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationsBookedDigest, this, &CAtcStationComponent::ps_changedAtcStationsBooked);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationOnlineConnectionStatus, this, &CAtcStationComponent::changedAtcStationOnlineConnectionStatus);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAtcStationComponent::ps_connectionStatusChanged);

            // web readers
            if (sGui->hasWebDataServices())
            {
                connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAirportsRead, this, &CAtcStationComponent::ps_airportsRead);
                this->ps_airportsRead();
            }

            // init settings
            this->settingsChanged();
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

        bool CAtcStationComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CAtcStationComponent::ps_infoAreaTabBarChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            Q_ASSERT_X(parentDockableWidget, Q_FUNC_INFO, "missing parent");
            return c && parentDockableWidget;
        }

        void CAtcStationComponent::update()
        {
            if (!sGui || !sGui->getIContextNetwork()) { return; }
            Q_ASSERT(ui->tvp_AtcStationsBooked);
            Q_ASSERT(ui->tvp_AtcStationsOnline);

            // check if component is visible, if we have already data then skip udpate
            const bool hasData = this->countBookedStations() > 0 || this->countOnlineStations() > 0;
            if (hasData && !this->isVisibleWidget())
            {
                // Update skipped, as not visible
                ui->tvp_AtcStationsBooked->hideLoadIndicator();
                ui->tvp_AtcStationsOnline->hideLoadIndicator();
                return;
            }

            // bookings
            if (this->m_timestampBookedStationsChanged > this->m_timestampLastReadBookedStations)
            {
                this->ps_reloadAtcStationsBooked();
            }

            // online stations, only when connected
            if (sGui->getIContextNetwork()->isConnected())
            {
                // update
                if (this->m_timestampOnlineStationsChanged > this->m_timestampLastReadOnlineStations)
                {
                    const CAtcStationsSettings settings = m_settingsAtc.getThreadLocal();
                    CAtcStationList onlineStations =
                        sGui->getIContextNetwork()->getAtcStationsOnline().stationsWithValidFrequency(); // alternatively: stationsWithValidVoiceRoom()

                    if (settings.showOnlyInRange())
                    {
                        onlineStations.removeIfOutsideRange();
                    }

                    ui->tvp_AtcStationsOnline->updateContainerMaybeAsync(onlineStations);
                    this->m_timestampLastReadOnlineStations = QDateTime::currentDateTimeUtc();
                    this->m_timestampOnlineStationsChanged = this->m_timestampLastReadOnlineStations;
                    this->updateTreeView();
                }
            }
            else
            {
                ui->tvp_AtcStationsOnline->clear();
                this->updateTreeView();
            }
        }

        void CAtcStationComponent::changedAtcStationOnlineConnectionStatus(const CAtcStation &station, bool added)
        {
            // trick here is, we want to display a station ASAP
            ui->tvp_AtcStationsOnline->changedAtcStationConnectionStatus(station, added);
        }

        void CAtcStationComponent::ps_getMetarAsEntered()
        {
            this->getMetar("");
        }

        void CAtcStationComponent::getMetar(const QString &airportIcaoCode)
        {
            const QString icao(airportIcaoCode.isEmpty() ? ui->le_AtcStationsOnlineMetar->text().trimmed().toUpper() : airportIcaoCode.trimmed().toUpper());
            ui->le_AtcStationsOnlineMetar->setText(icao);
            if (icao.length() != 4) { return; }
            CMetar metar(sGui->getIContextNetwork()->getMetarForAirport(icao));
            if (metar == CMetar())
            {
                ui->te_AtcStationsOnlineInfo->clear();
            }
            else
            {
                QString metarText = metar.getMessage();
                metarText += "\n\n";
                metarText += metar.getMetarText();
                ui->te_AtcStationsOnlineInfo->setText(metarText);
            }
            ui->le_AtcStationsOnlineMetar->clear();
        }

        void CAtcStationComponent::ps_reloadAtcStationsBooked()
        {
            Q_ASSERT(ui->tvp_AtcStationsBooked);
            Q_ASSERT(sGui->getIContextNetwork());

            QObject *sender = QObject::sender();
            if (sender == ui->tvp_AtcStationsBooked && sGui->getIContextNetwork())
            {
                // trigger new read, which takes some time. A signal will be received when this is done
                CLogMessage(this).info("Requested new bookings");
                sGui->getIContextNetwork()->requestAtcBookingsUpdate();
            }
            else
            {
                ui->tvp_AtcStationsBooked->updateContainerMaybeAsync(sGui->getIContextNetwork()->getAtcStationsBooked());
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
            // unlike online stations, this can happen if we are not connected to a FSD server

            this->m_timestampBookedStationsChanged = QDateTime::currentDateTimeUtc();
            if (this->m_updateTimer.isActive()) { return; } // update by timer
            this->update();
        }

        void CAtcStationComponent::ps_connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                ui->tvp_AtcStationsOnline->clear();
                this->updateTreeView();
                this->m_updateTimer.start();
            }
            else if (INetwork::isConnectedStatus(to))
            {
                this->m_updateTimer.stop();
            }
        }

        void CAtcStationComponent::ps_testCreateDummyOnlineAtcStations(int number)
        {
            if (sGui->getIContextNetwork())
            {
                sGui->getIContextNetwork()->testCreateDummyOnlineAtcStations(number);
            }
        }

        void CAtcStationComponent::ps_requestOnlineStationsUpdate()
        {
            this->m_timestampLastReadOnlineStations.setMSecsSinceEpoch(0); // mark as outdated
            this->update();
        }

        void CAtcStationComponent::ps_infoAreaTabBarChanged(int index)
        {
            // ignore in those cases
            if (!this->isVisibleWidget()) { return; }
            if (this->isParentDockWidgetFloating()) { return; }

            // here I know I am the selected widget, update, but keep GUI responsive (-> timer)
            QTimer::singleShot(1000, this, &CAtcStationComponent::update);
            Q_UNUSED(index);
        }

        void CAtcStationComponent::ps_onCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int io = this->indexOf(ui->tb_AtcStationsOnline);
            int ib = this->indexOf(ui->tb_AtcStationsBooked);
            QString o = this->tabBar()->tabText(io);
            QString b = this->tabBar()->tabText(ib);
            o = CGuiUtility::replaceTabCountValue(o, this->countOnlineStations());
            b = CGuiUtility::replaceTabCountValue(b, this->countBookedStations());
            this->tabBar()->setTabText(io, o);
            this->tabBar()->setTabText(ib, b);
        }

        void CAtcStationComponent::ps_setComFrequency(const PhysicalQuantities::CFrequency &frequency, CComSystem::ComUnit unit)
        {
            if (unit != CComSystem::Com1 && unit != CComSystem::Com2) { return; }
            if (!CComSystem::isValidComFrequency(frequency)) { return; }
            sGui->getIContextOwnAircraft()->updateActiveComFrequency(frequency, unit, identifier());
        }

        void CAtcStationComponent::settingsChanged()
        {
            const CViewUpdateSettings settings = this->m_settingsView.get();
            const int ms = settings.getAtcUpdateTime().toMs();
            this->m_updateTimer.setInterval(ms);
        }

        void CAtcStationComponent::ps_airportsRead()
        {
            this->initCompleters();
        }

        void CAtcStationComponent::updateTreeView()
        {
            //! \fixme EXPERIMENTAL CODE: change model so we can directly use hierarchies
            QAbstractItemModel *old = (ui->tvp_AtcStationsOnlineTree->model());
            ui->tvp_AtcStationsOnlineTree->setModel(
                ui->tvp_AtcStationsOnline->derivedModel()->toAtcGroupModel()
            );
            if (old) { old->deleteLater(); } // delete old model replaced by current model
            if (!ui->tvp_AtcStationsOnlineTree->model()) { return; }

            ui->tvp_AtcStationsOnlineTree->expandAll();
            for (int i = 0; i < ui->tvp_AtcStationsOnlineTree->model()->columnCount(); i++)
            {
                ui->tvp_AtcStationsOnlineTree->resizeColumnToContents(i);
            }
        }

        void CAtcStationComponent::initCompleters()
        {
            if (!sGui || !sGui->getWebDataServices()) { return; }
            const QStringList airports = sGui->getWebDataServices()->getAirports().allIcaoCodes(true);
            if (!airports.isEmpty())
            {
                QCompleter *airportCompleter = new QCompleter(airports, this);
                airportCompleter->popup()->setMinimumWidth(75);
                ui->le_AtcStationsOnlineMetar->setCompleter(airportCompleter);
            }
        }

        void CAtcStationComponent::ps_onlineAtcStationSelected(QModelIndex index)
        {
            ui->te_AtcStationsOnlineInfo->setText(""); // reset
            const CAtcStation stationClicked = ui->tvp_AtcStationsOnline->derivedModel()->at(index);
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
            ui->te_AtcStationsOnlineInfo->setText(infoMessage);
        }

        void CAtcStationComponent::ps_atcStationsTabChanged()
        {
            if (this->currentWidget() == ui->tb_AtcStationsBooked)
            {
                if (ui->tvp_AtcStationsBooked->isEmpty())
                {
                    this->ps_reloadAtcStationsBooked();
                }
            }
        }

        void CAtcStationComponent::ps_requestAtis()
        {
            if (!sGui->getIContextNetwork()->isConnected()) return;
            sGui->getIContextNetwork()->requestAtisUpdates();
        }
    } // namespace
} // namespace
