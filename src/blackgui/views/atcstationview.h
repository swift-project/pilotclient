/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ATCSTATIONVIEW_H
#define BLACKGUI_ATCSTATIONVIEW_H

#include "blackgui/views/viewcallsignobjects.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/pq/frequency.h"

#include <QList>
#include <QObject>

class QAction;

namespace BlackMisc::Aviation
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
            void changedAtcStationConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

        signals:
            //! Request some dummy ATC stations
            void testRequestDummyAtcOnlineStations(int number);

            //! Request COM frequency
            void requestComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit);

            //! Request a text message to
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

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
