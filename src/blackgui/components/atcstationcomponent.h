/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ATCSTATIONCOMPONENT_H
#define BLACKGUI_ATCSTATIONCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/components/updatetimer.h"
#include "blackcore/network.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/aviation/atcstation.h"
#include <QTabWidget>
#include <QModelIndex>
#include <QScopedPointer>

namespace Ui { class CAtcStationComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! ATC stations component
        class BLACKGUI_EXPORT CAtcStationComponent :
            public QTabWidget,
            public CEnableForDockWidgetInfoArea,
            public CEnableForRuntime,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAtcStationComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAtcStationComponent();

            //! Number of booked stations
            int countBookedStations() const;

            //! Number of online stations
            int countOnlineStations() const;

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        public slots:
            //! Update stations
            void update();

            //! \copydoc CTimerBasedComponent::setUpdateIntervalSeconds
            void setUpdateIntervalSeconds(int seconds) { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->setUpdateIntervalSeconds(seconds); }

            //! \copydoc CTimerBasedComponent::stopTimer
            void stopTimer() { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->stopTimer(); }

            //! Get METAR for given ICAO airport code
            void getMetar(const QString &airportIcaoCode = "");

            //! \copydoc CAtcStationListModel::changedAtcStationConnectionStatus
            void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private slots:
            //! Request new ATIS
            void ps_requestAtis();

            //! Online ATC station selected
            void ps_onlineAtcStationSelected(QModelIndex index);

            //! Tab changed
            void ps_atcStationsTabChanged();

            //! Booked stations reloading
            void ps_reloadAtcStationsBooked();

            //! Booked stations changed
            void ps_changedAtcStationsBooked();

            //! Online stations changed
            void ps_changedAtcStationsOnline();

            //! Connection status has been changed
            void ps_connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Request dummy ATC online stations
            void ps_testCreateDummyOnlineAtcStations(int number);

            //! Request udpate
            void ps_requestOnlineStationsUpdate();

            //! Info area tab bar has changed
            void ps_infoAreaTabBarChanged(int index);

            //! Count has been changed
            void ps_onCountChanged(int count, bool withFilter);

            //! Set COM frequency
            void ps_setComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit);

        private:
            void updateTreeView();
            QScopedPointer<Ui::CAtcStationComponent> ui;
            QScopedPointer<CUpdateTimer> m_updateTimer;
            QDateTime m_timestampLastReadOnlineStations = CUpdateTimer::epoch();  //!< stations read
            QDateTime m_timestampOnlineStationsChanged  = CUpdateTimer::epoch();  //!< stations marked as changed
            QDateTime m_timestampLastReadBookedStations = CUpdateTimer::epoch();  //!< stations read
            QDateTime m_timestampBookedStationsChanged  = CUpdateTimer::epoch();  //!< stations marked as changed
        };
    } // namespace
} // namespace
#endif // guard
