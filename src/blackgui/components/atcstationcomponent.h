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

#include "blackgui/components/runtimebasedcomponent.h"
#include "blackgui/components/timerbasedcomponent.h"
#include "blackmisc/avatcstation.h"

#include <QTabWidget>
#include <QModelIndex>

namespace Ui { class CAtcStationComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * ATC stations component
         */
        class CAtcStationComponent : public QTabWidget, public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAtcStationComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CAtcStationComponent();

            //! Timer for updating
            CTimerBasedComponent *getTimerComponent() { return this->m_timerComponent; }

        public slots:
            //! Update users
            void update();

            //! \copydoc CTimerBasedComponent::setUpdateIntervalSeconds
            void setUpdateIntervalSeconds(int seconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateIntervalSeconds(seconds); }

            //! \copydoc CTimerBasedComponent::setUpdateInterval
            void setUpdateInterval(int milliSeconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateInterval(milliSeconds); }

            //! \copydoc CTimerBasedComponent::stopTimer
            void stopTimer() { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->stopTimer(); }

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
            void ps_connectionStatusChanged(uint from, uint to, const QString &message);

        private:
            Ui::CAtcStationComponent *ui;
            CTimerBasedComponent *m_timerComponent;
            QDateTime m_timestampLastReadOnlineStations = CTimerBasedComponent::epoch();  //!< stations read
            QDateTime m_timestampOnlineStationsChanged = CTimerBasedComponent::epoch();   //!< stations marked as changed
            QDateTime m_timestampLastReadBookedStations = CTimerBasedComponent::epoch();  //!< stations read
            QDateTime m_timestampBookedStationsChanged = CTimerBasedComponent::epoch();   //!< stations marked as changed
        };
    }
}
#endif // guard
