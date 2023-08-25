// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_ATCSTATIONTREEVIEW_H
#define BLACKGUI_VIEWS_ATCSTATIONTREEVIEW_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/digestsignal.h"

#include <QTreeView>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QMap>
#include <QModelIndex>

namespace BlackGui
{
    namespace Models
    {
        class CAtcStationTreeModel;
        class CColumns;
    }

    namespace Views
    {
        //! ATC stations view
        class BLACKGUI_EXPORT CAtcStationTreeView : public COverlayMessagesTreeView
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAtcStationTreeView(QWidget *parent = nullptr);

            //! \copydoc Models::CAtcStationListModel::changedAtcStationConnectionStatus
            void changedAtcStationConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

            //! Update container
            void updateContainer(const BlackMisc::Aviation::CAtcStationList &stations);

            //! Clear
            void clear();

            //! Empty
            bool isEmpty() const;

            //! Set columns
            void setColumns(const Models::CColumns &columns);

            //! Resize all columns
            void fullResizeToContents();

        signals:
            //! Request some dummy ATC stations
            void testRequestDummyAtcOnlineStations(int number);

            //! Request COM frequency
            void requestComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit);

            //! Request a text message to
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

            //! This object has been selected
            void objectSelected(const BlackMisc::Aviation::CAtcStation &station);

        private:
            //! Used model
            const Models::CAtcStationTreeModel *stationModel() const;

            //! Used model
            Models::CAtcStationTreeModel *stationModel();

            //! Resize all columns
            void fullResizeToContentsImpl();

            //! The selected object
            BlackMisc::Aviation::CAtcStation selectedObject() const;

            //! The selected object
            BlackMisc::Aviation::CAtcStation selectedObject(const QModelIndex &index) const;

            //! Suffix for index
            QString suffixForIndex(const QModelIndex &index);

            //! Expanded
            void onExpanded(const QModelIndex &index);

            //! Selected
            void onSelected(const QItemSelection &selected, const QItemSelection &deselected);

            //! Custom menu
            void customMenu(const QPoint &point);

            //! Store state
            void storeState();

            //! Restore state
            void restoreState();

            //! @{
            //! Tune in/invoke
            void tuneInAtcCom1();
            void tuneInAtcCom2();
            void requestTextMessage();
            //! @}

            BlackMisc::CDigestSignal m_dsFullResize { this, &CAtcStationTreeView::fullResizeToContentsImpl, 1000, 25 };
            QMap<QString, bool> m_expanded; //!< suffix/expanded
        };
    } // ns
} // ns

#endif // guard
