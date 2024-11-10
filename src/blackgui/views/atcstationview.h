// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_ATCSTATIONVIEW_H
#define BLACKGUI_ATCSTATIONVIEW_H

#include "blackgui/views/viewcallsignobjects.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/blackguiexport.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/comsystem.h"
#include "misc/pq/frequency.h"

#include <QList>
#include <QObject>

class QAction;

namespace swift::misc::aviation
{
    class CCallsign;
}
namespace BlackGui
{
    namespace Menus
    {
        class CMenuActions;
    }
    namespace Views
    {
        //! ATC stations view
        class BLACKGUI_EXPORT CAtcStationView : public CViewWithCallsignObjects<BlackGui::Models::CAtcStationListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAtcStationView(QWidget *parent = nullptr);

            //! Set station mode
            void setStationMode(BlackGui::Models::CAtcStationListModel::AtcStationMode stationMode);

            //! \copydoc Models::CAtcStationListModel::changedAtcStationConnectionStatus
            void changedAtcStationConnectionStatus(const swift::misc::aviation::CAtcStation &station, bool added);

        signals:
            //! Request some dummy ATC stations
            void testRequestDummyAtcOnlineStations(int number);

            //! Request COM frequency
            void requestComFrequency(const swift::misc::physical_quantities::CFrequency &frequency, swift::misc::aviation::CComSystem::ComUnit unit);

            //! Request a text message to
            void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

        private:
            void emitTestRequest1kAtcOnlineDummies() { emit this->testRequestDummyAtcOnlineStations(1000); }
            void emitTestRequest3kAtcOnlineDummies() { emit this->testRequestDummyAtcOnlineStations(3000); }
            void tuneInAtcCom1();
            void tuneInAtcCom2();
            void requestTextMessage();

            QList<QAction *> m_actions; //!< real actions
            QList<QAction *> m_debugActions; //!< used for debug context menu
        };
    }
} // ns

#endif // guard
