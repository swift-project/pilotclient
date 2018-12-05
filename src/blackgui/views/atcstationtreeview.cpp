/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/views/atcstationtreeview.h"
#include "blackgui/models/atcstationtreemodel.h"
#include "blackgui/menus/menuaction.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/icons.h"
#include "blackconfig/buildconfig.h"

#include <QFlags>
#include <QMenu>
#include <QtGlobal>
#include <QAction>
#include <QModelIndex>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CAtcStationTreeView::CAtcStationTreeView(QWidget *parent) : QTreeView(parent)
        {
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &CAtcStationTreeView::customContextMenuRequested, this, &CAtcStationTreeView::customMenu);
        }

        void CAtcStationTreeView::changedAtcStationConnectionStatus(const CAtcStation &station, bool added)
        {
            Q_UNUSED(station);
            Q_UNUSED(added);
        }

        const CAtcStationTreeModel *CAtcStationTreeView::stationModel() const
        {
            return qobject_cast<const CAtcStationTreeModel *>(this->model());
        }

        CAtcStation CAtcStationTreeView::selectedObject() const
        {
            const QModelIndex index = this->currentIndex();
            const QVariant data = this->model()->data(index.siblingAtColumn(0)); // supposed to be the callsign
            const QString callsign = data.toString();
            const CAtcStationTreeModel *model = this->stationModel();
            if (!model) { return CAtcStation(); }
            return model->container().findFirstByCallsign(CCallsign(callsign, CCallsign::Atc));
         }

        void CAtcStationTreeView::customMenu(const QPoint &point)
        {
            QMenu *menu = new QMenu(this);  // menu

            QAction *com1 = new QAction(CIcons::appCockpit16(), "Tune in COM1", this);
            QAction *com2 = new QAction(CIcons::appCockpit16(), "Tune in COM2", this);
            QAction *text = new QAction(CIcons::appTextMessages16(), "Show text messages", this);

            connect(com1, &QAction::triggered, this, &CAtcStationTreeView::tuneInAtcCom1);
            connect(com2, &QAction::triggered, this, &CAtcStationTreeView::tuneInAtcCom2);
            connect(text, &QAction::triggered, this, &CAtcStationTreeView::requestTextMessage);

            menu->addAction(com1);
            menu->addAction(com2);
            menu->addAction(text);

            menu->popup(this->viewport()->mapToGlobal(point));
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
} // namespace
