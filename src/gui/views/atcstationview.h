// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_ATCSTATIONVIEW_H
#define SWIFT_GUI_ATCSTATIONVIEW_H

#include <QList>
#include <QObject>

#include "gui/models/atcstationlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewcallsignobjects.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/comsystem.h"
#include "misc/pq/frequency.h"

class QAction;

namespace swift::misc::aviation
{
    class CCallsign;
}
namespace swift::gui
{
    namespace menus
    {
        class CMenuActions;
    }
    namespace views
    {
        //! ATC stations view
        class SWIFT_GUI_EXPORT CAtcStationView :
            public CViewWithCallsignObjects<swift::gui::models::CAtcStationListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAtcStationView(QWidget *parent = nullptr);

            //! Set station mode
            void setStationMode(swift::gui::models::CAtcStationListModel::AtcStationMode stationMode);

            //! \copydoc models::CAtcStationListModel::changedAtcStationConnectionStatus
            void changedAtcStationConnectionStatus(const swift::misc::aviation::CAtcStation &station, bool added);

        signals:
            //! Request COM frequency
            void requestComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                     swift::misc::aviation::CComSystem::ComUnit unit);

            //! Request a text message to
            void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(swift::gui::menus::CMenuActions &menuActions) override;

        private:
            void tuneInAtcCom1();
            void tuneInAtcCom2();
            void requestTextMessage();

            QList<QAction *> m_actions; //!< real actions
        };
    } // namespace views
} // namespace swift::gui

#endif // SWIFT_GUI_ATCSTATIONVIEW_H
