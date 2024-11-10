// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/atcstationtreeview.h"
#include "gui/models/atcstationtreemodel.h"
#include "gui/menus/menuaction.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/icons.h"
#include "config/buildconfig.h"

#include <QFlags>
#include <QMenu>
#include <QtGlobal>
#include <QAction>
#include <QModelIndex>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CAtcStationTreeView::CAtcStationTreeView(QWidget *parent) : COverlayMessagesTreeView(parent)
    {
        this->setModel(new CAtcStationTreeModel(this));
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &CAtcStationTreeView::customContextMenuRequested, this, &CAtcStationTreeView::customMenu);
        connect(this, &CAtcStationTreeView::expanded, this, &CAtcStationTreeView::onExpanded, Qt::QueuedConnection);
        connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CAtcStationTreeView::onSelected, Qt::QueuedConnection);
    }

    void CAtcStationTreeView::changedAtcStationConnectionStatus(const CAtcStation &station, bool added)
    {
        if (!this->stationModel()) { return; }
        this->stationModel()->changedAtcStationConnectionStatus(station, added);
    }

    void CAtcStationTreeView::updateContainer(const CAtcStationList &stations)
    {
        if (!this->stationModel()) { return; }
        this->storeState();
        this->stationModel()->updateContainer(stations);
        this->restoreState();
    }

    void CAtcStationTreeView::clear()
    {
        if (!this->stationModel()) { return; }
        this->stationModel()->clear();
    }

    bool CAtcStationTreeView::isEmpty() const
    {
        return this->stationModel()->rowCount() < 1;
    }

    void CAtcStationTreeView::setColumns(const CColumns &columns)
    {
        if (this->stationModel()) { this->stationModel()->setColumns(columns); }
    }

    void CAtcStationTreeView::fullResizeToContents()
    {
        m_dsFullResize.inputSignal();
    }

    void CAtcStationTreeView::fullResizeToContentsImpl()
    {
        if (this->isEmpty()) { return; }
        for (int c = 0; c < this->model()->columnCount(); c++)
        {
            this->resizeColumnToContents(c);
        }
    }

    const CAtcStationTreeModel *CAtcStationTreeView::stationModel() const
    {
        return qobject_cast<const CAtcStationTreeModel *>(this->model());
    }

    CAtcStationTreeModel *CAtcStationTreeView::stationModel()
    {
        return qobject_cast<CAtcStationTreeModel *>(this->model());
    }

    CAtcStation CAtcStationTreeView::selectedObject() const
    {
        const QModelIndex index = this->currentIndex();
        return this->selectedObject(index);
    }

    CAtcStation CAtcStationTreeView::selectedObject(const QModelIndex &index) const
    {
        const QVariant data = this->model()->data(index.siblingAtColumn(0)); // supposed to be the callsign
        const QString callsign = data.toString();
        const CAtcStationTreeModel *model = this->stationModel();
        if (!model) { return CAtcStation(); }
        return model->container().findFirstByCallsign(CCallsign(callsign, CCallsign::Atc));
    }

    QString CAtcStationTreeView::suffixForIndex(const QModelIndex &index)
    {
        const QVariant data = this->model()->data(index); // supposed to be the suffix
        return data.toString();
    }

    void CAtcStationTreeView::onExpanded(const QModelIndex &index)
    {
        Q_UNUSED(index);
        this->fullResizeToContents();
    }

    void CAtcStationTreeView::onSelected(const QItemSelection &selected, const QItemSelection &deselected)
    {
        Q_UNUSED(deselected);
        if (selected.isEmpty()) { return; }
        const CAtcStation atcStation = this->selectedObject(selected.indexes().front());
        if (!atcStation.hasCallsign()) { return; }
        emit this->objectSelected(atcStation);
    }

    void CAtcStationTreeView::customMenu(const QPoint &point)
    {
        if (!this->stationModel()) { return; }
        if (this->stationModel()->container().isEmpty()) { return; }

        QMenu *menu = new QMenu(this); // menu

        QAction *com1 = new QAction(CIcons::appCockpit16(), "Tune in COM1", this);
        QAction *com2 = new QAction(CIcons::appCockpit16(), "Tune in COM2", this);
        QAction *text = new QAction(CIcons::appTextMessages16(), "Show text messages", this);
        QAction *resize = new QAction(CIcons::resize16(), "Resize", this);

        connect(com1, &QAction::triggered, this, &CAtcStationTreeView::tuneInAtcCom1);
        connect(com2, &QAction::triggered, this, &CAtcStationTreeView::tuneInAtcCom2);
        connect(text, &QAction::triggered, this, &CAtcStationTreeView::requestTextMessage);
        connect(resize, &QAction::triggered, this, &CAtcStationTreeView::fullResizeToContentsImpl);

        menu->addAction(com1);
        menu->addAction(com2);
        menu->addAction(text);
        menu->addSeparator();
        menu->addAction(resize);

        menu->popup(this->viewport()->mapToGlobal(point));
    }

    void CAtcStationTreeView::storeState()
    {
        m_expanded.clear();
        for (int row = 0; row < this->model()->rowCount(); ++row)
        {
            const QModelIndex i = this->model()->index(row, 0);
            const bool expanded = this->isExpanded(i);
            const QString suffix = this->suffixForIndex(i);
            m_expanded.insert(suffix, expanded);
        }
    }

    void CAtcStationTreeView::restoreState()
    {
        for (int row = 0; row < this->model()->rowCount(); ++row)
        {
            const QModelIndex i = this->model()->index(row, 0);
            const QString suffix = this->suffixForIndex(i); // suffix of new data
            const bool expanded = m_expanded.value(suffix, true); // default expanded
            this->setExpanded(i, expanded);
        }
    }

    void CAtcStationTreeView::tuneInAtcCom1()
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestComFrequency(s.getFrequency(), CComSystem::Com1);
    }

    void CAtcStationTreeView::tuneInAtcCom2()
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestComFrequency(s.getFrequency(), CComSystem::Com2);
    }

    void CAtcStationTreeView::requestTextMessage()
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestTextMessageWidget(s.getCallsign());
    }
} // namespace
