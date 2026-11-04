// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniclientswidget.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QtGlobal>

#include "core/context/contextnetwork.h"
#include "gui/guiapplication.h"
#include "gui/models/atcstationlistmodel.h"
#include "gui/models/simulatedaircraftlistmodel.h"
#include "gui/views/atcstationview.h"
#include "gui/views/simulatedaircraftview.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/simulation/simulatedaircraftlist.h"

using namespace swift::core::context;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::gui::models;
using namespace swift::gui::settings;
using namespace swift::gui::views;

namespace
{
    //! Hide all columns of the view whose header name is not in the given whitelist
    //! \remark must be a template because CViewBaseNonTemplate itself has no getColumns(),
    //!         only the templated CViewBase<T> (and thus CAtcStationView/CSimulatedAircraftView) provides it
    template <class ViewT>
    void restrictColumns(ViewT *view, const QStringList &keepColumns)
    {
        if (!view) { return; }
        const auto &columns = view->getColumns().columns();
        for (int i = 0; i < columns.size(); ++i)
        {
            const QString name = columns.at(i).getColumnName();
            view->setColumnHidden(i, !keepColumns.contains(name));
        }
    }
} // namespace

namespace swift::gui::components
{
    CMiniClientsWidget::CMiniClientsWidget(QWidget *parent) : QFrame(parent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

        m_atcView = new CAtcStationView(this);
        m_atcView->setStationMode(CAtcStationListModel::StationsOnline);
        m_atcView->acceptClickSelection(true);
        m_atcView->acceptRowSelection(true);
        restrictColumns(m_atcView, { "cs.", "", "frequency", "distance" });
        // Hinweis: die Icon-Spalte ("leer") hat einen leeren Spaltennamen ""

        m_aircraftView = new CSimulatedAircraftView(this);
        m_aircraftView->setAircraftMode(CSimulatedAircraftListModel::RenderedMode);
        restrictColumns(m_aircraftView, { "e.", "r.", "cs.", "dist.", "altitude", "CG", "gs.", "p.", "realname" });

        QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(m_atcView);
        splitter->addWidget(m_aircraftView);
        // ein Drittel links (ATC), zwei Drittel rechts (Piloten)
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 2);
        splitter->setSizes({ 1, 2 });

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(splitter);
        this->setLayout(layout);

        // keine Abhaengigkeit zu CDockWidgetInfoArea/CInfoArea:
        // direktes Verdrahten mit dem Netzwerkkontext, kein interner
        // Sichtbarkeits-Check (anders als CAtcStationComponent/CAircraftComponent)
        if (sGui && sGui->getIContextNetwork())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnlineDigest, this,
                    &CMiniClientsWidget::onChangedAtcStationsOnline, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAircraftInRangeDigest, this,
                    &CMiniClientsWidget::onChangedAircraftInRange, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                    &CMiniClientsWidget::onConnectionStatusChanged, Qt::QueuedConnection);
        }

        this->update();
    }

    CMiniClientsWidget::~CMiniClientsWidget() = default;

    CAtcStationList CMiniClientsWidget::sortAtcStations(CAtcStationList stations) const
    {
        const int mode = m_atcSortSetting.get();
        if (mode == SortByCategory)
        {
            // Kategorie = Buchstaben nach dem letzten Unterstrich im Rufzeichen (z.B. EDDF_TWR -> TWR)
            stations.sortBy(&CAtcStation::getCallsignSuffix, &CAtcStation::getRelativeDistance);
        }
        else
        {
            // nach Entfernung ("Distance")
            stations.sortBy(&CAtcStation::getRelativeDistance);
        }
        return stations;
    }

    void CMiniClientsWidget::update()
    {
        if (!sGui || !sGui->getIContextNetwork() || sGui->isShuttingDown()) { return; }

        if (sGui->getIContextNetwork()->isConnected())
        {
            CAtcStationList onlineStations = sGui->getIContextNetwork()->getAtcStationsOnline(true);
            onlineStations = this->sortAtcStations(onlineStations);
            m_atcView->updateContainerMaybeAsync(onlineStations, false); // bereits sortiert, nicht erneut sortieren

            const CSimulatedAircraftList aircraft = sGui->getIContextNetwork()->getAircraftInRange();
            m_aircraftView->updateContainerMaybeAsync(aircraft);
        }
        else
        {
            m_atcView->clear();
            m_aircraftView->clear();
        }
    }

    void CMiniClientsWidget::onChangedAtcStationsOnline() { this->update(); }

    void CMiniClientsWidget::onChangedAircraftInRange() { this->update(); }

    void CMiniClientsWidget::onConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        if (!to.isConnected())
        {
            m_atcView->clear();
            m_aircraftView->clear();
        }
        else { this->update(); }
    }
} // namespace swift::gui::components