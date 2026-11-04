// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniatcstationwidget.h"

#include <QVBoxLayout>
#include <QtGlobal>

#include "core/context/contextnetwork.h"
#include "gui/guiapplication.h"
#include "gui/models/atcstationlistmodel.h"
#include "gui/views/atcstationview.h"
#include "misc/aviation/atcstationlist.h"

using namespace swift::core::context;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::gui::models;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CMiniAtcStationWidget::CMiniAtcStationWidget(QWidget *parent) : QFrame(parent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

        m_view = new CAtcStationView(this);
        m_view->setStationMode(CAtcStationListModel::StationsOnline);
        m_view->acceptClickSelection(true);
        m_view->acceptRowSelection(true);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_view);
        this->setLayout(layout);

        // keine Abhaengigkeit zu CDockWidgetInfoArea/CInfoArea:
        // direktes Verdrahten mit dem Netzwerkkontext, kein interner
        // Sichtbarkeits-Check (anders als CAtcStationComponent::update())
        if (sGui && sGui->getIContextNetwork())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnlineDigest, this,
                    &CMiniAtcStationWidget::onChangedAtcStationsOnline, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                    &CMiniAtcStationWidget::onConnectionStatusChanged, Qt::QueuedConnection);
        }

        this->update();
    }

    CMiniAtcStationWidget::~CMiniAtcStationWidget() = default;

    int CMiniAtcStationWidget::countOnlineStations() const { return m_view->rowCount(); }

    void CMiniAtcStationWidget::update()
    {
        if (!sGui || !sGui->getIContextNetwork() || sGui->isShuttingDown()) { return; }

        if (sGui->getIContextNetwork()->isConnected())
        {
            const CAtcStationList onlineStations = sGui->getIContextNetwork()->getAtcStationsOnline(true);
            m_view->updateContainerMaybeAsync(onlineStations);
        }
        else { m_view->clear(); }
    }

    void CMiniAtcStationWidget::onChangedAtcStationsOnline() { this->update(); }

    void CMiniAtcStationWidget::onConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        if (!to.isConnected()) { m_view->clear(); }
        else { this->update(); }
    }
} // namespace swift::gui::components