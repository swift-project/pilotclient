// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/atcstationcomponent.h"
#include "blackgui/views/atcstationview.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/atcstationtreemodel.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/infoarea.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "ui_atcstationcomponent.h"

#include <QAbstractItemModel>
#include <QLineEdit>
#include <QModelIndex>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStringBuilder>
#include <QTabBar>
#include <QGroupBox>
#include <QTableView>
#include <QTextEdit>
#include <QTimer>
#include <QTreeView>
#include <QCompleter>
#include <QPointer>

using namespace BlackGui::Models;
using namespace BlackGui::Views;
using namespace BlackGui::Settings;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CAtcStationComponent::CAtcStationComponent(QWidget *parent) : COverlayMessagesFrameEnableForDockWidgetInfoArea(parent),
                                                                  CIdentifiable(this),
                                                                  ui(new Ui::CAtcStationComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        ui->setupUi(this);
        ui->tw_Atc->setCurrentIndex(0);
        ui->tw_Atc->tabBar()->setExpanding(false);
        ui->tw_Atc->tabBar()->setUsesScrollButtons(true);
        CUpperCaseValidator *ucv = new CUpperCaseValidator(ui->le_AtcStationsOnlineMetar);
        ui->le_AtcStationsOnlineMetar->setValidator(ucv);

        // some icons
        ui->tb_AtcStationsAtisReload->setIcon(CIcons::atis());
        ui->tb_AtcStationsAtisReload->setText("ATIS");
        ui->tb_AtcStationsAtisReload->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->tb_AtcStationsLoadMetar->setIcon(CIcons::metar());
        ui->tb_AtcStationsLoadMetar->setText("METAR");
        ui->tb_AtcStationsLoadMetar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->tb_Audio->setIcon(CIcons::appAudio16());
        ui->tb_Audio->setText("Audio");
        ui->tb_Audio->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->tb_TextMessageOverlay->setIcon(CIcons::appTextMessages16());
        ui->tb_TextMessageOverlay->setText("Text");
        ui->tb_TextMessageOverlay->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        // set station mode
        ui->tvp_AtcStationsOnline->setStationMode(CAtcStationListModel::StationsOnline);
        ui->tvp_AtcStationsOnlineTree->setColumns(ui->tvp_AtcStationsOnline->getColumns());

        // menus
        ui->tvp_AtcStationsOnline->menuRemoveItems(CAtcStationView::MenuClear);

        // Signal / Slots
        connect(ui->le_AtcStationsOnlineMetar, &QLineEdit::returnPressed, this, &CAtcStationComponent::getMetarAsEntered);
        connect(ui->tb_AtcStationsLoadMetar, &QPushButton::clicked, this, &CAtcStationComponent::getMetarAsEntered);
        connect(ui->tb_Audio, &QPushButton::clicked, this, &CAtcStationComponent::requestAudioWidget);
        connect(ui->tb_TextMessageOverlay, &QPushButton::clicked, this, &CAtcStationComponent::showOverlayInlineTextMessage);

        connect(ui->tvp_AtcStationsOnline, &CAtcStationView::objectClicked, this, &CAtcStationComponent::onOnlineAtcStationVariantSelected, Qt::QueuedConnection);
        connect(ui->tvp_AtcStationsOnline, &CAtcStationView::objectSelected, this, &CAtcStationComponent::onOnlineAtcStationVariantSelected, Qt::QueuedConnection);
        connect(ui->tvp_AtcStationsOnline, &CAtcStationView::testRequestDummyAtcOnlineStations, this, &CAtcStationComponent::testCreateDummyOnlineAtcStations);
        connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestUpdate, this, &CAtcStationComponent::requestOnlineStationsUpdate);
        connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestNewBackendData, this, &CAtcStationComponent::requestOnlineStationsUpdate);
        connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestComFrequency, this, &CAtcStationComponent::setComFrequency);
        connect(ui->tvp_AtcStationsOnline, &CAtcStationView::requestTextMessageWidget, this, &CAtcStationComponent::requestTextMessageWidget);

        connect(ui->tvp_AtcStationsOnlineTree, &CAtcStationTreeView::requestComFrequency, this, &CAtcStationComponent::setComFrequency);
        connect(ui->tvp_AtcStationsOnlineTree, &CAtcStationTreeView::objectSelected, this, &CAtcStationComponent::onOnlineAtcStationSelected, Qt::QueuedConnection);
        connect(ui->tvp_AtcStationsOnlineTree, &CAtcStationTreeView::requestTextMessageWidget, this, &CAtcStationComponent::requestTextMessageWidget);

        connect(ui->comp_AtcStationsSettings, &CSettingsAtcStationsInlineComponent::changed, this, &CAtcStationComponent::forceUpdate, Qt::QueuedConnection);

        connect(ui->tb_AtcStationsAtisReload, &QPushButton::clicked, this, &CAtcStationComponent::requestAtisUpdates);
        connect(&m_updateTimer, &QTimer::timeout, this, &CAtcStationComponent::update);

        // Group box
        connect(ui->gb_Details, &QGroupBox::toggled, this, &CAtcStationComponent::onDetailsToggled, Qt::QueuedConnection);

        // runtime based connects
        if (sGui)
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnlineDigest, this, &CAtcStationComponent::changedAtcStationsOnline, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationOnlineConnectionStatus, this, &CAtcStationComponent::changedAtcStationOnlineConnectionStatus, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAtcStationComponent::connectionStatusChanged, Qt::QueuedConnection);
        }

        // selection
        ui->tvp_AtcStationsOnline->acceptClickSelection(true);
        ui->tvp_AtcStationsOnline->acceptRowSelection(true);

        // before splitter
        // QVBoxLayout *layout = this->vLayout();
        // m_stretch.push_back(layout->stretch(0));
        // m_stretch.push_back(layout->stretch(1));

        // no effect
        ui->sp_AtcSplitter->setStretchFactor(0, 3);
        ui->sp_AtcSplitter->setStretchFactor(1, 1);

        // web readers
        if (sGui && sGui->hasWebDataServices())
        {
            connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAirportsRead, this, &CAtcStationComponent::airportsRead);
            this->airportsRead();
        }

        // init settings
        this->settingsChanged();
    }

    CAtcStationComponent::~CAtcStationComponent()
    {}

    void CAtcStationComponent::setTab(AtcTab tab)
    {
        const int t = static_cast<int>(tab);
        ui->tw_Atc->setCurrentIndex(t);
    }

    int CAtcStationComponent::countOnlineStations() const
    {
        return ui->tvp_AtcStationsOnline->rowCount();
    }

    bool CAtcStationComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
    {
        CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
        const bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CAtcStationComponent::infoAreaTabBarChanged, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
        Q_ASSERT_X(parentDockableWidget, Q_FUNC_INFO, "missing parent");
        return c && parentDockableWidget;
    }

    void CAtcStationComponent::forceUpdate()
    {
        m_timestampOnlineStationsChanged = QDateTime::currentDateTimeUtc();
        this->update();
    }

    void CAtcStationComponent::update()
    {
        if (!this->canAccessContext()) { return; }
        Q_ASSERT(ui->tvp_AtcStationsOnline);

        // check if component is visible, if we have already data then skip udpate
        const bool hasData = this->countOnlineStations() > 0;
        if (hasData && !this->isVisibleWidget())
        {
            // Update skipped, as not visible
            ui->tvp_AtcStationsOnline->hideLoadIndicator();
            return;
        }

        // online stations, only when connected
        if (sGui->getIContextNetwork()->isConnected())
        {
            // update
            if (m_timestampOnlineStationsChanged > m_timestampLastReadOnlineStations)
            {
                const CAtcStationsSettings settings = ui->comp_AtcStationsSettings->getSettings();
                CAtcStationList onlineStations = sGui->getIContextNetwork()->getAtcStationsOnline(true);
                const int allStationsCount = onlineStations.sizeInt();
                int inRangeCount = -1;

                if (settings.showOnlyWithValidFrequency()) { onlineStations = onlineStations.stationsWithValidFrequency(); }
                if (settings.showOnlyInRange())
                {
                    onlineStations.removeIfOutsideRange();
                    inRangeCount = onlineStations.sizeInt();
                }

                const int stationsCount = onlineStations.sizeInt();
                ui->tvp_AtcStationsOnline->updateContainerMaybeAsync(onlineStations);
                m_timestampLastReadOnlineStations = QDateTime::currentDateTimeUtc();
                m_timestampOnlineStationsChanged = m_timestampLastReadOnlineStations;
                this->updateTreeView();
                this->setOnlineTabs(allStationsCount, stationsCount);
                ui->comp_AtcStationsSettings->setCounts(allStationsCount, inRangeCount);

                if (stationsCount < 1 && allStationsCount > 0)
                {
                    const QString msg = QStringLiteral("All %1 ATC stations are filtered").arg(allStationsCount);
                    ui->tvp_AtcStationsOnline->showOverlayHTMLMessage(msg, 5000);
                    ui->tvp_AtcStationsOnlineTree->showOverlayHTMLMessage(msg, 5000);
                }

                if (stationsCount < 1)
                {
                    m_selectedCallsign.clear();
                }
                else if (!m_selectedCallsign.isEmpty() && onlineStations.containsCallsign(m_selectedCallsign))
                {
                    const CAtcStation lastSelected = onlineStations.findFirstByCallsign(m_selectedCallsign);
                    if (lastSelected.hasCallsign())
                    {
                        // avoid override of manually entered METAR callsigns
                        const CCallsign currentCs(ui->le_AtcStationsOnlineMetar->text());
                        if (currentCs.isEmpty() || currentCs == lastSelected.getCallsign())
                        {
                            this->triggerOnlineAtcStationSelected(lastSelected);
                        }
                    }
                } // stations
            }
        }
        else
        {
            m_selectedCallsign.clear();
            ui->tvp_AtcStationsOnline->clear();
            this->updateTreeView();
            this->setOnlineTabs(0, 0);
        }
    }

    void CAtcStationComponent::changedAtcStationOnlineConnectionStatus(const CAtcStation &station, bool added)
    {
        // trick here is, we want to display a station ASAP
        ui->tvp_AtcStationsOnline->changedAtcStationConnectionStatus(station, added);
    }

    void CAtcStationComponent::getMetarAsEntered()
    {
        this->getMetar("");
    }

    void CAtcStationComponent::getMetar(const QString &airportIcaoCode)
    {
        if (!this->canAccessContext()) { return; }
        const CAirportIcaoCode icao(airportIcaoCode.isEmpty() ? ui->le_AtcStationsOnlineMetar->text().trimmed().toUpper() : airportIcaoCode.trimmed().toUpper());
        ui->le_AtcStationsOnlineMetar->setText(icao.asString());
        if (!icao.hasValidIcaoCode(true)) { return; }
        const CMetar metar(sGui->getIContextNetwork()->getMetarForAirport(icao));
        if (metar.hasMessage())
        {
            const QString metarText = metar.getMessage() % u"\n\n" % metar.getMetarText();
            ui->te_AtcStationsOnlineInfo->setText(metarText);
        }
        else
        {
            ui->te_AtcStationsOnlineInfo->clear();
        }
    }

    void CAtcStationComponent::changedAtcStationsOnline()
    {
        // just update timestamp, data will be pulled by timer
        // the timestamp will tell if there are any newer data
        m_timestampOnlineStationsChanged = QDateTime::currentDateTimeUtc();
    }

    void CAtcStationComponent::connectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        if (to.isConnected())
        {
            ui->tvp_AtcStationsOnline->clear();
            this->updateTreeView();
            m_updateTimer.start();
        }
        else if (to.isDisconnected())
        {
            m_updateTimer.stop();
            this->clearOnlineViews();
            this->update();
        }
    }

    void CAtcStationComponent::testCreateDummyOnlineAtcStations(int number)
    {
        if (!sGui || !sGui->getIContextNetwork()) { return; }
        if (this->canAccessContext())
        {
            sGui->getIContextNetwork()->testCreateDummyOnlineAtcStations(number);
        }
    }

    void CAtcStationComponent::requestOnlineStationsUpdate()
    {
        m_timestampLastReadOnlineStations.setMSecsSinceEpoch(0); // mark as outdated
        this->update();
    }

    void CAtcStationComponent::infoAreaTabBarChanged(int index)
    {
        // ignore in those cases
        if (!this->isVisibleWidget()) { return; }
        if (this->isParentDockWidgetFloating()) { return; }

        // here I know I am the selected widget, update, but keep GUI responsive (-> timer)
        const QPointer<CAtcStationComponent> myself(this);
        QTimer::singleShot(1000, this, [=] {
            if (!sApp || sApp->isShuttingDown() || !myself) { return; }
            this->update();
        });
        Q_UNUSED(index)
    }

    void CAtcStationComponent::setOnlineTabs(int count, int filtered)
    {
        const int io = ui->tw_Atc->indexOf(ui->tb_AtcStationsOnline);
        const int it = ui->tw_Atc->indexOf(ui->tb_AtcStationsOnlineTree);
        static const QString o = ui->tw_Atc->tabBar()->tabText(io);
        static const QString t = ui->tw_Atc->tabBar()->tabText(it);
        const bool isFiltered = filtered < count && filtered >= 0;
        const QString filteredInfo = isFiltered ? QStringLiteral(" (%1 of %2)").arg(filtered).arg(count) : QStringLiteral(" (%1)").arg(count);
        ui->tw_Atc->tabBar()->setTabText(io, o % filteredInfo);
        ui->tw_Atc->tabBar()->setTabText(it, t % filteredInfo);
    }

    void CAtcStationComponent::setComFrequency(const PhysicalQuantities::CFrequency &frequency, CComSystem::ComUnit unit)
    {
        if (unit != CComSystem::Com1 && unit != CComSystem::Com2) { return; }
        if (!CComSystem::isValidComFrequency(frequency)) { return; }
        sGui->getIContextOwnAircraft()->updateActiveComFrequency(frequency, unit, identifier());
    }

    void CAtcStationComponent::settingsChanged()
    {
        if (!this->canAccessContext()) { return; }
        const CViewUpdateSettings settings = m_settingsView.get();
        const int ms = settings.getAtcUpdateTime().toMs();
        const bool connected = sGui->getIContextNetwork()->isConnected();
        m_updateTimer.setInterval(ms);
        if (connected)
        {
            m_timestampOnlineStationsChanged = QDateTime::currentDateTimeUtc();
            m_updateTimer.start(ms); // restart
            this->update();
        }
        else
        {
            m_updateTimer.stop();
        }
    }

    void CAtcStationComponent::airportsRead()
    {
        this->initCompleters();
    }

    void CAtcStationComponent::updateTreeView()
    {
        ui->tvp_AtcStationsOnlineTree->updateContainer(ui->tvp_AtcStationsOnline->container());
        ui->tvp_AtcStationsOnlineTree->fullResizeToContents();
    }

    void CAtcStationComponent::initCompleters()
    {
        if (!sGui || !sGui->getWebDataServices()) { return; }
        const QStringList airports = sGui->getWebDataServices()->getAirports().allIcaoCodes(true);
        if (!airports.isEmpty())
        {
            QCompleter *airportCompleter = new QCompleter(airports, this);
            const int w5chars = airportCompleter->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
            airportCompleter->popup()->setMinimumWidth(w5chars * 5);
            ui->le_AtcStationsOnlineMetar->setCompleter(airportCompleter);
        }
    }

    void CAtcStationComponent::onOnlineAtcStationVariantSelected(const CVariant &object)
    {
        ui->te_AtcStationsOnlineInfo->setText(""); // reset
        if (!object.isValid() || !object.canConvert<CAtcStation>()) { return; }
        const CAtcStation station = object.valueOrDefault(CAtcStation());
        this->onOnlineAtcStationSelected(station);
    }

    void CAtcStationComponent::triggerOnlineAtcStationSelected(const CAtcStation &station)
    {
        // pass copy, not reference -> can crash
        QPointer<CAtcStationComponent> myself(this);
        QTimer::singleShot(500, this, [=] {
            if (myself) { myself->onOnlineAtcStationSelected(station); }
        });
    }

    void CAtcStationComponent::onOnlineAtcStationSelected(const CAtcStation &station)
    {
        if (!station.hasCallsign()) { return; }
        const QString infoMessage =
            station.getCallsignAsString() % u": " % station.getFrequency().valueRoundedWithUnit(CFrequencyUnit::MHz(), 3) %
            (station.hasAtis() ? u"\n\n" % station.getAtis().getMessage() : QStringLiteral("")) %
            (station.hasMetar() ? u"\n\n" % station.getMetar().getMessage() : QStringLiteral(""));
        ui->te_AtcStationsOnlineInfo->setText(infoMessage);
        ui->le_AtcStationsOnlineMetar->setText(station.getCallsign().asString());
        m_selectedCallsign = station.getCallsign();
    }

    void CAtcStationComponent::requestAtisUpdates()
    {
        if (!this->canAccessContext()) { return; }
        sGui->getIContextNetwork()->requestAtisUpdates();
        if (ui->tw_Atc->currentIndex() == TabAtcOnline)
        {
            ui->tvp_AtcStationsOnline->showOverlayHTMLMessage("Requested ATIS update", 5000);
        }
        else
        {
            ui->tvp_AtcStationsOnlineTree->showOverlayHTMLMessage("Requested ATIS update", 5000);
        }
    }

    bool CAtcStationComponent::canAccessContext() const
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return false; }
        return true;
    }

    void CAtcStationComponent::clearOnlineViews()
    {
        ui->tvp_AtcStationsOnline->clear();
        ui->tvp_AtcStationsOnlineTree->clear();
        m_selectedCallsign.clear();
    }

    void CAtcStationComponent::showOverlayInlineTextMessage()
    {
        // COverlayMessagesFrame::showOverlayInlineTextMessage(TextMessagesCom1);
        emit requestTextMessageEntryTab(TextMessagesCom1);
    }

    void CAtcStationComponent::onDetailsToggled(bool checked)
    {
        QVBoxLayout *layout = this->vLayout();
        if (layout)
        {
            if (checked)
            {
                if (!m_splitterSizes.isEmpty())
                {
                    ui->sp_AtcSplitter->setSizes(m_splitterSizes);
                }
            }
            else
            {
                m_splitterSizes = ui->sp_AtcSplitter->sizes();
                if (m_splitterSizes.size() > 1)
                {
                    int min, max;
                    ui->sp_AtcSplitter->getRange(1, &min, &max);
                    QList<int> newSizes;
                    newSizes.push_back(qMax(0, m_splitterSizes.first() + m_splitterSizes.last() - min));
                    newSizes.push_back(min);
                    ui->sp_AtcSplitter->setSizes(newSizes);
                }
            }
        }

        ui->te_AtcStationsOnlineInfo->setVisible(checked);
        ui->comp_AtcStationsSettings->setVisible(checked);
        ui->le_AtcStationsOnlineMetar->setVisible(checked);
        ui->tb_AtcStationsAtisReload->setVisible(checked);
        ui->tb_AtcStationsLoadMetar->setVisible(checked);
        ui->tb_TextMessageOverlay->setVisible(checked);
        ui->tb_Audio->setVisible(checked);
    }

    QVBoxLayout *CAtcStationComponent::vLayout() const
    {
        QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->layout());
        return layout;
    }
} // namespace
