/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ATCSTATIONVIEW_H
#define BLACKGUI_ATCSTATIONVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/views/viewcallsignobjects.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/pq/frequency.h"

#include <QList>
#include <QObject>

class QAction;
class QWidget;

namespace BlackMisc { namespace Aviation { class CCallsign; } }
namespace BlackGui
{
    namespace Menus { class CMenuActions; }
    namespace Views
    {
        //! ATC stations view
        class BLACKGUI_EXPORT CAtcStationView : public CViewWithCallsignObjects<BlackGui::Models::CAtcStationListModel, BlackMisc::Aviation::CAtcStationList, BlackMisc::Aviation::CAtcStation>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAtcStationView(QWidget *parent = nullptr);

            //! Set station mode
            void setStationMode(BlackGui::Models::CAtcStationListModel::AtcStationMode stationMode);

        signals:
            //! Request some dummy ATC stations
            void testRequestDummyAtcOnlineStations(int number);

            //! Request COM frequency
            void requestComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit);

            //! Request a text message to
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        public slots:
            //! \copydoc Models::CAtcStationListModel::changedAtcStationConnectionStatus
            void changedAtcStationConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

        private slots:
            void ps_testRequest1kAtcOnlineDummies() { emit this->testRequestDummyAtcOnlineStations(1000); }
            void ps_testRequest3kAtcOnlineDummies() { emit this->testRequestDummyAtcOnlineStations(3000); }
            void ps_tuneInAtcCom1();
            void ps_tuneInAtcCom2();
            void ps_requestTextMessage();

        private:
            QList<QAction *> m_actions;
            QList<QAction *> m_debugActions;
        };
    }
}
#endif // guard
