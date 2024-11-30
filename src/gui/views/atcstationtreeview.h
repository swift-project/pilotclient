// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_ATCSTATIONTREEVIEW_H
#define SWIFT_GUI_VIEWS_ATCSTATIONTREEVIEW_H

#include <QList>
#include <QMap>
#include <QModelIndex>
#include <QObject>
#include <QPoint>
#include <QTreeView>

#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/comsystem.h"
#include "misc/digestsignal.h"
#include "misc/pq/frequency.h"

namespace swift::gui
{
    namespace models
    {
        class CAtcStationTreeModel;
        class CColumns;
    } // namespace models

    namespace views
    {
        //! ATC stations view
        class SWIFT_GUI_EXPORT CAtcStationTreeView : public COverlayMessagesTreeView
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAtcStationTreeView(QWidget *parent = nullptr);

            //! \copydoc models::CAtcStationListModel::changedAtcStationConnectionStatus
            void changedAtcStationConnectionStatus(const swift::misc::aviation::CAtcStation &station, bool added);

            //! Update container
            void updateContainer(const swift::misc::aviation::CAtcStationList &stations);

            //! Clear
            void clear();

            //! Empty
            bool isEmpty() const;

            //! Set columns
            void setColumns(const models::CColumns &columns);

            //! Resize all columns
            void fullResizeToContents();

        signals:
            //! Request some dummy ATC stations
            void testRequestDummyAtcOnlineStations(int number);

            //! Request COM frequency
            void requestComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                     swift::misc::aviation::CComSystem::ComUnit unit);

            //! Request a text message to
            void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);

            //! This object has been selected
            void objectSelected(const swift::misc::aviation::CAtcStation &station);

        private:
            //! Used model
            const models::CAtcStationTreeModel *stationModel() const;

            //! Used model
            models::CAtcStationTreeModel *stationModel();

            //! Resize all columns
            void fullResizeToContentsImpl();

            //! The selected object
            swift::misc::aviation::CAtcStation selectedObject() const;

            //! The selected object
            swift::misc::aviation::CAtcStation selectedObject(const QModelIndex &index) const;

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

            swift::misc::CDigestSignal m_dsFullResize { this, &CAtcStationTreeView::fullResizeToContentsImpl,
                                                        std::chrono::milliseconds(1000), 25 };
            QMap<QString, bool> m_expanded; //!< suffix/expanded
        };
    } // namespace views
} // namespace swift::gui

#endif // guard
