/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTCOMPONENT_H
#define BLACKGUI_AIRCRAFTCOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/components/updatetimer.h"
#include "blackmisc/aviation/aircraft.h"

#include <QTabWidget>
#include <QScopedPointer>

namespace Ui { class CAircraftComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Aircraft widget
        class CAircraftComponent :
            public QTabWidget,
            public CEnableForDockWidgetInfoArea,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CAircraftComponent();

            //! Timer for updating
            CUpdateTimer *getTimerComponent() { return this->m_updateTimer; }

            //! Aircraft in range
            int countAircraft() const;

            //! Airports in range
            int countAirportsInRange() const;

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        public slots:
            //! Update aircrafts
            void update();

            //! \copydoc CTimerBasedComponent::setUpdateIntervalSeconds
            void setUpdateIntervalSeconds(int seconds) { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->setUpdateIntervalSeconds(seconds); }

            //! \copydoc CTimerBasedComponent::setUpdateInterval
            void setUpdateInterval(int milliSeconds) { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->setUpdateInterval(milliSeconds); }

            //! \copydoc CTimerBasedComponent::stopTimer
            void stopTimer() { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->stopTimer(); }

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private slots:
            //! Info area tab bar has changed
            void ps_infoAreaTabBarChanged(int index);

            //! Number of elements changed
            void ps_onRowCountChanged(int count, bool withFilter);

            //! Connection status has been changed
            void ps_connectionStatusChanged(int from, int to);

            //! Highlight in simulator
            void ps_onMenuHighlightInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        private:
            QScopedPointer<Ui::CAircraftComponent> ui;
            CUpdateTimer *m_updateTimer = nullptr;
        };
    } // ns
} // ns

#endif // guard
